#include "core/v.h"
#include "core/threading/vthread.h"
#include "core/vlogging.h"
#include "core/vtimer.h"
#include "core/vevent_queue.h"
#include "core/vutils.h"
#include "core/threading/vmutex.h"

#include "graphics/vrenderer.h"
#include "graphics/vrender_api.h"

#include "threading/vthread_sync_barrier.h"
#include "ui/vui_element.h"
#include "ui/_vui_element.h"

#include "platform/vwindow.h"

#include <locale.h>
#include <stdatomic.h>

typedef struct V {
    vWindow window;
    vRenderer renderer;

    vThreadSyncBarrier init_barrier;
    vThread renderer_thread;
    vThread main_thread;
    void (*application_run)(void);
    void (*application_init)(void);
    
    atomic_bool is_running;
    u64 target_fps;
    u64 speed;
} V;

V v = {
    .target_fps = 60,
    .speed = 50,
};

vUIElement* button;
vTextureData texture_data1;
vTextureData texture_data2;

static void* vRendererThread(void* arg) {
    vLog("[RENDERER THREAD] Starting renderer initialization\n");
    v.renderer = vRendererCreate(v.window);

    vRenderAPIInit(v.renderer);
    vLog("[RENDERER THREAD] Render API ready\n");
    v.application_init();

    vTimer render_timer = vTimerCreate(true);

    vThreadSyncBarrierConfirm(v.init_barrier, v.renderer_thread);

    vTexture texture1 = vTextureCreate(texture_data1);
    vTexture texture2 = vTextureCreate(texture_data2);
    vLog("[RENDERER THREAD] Startup textures created: pressed=%p hover=%p\n", texture1, texture2);
    
    vUIElementSetTexture(button, V_UIE_TEXTURE_PRESSED, texture1);
    vUIElementSetTexture(button, V_UIE_TEXTURE_HOVER, texture2);
    u32 next_rcmd_buf = 0;

    while(atomic_load(&v.is_running)) {
        vArray elements = vUIElementGetAllRegisteredElements();
        u32 element_count = vUIElementGetRegisteredElementCount();
        u64 start_ms = vTimerGetMs(render_timer);
        vRendererBeginFrame(v.renderer);
       
        vRendererExecuteCommands(
            v.renderer,
            vRenderAPIGetCommandBuffer(next_rcmd_buf)
        );

        next_rcmd_buf = (next_rcmd_buf + 1) % 2;

        for(u32 i = 0; i < element_count; ++i) {
            vTexture texture = NULL;
            vUIElement *element = *(vUIElement**)vArrayGet(elements, i);
            
            if(element->is_visible && element->use_texture) {
                if(element->is_pressed && element->pressed_texture) 
                    texture = element->pressed_texture;
                else if(element->is_hovered && element->hover_texture)
                    texture = element->hover_texture;
                else if(element->normal_texture)
                    texture = element->normal_texture;
            }
            if(texture) {
                vRendererDrawTexture(
                    v.renderer,
                    texture,
                    (vVec2){ (f32)element->x, (f32)element->y },
                    (vVec2){ (f32)element->width, (f32)element->height }
                );
            }
        }

        vRendererEndFrame(v.renderer);

        u64 end_ms = vTimerGetMs(render_timer);

        u64 elapsed_ms = end_ms - start_ms;
        if(elapsed_ms < 1'000 / v.target_fps) {
            vThreadSleep(1'000 / v.target_fps - elapsed_ms);
        }
    }

    vLog("[RENDERER THREAD] Thread finished\n"); 
    
    return NULL;
}

static void vDispatchEvents() {
    vLog("[ENGINE] Dispatching events\n");
    vEventQueuePublish((vEvent){ .type = V_EVENT_ENGINE_TICK });
    vEventQueueDispatch();
}

static void vUIEventDispatch(const vEvent *ev) {
    vArray elements = vUIElementGetAllRegisteredElements();
    u32 count = vUIElementGetRegisteredElementCount();

    for(u32 i = 0; i < count; ++i) {
        vUIElement *element = *(vUIElement**)vArrayGet(elements, i);
        if(!element->is_enabled) continue;
        bool is_hovered = 
            vPointInsideRectangleF32(
                (vVec2){ (f32)ev->mouse_x, (f32)ev->mouse_y },
                (vVec2){ (f32)element->x, (f32)element->y },
                (vVec2){ (f32)element->width, (f32)element->height }
            );
        
        if(ev->type == V_EVENT_MOUSE_MOVED) {
            // To prevent multiple calls to on_hover when the mouse is already over the element
            if(is_hovered && !element->is_hovered) { 
                element->is_hovered = true;
                
                if(element->on_hover) {
                    element->on_hover();
                }
            } else if(!is_hovered && element->is_hovered) {
                element->is_hovered = false;
            }
        }

        if(ev->type == V_EVENT_MOUSE_LBUTTON) {
            if(element->is_hovered && ev->as.mouse_lbutton.pressed) {
                element->is_pressed = true;
                if(element->on_press) element->on_press();
                
            } else if(element->is_pressed && ev->as.mouse_lbutton.released) {
                element->is_pressed = false;
                if(element->on_release) {
                    element->on_release();
                }
                if(element->on_click && element->is_hovered) element->on_click();
                
            }
        }
    }
    
}

static void* vMainThread(void* arg) {
    vLog("[MAIN THREAD] Started\n");

    vThreadSyncBarrierConfirm(v.init_barrier, v.main_thread);

    vTimer main_timer = vTimerCreate(true);

    while(atomic_load(&v.is_running)) {
        u64 start_ms = vTimerGetMs(main_timer);
        
        vDispatchEvents();
        // vUpdateUI(); 
        if(v_render.is_ready) {
            v.application_run();
        }

        u64 end_ms = vTimerGetMs(main_timer);
        u64 elapsed_ms = end_ms - start_ms;

        if(elapsed_ms < 1'000 / v.speed) {
            vThreadSleep(1'000 / v.speed - elapsed_ms);
        }
    }

    vLog("[MAIN THREAD] Thread finished\n");
    return NULL;
}

static void vOnWindowClosed(const vEvent *ev) {
    vLog("[ENGINE] Window is closed\n");
    atomic_store(&v.is_running, false);
}

void vInit(vString title, u32 w, u32 h) {
    vLog("[ENGINE] Initializing (%ux%u)\n", w, h);
    setlocale(LC_ALL, "");

    vEventQueueInit();
    vLog("[ENGINE] Event queue initialized\n");
    
    vUIElementRegistryInit();
    vLog("[ENGINE] UI registry initialized\n");

    vEventQueueSubscribe(V_EVENT_WINDOW_CLOSED, vOnWindowClosed);
    
    vEventQueueSubscribe(V_EVENT_MOUSE_LBUTTON, vUIEventDispatch);
    vEventQueueSubscribe(V_EVENT_MOUSE_RBUTTON, vUIEventDispatch);
    vEventQueueSubscribe(V_EVENT_MOUSE_MBUTTON, vUIEventDispatch);
    vEventQueueSubscribe(V_EVENT_MOUSE_MOVED, vUIEventDispatch);
    vEventQueueSubscribe(V_EVENT_KEY_PRESSED, vUIEventDispatch);
    vEventQueueSubscribe(V_EVENT_KEY_RELEASED, vUIEventDispatch);

    texture_data1 = vTextureLoad("assets/textures/test.png");
    texture_data2 = vTextureLoad("assets/textures/test2.png");
    vLog("[ENGINE] Startup texture data loaded: first=%p second=%p\n", texture_data1, texture_data2);

    button = vUIElementCreate(
        100, 100, 100, 50,
        true,
        NULL,
        NULL,
        NULL,
        NULL
    );
    
    vUIElementRegister(button);
    vLog("[ENGINE] Startup UI element registered: %p\n", button);

    v.window = vWindowCreate(title, w, h);
    if (!v.window) {
        vLog("[ENGINE] Window creation failed\n");
        return;
    }
    vLog("[ENGINE] Window created: %p\n", v.window);

    v.renderer_thread = vThreadCreate(vRendererThread, NULL);
    v.main_thread = vThreadCreate(vMainThread, NULL);
    vLog("[ENGINE] Worker threads created: main=%p renderer=%p\n", v.main_thread, v.renderer_thread);

    v.init_barrier = vThreadSyncBarrierCreate();
    vThreadSyncBarrierAdd(v.init_barrier, v.renderer_thread);
    vThreadSyncBarrierAdd(v.init_barrier, v.main_thread);
    vThreadSyncBarrierStart(v.init_barrier);
}

void vRun(void (*application_init)(void), void (*application_run)(void)) {
    vLog("[ENGINE] Starting run loop\n");
    atomic_store(&v.is_running, true);
    
    v.application_run = application_run;
    v.application_init = application_init;

    vThreadStart(v.renderer_thread);
    vThreadStart(v.main_thread);
    vLog("[ENGINE] Worker threads started\n");

    while(atomic_load(&v.is_running)) {
        vWindowPollEvents();
    }

    vThreadJoin(v.renderer_thread);
    vThreadJoin(v.main_thread);

    vLog("[ENGINE] Run loop stopped; worker threads joined\n");
    vLog("[WINDOW THREAD] Thread finished\n");
}

void vShutdown() {
    vLog("[ENGINE] Shutdown requested\n");
}

bool vIsRunning() {
    return v.is_running;
}

vRenderAPI* vGetRenderAPI() {
    return &v_render;
}

void vAddEventListener(vEventType type, void (*callback)(const vEvent*)) {
    vLog("[ENGINE] Adding listener %p for event %d\n", callback, type);
    vEventQueueSubscribe(type, callback);
}

void vRemoveEventListener(vEventType type, void (*callback)(const vEvent*)) {
    // vEventQueueUnsubscribe(type, callback);
}