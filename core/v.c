#include "platform/vwindow.h"
#include "core/v.h"
#include "graphics/vrenderer.h"
#include "core/vthread.h"
#include "core/vlogging.h"
#include "core/vtimer.h"
#include "core/vevent_queue.h"

typedef struct V {
    vWindow window;
    vRenderer renderer;

    vThread renderer_thread;
    vThread main_thread;
    bool is_running;

    u64 target_fps;
} V;

V v = {
    .target_fps = 60
};

static void* vRendererThread(void* arg) {
    v.renderer = vRendererCreate(v.window);
    vTimer timer = vTimerCreate(true);
    
    vTexture texture = vTextureCreate(
        "assets/textures/test.png"
    );

    while(v.is_running) {
        u64 start_ms = vTimerGetMs(timer);
        vRendererBeginFrame(v.renderer);

        vDrawTexture(
            v.renderer,
            texture,
            (vVec2){ 100.0f, 100.0f },
            (vVec2){ 256.0f, 256.0f }
        );
        
        vRendererEndFrame(v.renderer);
        
        u64 end_ms = vTimerGetMs(timer);

        u64 elapsed_ms = end_ms - start_ms;
        
#ifdef DEBUG
            vLog(
                "[RENDERER THREAD] Frame rendered in %llu ms\r", 
                elapsed_ms
            );
        }
#endif

        if(elapsed_ms < 1'000 / v.target_fps) {
            vThreadSleep(1'000 / v.target_fps - elapsed_ms);
        }
    }

    vLog("[RENDERER THREAD] Thread finished\n"); 
    
    return NULL;
}

static void vDispatchEvents() {
    vEventQueueDispatch();
    
}

static void vUpdate() {

}

static void* vMainThread(void* arg) {

    while(v.is_running) {
        vDispatchEvents();
        vUpdate();
    }

    vThreadJoin(v.renderer_thread);

    vLog("[MAIN THREAD] Thread finished\n");
    return NULL;
}

static void vOnWindowClosed(const vEvent *ev) {
    v.is_running = false;
}

void vInit(const char* title, u32 w, u32 h) {
    v.window = vWindowCreate(title, w, h);

    vEventQueueSubscribe(V_EVENT_WINDOW_CLOSED, vOnWindowClosed);

    v.renderer_thread = vThreadCreate(vRendererThread, NULL);

    v.main_thread = vThreadCreate(vMainThread, NULL);
}

void vRun() {
    v.is_running = true;

    vThreadStart(v.renderer_thread);
    vThreadStart(v.main_thread);

    while(v.is_running) {
        vWindowPollEvents(v.window);
    }

    vThreadJoin(v.main_thread);

    vLog("[WINDOW THREAD] Thread finished\n");
}

void vShutdown() {

}
