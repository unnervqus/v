#pragma once

#include "core/vtypes.h"

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

vTexture vTextureCreate(const char* path);
vTexture vTextureCreateEx(
    const char* path,
    vTextureSettings settings
);

void vTextureDestroy(vTexture texture);

void vTextureBind(vTexture texture, u32 slot);
void vTextureUnbind(void);

u32 vTextureGetID(vTexture texture);
u32 vTextureGetWidth(vTexture texture);
u32 vTextureGetHeight(vTexture texture);