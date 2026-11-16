#pragma once

#include "core/vtypes.h"

typedef struct vThread* vThread;
typedef void* (*vThreadFn)(void*);

vThread vThreadCreate(vThreadFn thread_fn, void* arg);
void vThreadDestroy(vThread);
void vThreadStart(vThread);
void vThreadJoin(vThread);
bool vThreadIsFinished(vThread);
void vThreadSleep(u64 ms);