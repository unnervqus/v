#pragma once

#include "core/vtypes.h"

typedef struct vVec2 {
    f32 x, y;
} vVec2;

typedef struct vVec3 {
    f32 x, y, z;
} vVec3;

typedef struct vVec4 {
    f32 x, y, z, w;
} vVec4;

typedef struct vRectF32 {
    f32 z, y, w, h;
} vRectF32;

vVec2 vVec2Multiply(vVec2 a, vVec2 b);