#pragma once

#include "core/vtypes.h"
#include <wchar.h>

typedef struct vString* vString;

vString vStringCreate(const char* data, u32 capacity);
void vStringDestroy(vString);

const char* vStringGetData(vString);
u32 vStringGetDataW(vString string, wchar_t* out, u32 wcapacity);
u32 vStringGetCapacity(vString);
u32 vStringGetSize(vString);
vString vStringCopy(vString);