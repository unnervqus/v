#pragma once

#include "core/vtypes.h"

typedef struct vText* vText;
typedef struct vTexture* vTexture;
typedef struct vSprite *vSprite;
typedef struct vScene *vScene;

typedef struct vColor {
    f32 r, g, b, a;
} vColor;

typedef struct vPoint {
    f32 x, y, z, w;
    vColor color;
} vPoint;

typedef enum vPrimitiveType {
    V_PRIMITIVE_CIRLE,
    V_PRIMITIVE_RECT,
    V_PRIMITIVE_TRIANGLE,
} vPrimitiveType;

typedef struct vPrimitive {
    vPrimitiveType type;
    vColor color;
    vPoint pos;
} vPrimitive;