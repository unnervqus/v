#include "core/vtimer.h"
#include "core/vmemory.h"
#include "core/vlogging.h"

#define UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct vTimer {
    LARGE_INTEGER start;
    LARGE_INTEGER freq;

    bool started;
};

vTimer vTimerCreate(bool start) {
    vTimer timer = vMalloc(sizeof(struct vTimer));
    if (!timer) {
        vLog("[TIMER] Create failed: allocation failed\n");
        return NULL;
    }

    timer->started = false;
    QueryPerformanceFrequency(&timer->freq);
    if(start) {
        vTimerStart(timer);
    }

    vLog("[TIMER] Created %p (started=%s)\n", timer, start ? "true" : "false");
    return timer;
}

void vTimerStart(vTimer timer) {
    if (!timer) return;

    if(!timer->started) {
        QueryPerformanceCounter(&timer->start);
        timer->started = true;
        vLog("[TIMER] Started %p\n", timer);
    }
}

u64 vTimerLoopms(vTimer timer) {
    LARGE_INTEGER end;
    QueryPerformanceCounter(&end);
    u64 freq = timer->freq.QuadPart;

    u64 ticks = end.QuadPart - timer->start.QuadPart;
    u64 elapsed_ms = 
        (ticks / freq) * 1000ULL + 
        ((ticks % freq) * 1000ULL) / freq;
    
    timer->start = end;

    return elapsed_ms;
}

u64 vTimerGetMs(vTimer timer) {
    LARGE_INTEGER end;
    QueryPerformanceCounter(&end);
    u64 freq = timer->freq.QuadPart;

    u64 ticks = end.QuadPart - timer->start.QuadPart;
    u64 elapsed_ms = 
        (ticks / freq) * 1000ULL + 
        ((ticks % freq) * 1000ULL) / freq;

    return elapsed_ms;
}