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
    v.renderer = vRendererCreate(v.window);

    vRenderAPIInit(v.renderer);
    v.application_init();

    vTimer render_timer = vTimerCreate(true);

    vThreadSyncBarrierConfirm(v.init_barrier, v.renderer_thread);

    vTexture texture1 = vTextureCreate(texture_data1);
    vTexture texture2 = vTextureCreate(texture_data2);
    
    vUIElementSetTexture(button, V_UIE_TEXTURE_PRESSED, texture1);
    vUIElementSetTexture(button, V_UIE_TEXTURE_HOVER, texture2);
    u32 next_rcmd_buf = 0;

    while(atomic_load(&v.is_running)) {
        vArray elements = vUIElementGetAllRegisteredElements();
        u64 start_ms = vTimerGetMs(render_timer);
        vRendererBeginFrame(v.renderer);
       
        vRendererExecuteCommands(
            v.renderer,
            vRenderAPIGetCommandBuffer(next_rcmd_buf)
        );

        next_rcmd_buf = (next_rcmd_buf + 1) % 2;

        vRendererDrawUI(
            v.renderer,
            elements
        );

        vRendererEndFrame(v.renderer);

        u64 end_ms = vTimerGetMs(render_timer);

        u64 elapsed_ms = end_ms - start_ms;
        if(elapsed_ms < 1'000 / v.target_fps) {
            vThreadSleep(1'000 / v.target_fps - elapsed_ms);
        }
    }

    return NULL;
}

static void vDispatchEvents() {
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

    return NULL;
}

static void vOnWindowClosed(const vEvent *ev) {
    atomic_store(&v.is_running, false);
}

void vInit(vString title, u32 w, u32 h) {
    setlocale(LC_ALL, "");

    vEventQueueInit();
    
    vUIElementRegistryInit();

    vEventQueueSubscribe(V_EVENT_WINDOW_CLOSED, vOnWindowClosed);
    
    vEventQueueSubscribe(V_EVENT_MOUSE_LBUTTON, vUIEventDispatch);
    vEventQueueSubscribe(V_EVENT_MOUSE_RBUTTON, vUIEventDispatch);
    vEventQueueSubscribe(V_EVENT_MOUSE_MBUTTON, vUIEventDispatch);
    vEventQueueSubscribe(V_EVENT_MOUSE_MOVED, vUIEventDispatch);
    vEventQueueSubscribe(V_EVENT_KEY_PRESSED, vUIEventDispatch);
    vEventQueueSubscribe(V_EVENT_KEY_RELEASED, vUIEventDispatch);

    texture_data1 = vTextureLoad("assets/textures/test.png");
    texture_data2 = vTextureLoad("assets/textures/test2.png");

    button = vUIElementCreate(
        100, 100, 100, 50,
        true,
        NULL,
        NULL,
        NULL,
        NULL
    );
    
    vUIElementRegister(button);

    v.window = vWindowCreate(title, w, h);
    if (!v.window) {
        vLog("[ENGINE] Window creation failed\n");
        return;
    }

    v.renderer_thread = vThreadCreate(vRendererThread, NULL);
    v.main_thread = vThreadCreate(vMainThread, NULL);

    v.init_barrier = vThreadSyncBarrierCreate();
    vThreadSyncBarrierAdd(v.init_barrier, v.renderer_thread);
    vThreadSyncBarrierAdd(v.init_barrier, v.main_thread);
    vThreadSyncBarrierStart(v.init_barrier);
}

void vRun(void (*application_init)(void), void (*application_run)(void)) {
    atomic_store(&v.is_running, true);

    v.application_run = application_run;
    v.application_init = application_init;

    vThreadStart(v.renderer_thread);
    vThreadStart(v.main_thread);

    while(atomic_load(&v.is_running)) {
        vWindowPollEvents();
    }

    vThreadJoin(v.renderer_thread);
    vThreadJoin(v.main_thread);

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
    vEventQueueSubscribe(type, callback);
}

void vRemoveEventListener(vEventType type, void (*callback)(const vEvent*)) {
    vEventQueueUnsubscribe(type, callback);
}