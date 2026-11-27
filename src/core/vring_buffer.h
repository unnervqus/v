#pragma once

#include "core/vtypes.h"

typedef struct vRingBuffer* vRingBuffer;

vRingBuffer vRingBufferCreate(u32 element_size, u32 capacity);

void vRingBufferPush(vRingBuffer, const void* item);

void* vRingBufferPop(vRingBuffer);

u32 vRingBufferGetCapacity(vRingBuffer);
u32 vRingBufferGetCount(vRingBuffer);

void vRingBufferDestroy(vRingBuffer);