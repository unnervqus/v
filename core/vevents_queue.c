#include "core/vevent_queue.h"
#include "core/vring_buffer.h"
#include "core/threading/vmutex.h"

#define V_EVENT_QUEUE_CAPACITY 1024
#define V_EVENT_MAX_HANDLERS 16

typedef struct vEventQueue {
    vRingBuffer events;

    vEventHandle handlers[V_EVENT_TYPE_COUNT][V_EVENT_MAX_HANDLERS];
    u32 handler_count[V_EVENT_TYPE_COUNT];

    vMutex mutex;
} vEventQueue;

static vEventQueue queue;

void vEventQueueInit() {
    queue.events = vRingBufferCreate(sizeof(vEvent), V_EVENT_QUEUE_CAPACITY);

    for (u32 i = 0; i < V_EVENT_TYPE_COUNT; i++) {
        for (u32 j = 0; j < V_EVENT_MAX_HANDLERS; j++) {
            queue.handlers[i][j] = NULL;
        }
    }

    vMutexCreate(&queue.mutex);
}

void vEventQueuePublish(const vEvent ev) {
    vMutexLock(&queue.mutex);
    vRingBufferPush(queue.events, &ev);
    vMutexUnlock(&queue.mutex);
}

void vEventQueueSubscribe(vEventType event_type, vEventHandle handler) {
    if (
        event_type < 0 ||
        event_type >= V_EVENT_TYPE_COUNT ||
        !handler
    ) {
        return;
    }

    vMutexLock(&queue.mutex);

    u32 count = queue.handler_count[event_type];

    // Make sure the handler wasn't subscribed before
    for (u32 j = 0; j < V_EVENT_MAX_HANDLERS; j++) {
        if(queue.handlers[event_type][j] == handler) {
            // Handler is already subscribed
            vMutexUnlock(&queue.mutex);
            return;
        }
    }

    if (count < V_EVENT_MAX_HANDLERS) {
        for (u32 i = 0; i < V_EVENT_MAX_HANDLERS; i++) {
            // Find first free
            if(queue.handlers[event_type][i] == NULL) {
                queue.handlers[event_type][count] = handler;
                queue.handler_count[event_type]++;
                break;
            }
        }
    }

    vMutexUnlock(&queue.mutex);
}

void vEventQueueDispatch() {
    vMutexLock(&queue.mutex);
    u32 events_to_dispatch = vRingBufferGetCount(queue.events);
    vMutexUnlock(&queue.mutex);

    for (u32 i = 0; i < events_to_dispatch; i++) {
        vEvent *ev, _ev;
        vEventHandle handlers[V_EVENT_MAX_HANDLERS];
        u32 handler_count = 0;

        vMutexLock(&queue.mutex);

        ev = (vEvent*)vRingBufferPop(queue.events);
        _ev = *ev;
        if (
            _ev.type >= 0 &&
            _ev.type < V_EVENT_TYPE_COUNT
        ) {
            handler_count = queue.handler_count[_ev.type];
            for (u32 j = 0; j < handler_count; j++) {
                handlers[j] = queue.handlers[_ev.type][j];
            }
        }

        vMutexUnlock(&queue.mutex);

        for (u32 j = 0; j < handler_count; j++) {
            handlers[j](&_ev);
        }
    }
}

void vEventQueueUnsubscribe(vEventType event_type, vEventHandle handler) {
    for (u32 i = 0; i < V_EVENT_MAX_HANDLERS; i++) {
        if(queue.handlers[event_type][i] == handler) {
            queue.handlers[event_type][i] = NULL;
            queue.handler_count[event_type]--;
        }
    }
}