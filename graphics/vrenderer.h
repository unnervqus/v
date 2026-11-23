#pragma once

#include "core/varray.h"
#include "graphics/vrender_cmd_buffer.h"
#include "platform/vwindow.h"
#include "graphics/vtexture.h"
#include "graphics/vgraphics_types.h"
#include "math/vmath.h"

typedef struct vRenderer* vRenderer;

vRenderer vRendererCreate(vWindow window);

void vRendererBeginFrame(vRenderer);
void vRendererExecuteCommands(vRenderer renderer, vRenderCommandBuffer* rcmd_buf);
void vRendererEndFrame(vRenderer);

void vRendererClearScreen(vRenderer, vColor);

void vRendererDrawTexture(
    vRenderer renderer, vTexture texture, 
    vVec2 position, vVec2 size
);

void vRendererDrawTextureRegion(
    vRenderer renderer, vTexture texture, 
    vVec2 position_src, vVec2 size_src, 
    vVec2 position_dst, vVec2 size_dst
);

void vRendererResize(
    vRenderer renderer,
    u32 width, u32 height
);

void vRendererDrawUI(
    vRenderer renderer,
    vArray elements
);