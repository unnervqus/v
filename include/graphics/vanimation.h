#pragma once

#include "core/vtypes.h"
#include "math/vmath.h"
#include "graphics/vtexture.h"

typedef struct vAnimation* vAnimation;

vAnimation vAnimationCreate(
    const char* atlas_path, 
    u32 frame_w, u32 frame_h, 
    u64 fps, u32 frame_count,
    u32 rows, u32 columns
);

void vAnimationPlay(vAnimation);

vVec2 vAnimationNextFrame(vAnimation a, u64 time_passed_ms);

void vAnimationDestroy(vAnimation);

vTexture vAnimationGetTexture(vAnimation);
vVec2 vAnimationGetFrameSize(vAnimation);