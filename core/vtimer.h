#pragma once

#include "core/vtypes.h"

typedef struct vTimer* vTimer;

vTimer vTimerCreate(bool start);
void vTimerStart(vTimer);
u64 vTimerLoopms(vTimer);
u64 vTimerGetMs(vTimer);