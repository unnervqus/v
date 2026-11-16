#pragma once

#include "platform/vwindow.h"

typedef struct vRenderer* vRenderer;

vRenderer vRendererCreate(vWindow window);

void vRendererBeginFrame(vRenderer);
void vRendererEndFrame(vRenderer);