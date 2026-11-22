#pragma once

#include "core/vmemory.h"

#include <pthread.h>

typedef struct vMutex {
    pthread_mutex_t mutex;
} vMutex;

static inline void vMutexCreate(vMutex* mtx) {
    pthread_mutex_init(&mtx->mutex, NULL);
}

static inline void vMutexLock(vMutex* mtx) {
    pthread_mutex_lock(&mtx->mutex);
}

static inline void vMutexUnlock(vMutex* mtx) {
    pthread_mutex_unlock(&mtx->mutex);
}