#pragma once

#include "core/vtypes.h"

typedef struct vSemaphore* vSemaphore;

vSemaphore vSemaphoreCreate(u32 signal_count);
void vSemaphoreDestroy(vSemaphore);

void vSemaphoreWait(vSemaphore);
void vSemaphorePost(vSemaphore);