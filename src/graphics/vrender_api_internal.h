#pragma once

#include "graphics/vrender_api.h"

typedef struct vRenderer* vRenderer;
typedef struct vRenderCommandBuffer vRenderCommandBuffer;

void vRenderAPIInit(vRenderer renderer);
vRenderCommandBuffer* vRenderAPIGetCommandBuffer(u32 index);