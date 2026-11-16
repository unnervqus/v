#pragma once

#include "math/vmath.h"

typedef struct vVertex2D {
    vVec2 position;
    vVec2 tex_coord;
    vVec4 color;
} vVertex2D;

/*
Example vertex structure for 2D rendering
vVertex2D vertices[4] = {
       position           tex_coord                color
    { { -0.5f, -0.5f }, { 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
    { {  0.5f, -0.5f }, { 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
    { {  0.5f,  0.5f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
    { { -0.5f,  0.5f }, { 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } }
};

*/
vVertex2D vVertex2DCreate(vVec2 position, vVec2 tex_coord, vVec4 color);
