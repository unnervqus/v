#pragma once

#include "core/vtypes.h"
#include "core/vstring.h"

typedef struct vWindow* vWindow;

vWindow vWindowCreate(vString title, u32 w, u32 h);
void vWindowDestroy(vWindow w);
void* vWindowGetNative(vWindow);
void vWindowPollEvents();

u32 vWindowGetWidth(vWindow w);
u32 vWindowGetHeight(vWindow w);