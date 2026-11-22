#include "core/vevent_queue.h"
#include "core/vring_buffer.h"
#include "core/vlogging.h"
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
    vMutexCreate(&queue.mutex);
    vLog("[EVENT SYSTEM] Initialized (capacity=%u)\n", V_EVENT_QUEUE_CAPACITY);
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

    if (count < V_EVENT_MAX_HANDLERS) {
        queue.handlers[event_type][count] = handler;
        queue.handler_count[event_type]++;
    }

    vLog("[EVENT SYSTEM] Subscribed %p on event %d\n", handler, event_type);

    vMutexUnlock(&queue.mutex);
}

void vEventQueueDispatch() {
    vMutexLock(&queue.mutex);
    u32 events_to_dispatch = vRingBufferGetCount(queue.events);
    vMutexUnlock(&queue.mutex);

    if (events_to_dispatch > 0) {
        vLog("[EVENT SYSTEM] Dispatching batch of %u event(s)\n", events_to_dispatch);
    }

    for (u32 i = 0; i < events_to_dispatch; i++) {
        vEvent *ev, _ev;
        vEventHandle handlers[V_EVENT_MAX_HANDLERS];
        u32 handler_count = 0;

        vMutexLock(&queue.mutex);

        ev = (vEvent*)vRingBufferPop(queue.events);
        _ev = *ev;
        vLog("[EVENT DISPATCH] Got new event. Type: %d. Address: %p\n", ev->type, ev);
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