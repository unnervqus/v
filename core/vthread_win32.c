#include "core/vthread.h"
#include "core/vmemory.h"

#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#define UNICODE
#include <windows.h>

struct vThread {
    HANDLE handle;
    DWORD id;

    vThreadFn fn;
    void* arg;

    bool started;
    bool joined;
};

static DWORD WINAPI vThreadEntry(LPVOID param) {
    vThread t = (vThread)param;

    t->fn(t->arg);

    return 0;
}

vThread vThreadCreate(vThreadFn thread_fn, void* arg) {
    if (!thread_fn) return NULL;

    vThread thread = vMalloc(sizeof(*thread));

    thread->fn = thread_fn;
    thread->arg = arg;
    thread->joined = false;
    thread->started = false;
    thread->handle = NULL;

    return thread;
}

void vThreadStart(vThread thread) {
    if(!thread || thread->handle || thread->started) return;

    thread->handle = CreateThread(
        NULL, 0,
        vThreadEntry,
        thread, 0,
        &thread->id
    );

    if (!thread->handle) {
        vFree(thread);
        return;
    }

    thread->started = true;
}

void vThreadDestroy(vThread thread) {
    if(!thread) return;

    vThreadJoin(thread);

    CloseHandle(thread->handle);
    vFree(thread);
}


void vThreadJoin(vThread thread) {
    if (!thread || !thread->started || thread->joined) return;

    DWORD result = WaitForSingleObject(thread->handle, INFINITE);

    if (result == WAIT_OBJECT_0) thread->joined = true;
}

bool vThreadIsFinished(vThread thread) {
    if (!thread || !thread->started)
        return false;

    return WaitForSingleObject(thread->handle, 0) == WAIT_OBJECT_0;
}

void vThreadSleep(u64 ms) {
    Sleep(ms);
}