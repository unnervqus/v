#pragma once

#include "core/vtypes.h"

#include <stdlib.h>

static inline void* vMalloc(u64 size) {
    return malloc(size);
}

static inline void vFree(void* ptr) {
    free(ptr);
}