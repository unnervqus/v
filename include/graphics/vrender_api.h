#pragma once

#include "core/vstring.h"

#include "graphics/vtexture.h"
#include "graphics/vfont.h"
#include "graphics/vcolor.h"

#include "math/vmath.h"
// #include "graphics/vgraphics_types.h"

typedef struct vRenderData vRenderData;
typedef struct vRenderAttributes vRenderAttributes;

struct vRenderAttributes {
    struct {
        vVec2 texture_coords;
        vVec2 screen_coords;
        vVec2 texture_size;
        vVec2 dst_size;
        vVec2 scale;
    } texture;

    bool active;

};

typedef struct vRenderAPI {
    void (*clear)           (vColor color);
    struct {
        void (*text)        (vFont font, vString text, vVec2 pos, vRenderAttributes attribs); 
        void (*font)        (vFont font, vVec2 pos); 
        void (*texture)     (vTexture texture, vVec2 pos, vRenderAttributes attribs);
    } draw;

    struct {
        void (*font)        (vFont* font_out, vFontData font_data);
        void (*texture)     (vTexture* texture_out, vTextureData texture_data);
    } create;

    struct {
        void (*viewport)    (vRectF32 viewport);
    } set;
    
    void (*execute) ();

    vRenderData* data;
    bool is_ready;
} vRenderAPI;

extern vRenderAPI v_render;