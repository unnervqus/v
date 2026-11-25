#include "core/threading/vthread_sync_barrier.h"
#include "core/varray.h"
#include "core/threading/vmutex.h"
#include "core/vmemory.h"
#include "core/vlogging.h"

#include <pthread.h>

struct vThreadSyncBarrier {
    vArray threads;
    u32 thread_count;
    u32 thread_confirms;
    bool is_started;
    vMutex mtx;
    pthread_cond_t cond;
};

vThreadSyncBarrier vThreadSyncBarrierCreate() {
    vThreadSyncBarrier tsb = vMalloc(sizeof(struct vThreadSyncBarrier));
    tsb->threads = vArrayCreate(sizeof(vThread), 10);
    tsb->thread_confirms = 0;
    tsb->thread_count = 0;
    vMutexCreate(&tsb->mtx);
    pthread_cond_init(&tsb->cond, NULL); // Remove via vCondVarCreate
    return tsb;
}

void vThreadSyncBarrierAdd(vThreadSyncBarrier tsb, vThread thread) {
    if(!tsb->is_started) {
        tsb->thread_count++;
        vArrayPush(tsb->threads, thread);
    }
}

bool tsb_removeif(const void* a, const void* b) {
    return a == b;
}

void vThreadSyncBarrierRemove(vThreadSyncBarrier tsb, vThread thread) {
    if(tsb->thread_count > 0 && !tsb->is_started) {
        u32 i =vArrayIndexOf(tsb->threads, thread, tsb_removeif);
        vArrayRemove(tsb->threads, i);
    }
}

void vThreadSyncBarrierStart(vThreadSyncBarrier tsb) {
    if(!tsb->is_started) {
        tsb->is_started = true;
    }
}

void vThreadSyncBarrierConfirm(vThreadSyncBarrier tsb, vThread thread) {
    vMutexLock(&tsb->mtx);

    tsb->thread_confirms++;
    
    // Save local copy of confimation count for each thread
    u32 thread_confirms = tsb->thread_confirms;

    // All threads finished, they can be free now
    if(tsb->thread_confirms == tsb->thread_count) {
        vLog("[THREAD SYNC BARRIER] All threads synced\n", thread);
        pthread_cond_broadcast(&tsb->cond);
    } else {
        while(thread_confirms != tsb->thread_confirms) {
            vLog("[THREAD SYNC BARRIER] Thread %p confirmed\n", thread);
            pthread_cond_wait(&tsb->cond, &tsb->mtx.mutex);
        }
    }

    vMutexUnlock(&tsb->mtx);
}

void vThreadSyncBarrierDestroy(vThreadSyncBarrier tsb) {
    vArrayDestroy(tsb->threads);
    vFree(tsb);
}