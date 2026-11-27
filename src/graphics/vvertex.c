#include "graphics/vvertex.h"

vVertex2D vVertex2DCreate(vVec2 position, vVec2 tex_coord, vVec4 color) {
    return (vVertex2D){
        position,
        tex_coord,
        color
    };
}
vQuad2D vQuad2DCreate(
    f32 start_x, f32 start_y, 
    f32 width, f32 height, 
    vVec2 uv0, vVec2 uv1,
    vVec4 colors[4]) {
        return (vQuad2D) { 
        .v0 = vVertex2DCreate( 
            (vVec2){ start_x, start_y },
            (vVec2){ uv0.x, uv0.y },
            colors[0]),
        .v1 = vVertex2DCreate(
            (vVec2){ start_x + width, start_y },
            (vVec2){ uv1.x, uv0.y },
            colors[1]),
        .v2 = vVertex2DCreate(
            (vVec2){ start_x + width, start_y + height },
            (vVec2){ uv1.x, uv1.y },
            colors[2]),
        .v3 = vVertex2DCreate(
            (vVec2){ start_x, start_y + height },
            (vVec2){ uv0.x, uv1.y },
            colors[3])
    };
}