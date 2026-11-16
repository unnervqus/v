#include "core/vevent_queue.h"

#include <stdatomic.h>
#include <stdbool.h>

#define V_EVENT_QUEUE_CAPACITY 1024
#define V_EVENT_MAX_HANDLERS 16

typedef struct vEventQueue {
    vEvent events[V_EVENT_QUEUE_CAPACITY];

    u32 head;
    u32 tail;
    u32 count;

    vEventHandle handlers[V_EVENT_TYPE_COUNT][V_EVENT_MAX_HANDLERS];
    u32 handler_count[V_EVENT_TYPE_COUNT];

    atomic_flag lock;
} vEventQueue;

static vEventQueue queue = {
    .lock = ATOMIC_FLAG_INIT
};

static void vEventQueueLock() {
    while (atomic_flag_test_and_set_explicit(
        &queue.lock,
        memory_order_acquire
    )) {}
}

static void vEventQueueUnlock() {
    atomic_flag_clear_explicit(
        &queue.lock,
        memory_order_release
    );
}

void vEventQueuePublish(const vEvent ev) {
    vEventQueueLock();

    if (queue.count >= V_EVENT_QUEUE_CAPACITY) {
        vEventQueueUnlock();
        return;
    }

    queue.events[queue.tail] = ev;

    queue.tail = (queue.tail + 1) % V_EVENT_QUEUE_CAPACITY;
    queue.count++;

    vEventQueueUnlock();
}

void vEventQueueSubscribe(vEventType event_type, vEventHandle handler) {
    if (
        event_type < 0 ||
        event_type >= V_EVENT_TYPE_COUNT ||
        !handler
    ) {
        return;
    }

    vEventQueueLock();

    u32 count = queue.handler_count[event_type];

    if (count < V_EVENT_MAX_HANDLERS) {
        queue.handlers[event_type][count] = handler;
        queue.handler_count[event_type]++;
    }

    vEventQueueUnlock();
}

void vEventQueueDispatch() {
    vEventQueueLock();
    u32 events_to_dispatch = queue.count;
    vEventQueueUnlock();

    for (u32 i = 0; i < events_to_dispatch; i++) {
        vEvent ev;

        vEventHandle handlers[V_EVENT_MAX_HANDLERS];
        u32 handler_count = 0;

        vEventQueueLock();

        if (queue.count == 0) {
            vEventQueueUnlock();
            break;
        }

        ev = queue.events[queue.head];

        queue.head = (queue.head + 1) % V_EVENT_QUEUE_CAPACITY;
        queue.count--;

        if (
            ev.type >= 0 &&
            ev.type < V_EVENT_TYPE_COUNT
        ) {
            handler_count = queue.handler_count[ev.type];

            for (u32 j = 0; j < handler_count; j++) {
                handlers[j] = queue.handlers[ev.type][j];
            }
        }

        vEventQueueUnlock();

        for (u32 j = 0; j < handler_count; j++) {
            handlers[j](&ev);
        }
    }
}