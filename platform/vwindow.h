#pragma once

#include "core/vtypes.h"

typedef struct vWindow* vWindow;

vWindow vWindowCreate(const char* title, u32 w, u32 h);
void vWindowDestroy(vWindow w);
void* vWindowGetNative(vWindow);
void vWindowPollEvents(vWindow);

u32 vWindowGetWidth(vWindow w);
u32 vWindowGetHeight(vWindow w);