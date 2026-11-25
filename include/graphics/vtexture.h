#pragma once

#include "core/vtypes.h"

typedef struct vTextureData* vTextureData;
typedef struct vTexture *vTexture;

typedef enum vTextureFilter {
    V_TEXTURE_FILTER_NEAREST,
    V_TEXTURE_FILTER_LINEAR
} vTextureFilter;

typedef enum vTextureWrap {
    V_TEXTURE_WRAP_REPEAT,
    V_TEXTURE_WRAP_CLAMP
} vTextureWrap;

typedef struct vTextureSettings {
    vTextureFilter filter;
    vTextureWrap wrap;
} vTextureSettings;

vTextureData vTextureLoad(const char* path);

u32 vTextureGetID(vTexture texture);
u32 vTextureGetWidth(vTexture texture);
u32 vTextureGetHeight(vTexture texture);