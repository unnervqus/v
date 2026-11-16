#pragma once

#include "platform/vwindow.h"
#include "graphics/vtexture.h"
#include "math/vmath.h"

typedef struct vRenderer* vRenderer;

vRenderer vRendererCreate(vWindow window);

void vRendererBeginFrame(vRenderer);
void vRendererEndFrame(vRenderer);

void vDrawTexture(
    vRenderer renderer, vTexture texture, 
    vVec2 position, vVec2 size
);

void vRendererResize(
    vRenderer renderer,
    u32 width, u32 height
);