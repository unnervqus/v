#pragma once

#include "core/vstring.h"

typedef struct vFont* vFont;
typedef struct vFontData* vFontData;
typedef struct vGlyph {
    f32 u0, v0;
    f32 u1, v1;

    i32 w, h;

    f32 bearingX, bearingY;
    i32 advance;
    u8* bitmap;
} vGlyph;

vFontData vFontLoad(const char* path, f32 size);

