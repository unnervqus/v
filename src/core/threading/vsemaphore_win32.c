#include "core/threading/vsemaphore.h"
#include "core/vmemory.h"

#include <semaphore.h>

struct vSemaphore {
    sem_t semaphore;
};

vSemaphore vSemaphoreCreate(u32 signal_count) {
    vSemaphore s = vMalloc(sizeof(struct vSemaphore));
    sem_init(&s->semaphore, 0, signal_count);
    return s;
}
void vSemaphoreDestroy(vSemaphore s) {
    sem_close(&s->semaphore);
    vFree(s);
}

void vSemaphoreWait(vSemaphore s) {
    sem_wait(&s->semaphore);
}

void vSemaphorePost(vSemaphore s) {
    sem_post(&s->semaphore);
}
