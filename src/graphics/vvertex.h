#pragma once

#include "math/vmath.h"
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
typedef struct vVertex2D {
    vVec2 position;
    vVec2 tex_coord;
    vVec4 color;
} vVertex2D;

typedef struct vQuad2D {
    vVertex2D v0, v1, v2;
    vVertex2D v3, v4, v5;
} vQuad2D;

vVertex2D vVertex2DCreate(vVec2 position, vVec2 tex_coord, vVec4 color);

vQuad2D vQuad2DCreate(
    f32 start_x, f32 start_y, 
    f32 width, f32 height, 
    vVec2 uv0, vVec2 uv1,
    vVec4 colors[4]
);