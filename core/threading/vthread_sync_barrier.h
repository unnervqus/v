#pragma once

#include "core/threading/vthread.h"

typedef struct vThreadSyncBarrier* vThreadSyncBarrier;

vThreadSyncBarrier vThreadSyncBarrierCreate();

void vThreadSyncBarrierAdd(vThreadSyncBarrier tsb, vThread thread);
void vThreadSyncBarrierRemove(vThreadSyncBarrier tsb, vThread thread);

void vThreadSyncBarrierStart(vThreadSyncBarrier tsb);

void vThreadSyncBarrierConfirm(vThreadSyncBarrier tsb, vThread thread);

void vThreadSyncBarrierDestroy(vThreadSyncBarrier tsb);