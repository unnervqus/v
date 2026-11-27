#pragma once

#include "graphics/vtexture.h"

vTexture vTextureCreate(const vTextureData texture_data);
vTexture vTextureCreateEx(
    const vTextureData texture_data,
    vTextureSettings settings
);

void vTextureDestroy(vTexture texture);

void vTextureBind(vTexture texture, u32 slot);
void vTextureUnbind(void);