#include "core/vring_buffer.h"

#include "core/varray.h"
#include "core/vmemory.h"
#include "core/vlogging.h"
#include "vtypes.h"

struct vRingBuffer {
    u32 head;
    u32 tail;
    u32 capacity;
    u32 count;
    vArray items;
};

vRingBuffer vRingBufferCreate(u32 element_size, u32 capacity) {
    vRingBuffer ring_buf = vMalloc(sizeof(struct vRingBuffer));
    ring_buf->items = vArrayCreate(element_size, capacity);
    // Fill array with zero value so we can use insert at index
    vbytes filler = vMalloc(element_size);
    vMemoryZero(filler, element_size);
    for(u32 i = 0; i < capacity; ++i) {
        vArrayPush(ring_buf->items, filler);
    }
    ring_buf->capacity = capacity;
    ring_buf->count = 0;
    ring_buf->head = 0;
    ring_buf->tail = 0;
    return ring_buf;
}

void vRingBufferPush(vRingBuffer rb, const void* item) {
    if(!rb) return;

    if(rb->count < rb->capacity) {
        vArraySet(rb->items, rb->head, item);
        rb->count++;
        rb->head = (rb->head + 1) % rb->capacity;
        // vLog(
        //     "[RING BUFFER] Pushed item in ring buffer (%p), count: %llu, head: %llu\n",
        //     rb, rb->count, rb->head); 
    }
}

void* vRingBufferPop(vRingBuffer rb) {
    if(!rb || rb->count == 0) {
        return NULL;
    }
    void* item = vArrayGet(rb->items, rb->tail);
    rb->tail = (rb->tail + 1) % rb->capacity;
    rb->count--;
    // vLog(
    //     "[RING BUFFER] Extracted item from ring buffer (%p), count: %llu, tail: %llu\n",
    //     rb, rb->count, rb->tail);        
    return item;
}

inline u32 vRingBufferGetCapacity(vRingBuffer rb) {
    return rb->capacity;
}

inline u32 vRingBufferGetCount(vRingBuffer rb) {
    return rb->count;
}