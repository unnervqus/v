#pragma once

#include "core/vtypes.h"

#include <stdlib.h>
#include <memory.h>

static inline void* vMalloc(u64 size) {
    return malloc(size);
}

static inline void vFree(void* ptr) {
    free(ptr);
}

static inline void* vRealloc(void* ptr, u64 new_size) {
    return realloc(ptr, new_size);
}

static inline void vMemoryCopy(void* dest, const void* src, u64 size) {
    memcpy(dest, src, size);
}

static inline void vMemoryZero(void* dst, u32 length) {
    memset(dst, 0, length);
}