#pragma once

#include "math/vmath.h"
#include "graphics/vtexture.h"
#include "graphics/vgraphics_types.h"

typedef struct vRenderAPI vRenderAPI;
typedef struct vRenderer* vRenderer;
typedef struct vRenderData vRenderData;
typedef struct vRenderCommandBuffer vRenderCommandBuffer;

typedef struct vRenderAttributes {
    struct {
        f32 rotation_x;
        f32 rotation_y;
        f32 rotation_z;

        f32 pivot_x;
        f32 pivot_y;
        f32 pivot_z;
    } transformation;

    struct {
        vColor tint;
        bool flip_x;
        bool flip_y;
        bool flip_z;
    } image;

    struct {
        f32 opacity;
        f32 brightness;
        f32 saturation;
        f32 contrast;
    } color;

    struct {
        f32 emission;
        vColor emission_color;
    } light;


    struct {
        vVec2 uv_coords;
        vVec2 screen_coords;
        vVec2 texture_size;
        vVec2 size;
        vVec2 scale;

        // f32 metallic;
        // f32 roughness;
        // f32 specular;
    } texture;

    bool wireframe_mode;
    // f32 animation_speed;
    // f32 animation_time;

    // f32 dissolve;
    // f32 dissolve_edge_width;

    // bool animated;
    // bool use_vertex_animation;
} vRenderAttributes;

struct vRenderAPI {
    void           (*clear) (vColor color);
    struct {
        void            (*text) (vText *text, vVec2 pos, vRenderAttributes *attribs);
        void           (*point) (vPoint* point, vVec2 pos, vRenderAttributes *attribs);
        void       (*primitive) (vPrimitive* figure, vVec2 pos, vRenderAttributes *attribs);
        void         (*texture) (vTexture texture, vVec2 pos, vRenderAttributes *attribs);
    } draw;



    struct {
        void     (*texture) (vTexture *texture_out, vTextureData texture_data);
    } create;

    struct {
        void       (*ayer) (u32 layer_id);
        void     (*scissors) (vRectF32 region);
        void    (*viewport) (vRectF32 viewport);
        void  (*blend_mode) ();
    } set;
    
    void         (*execute) ();

    vRenderData* data;
    bool is_ready;
};

extern vRenderAPI v_render;

void vRenderAPIInit(vRenderer renderer);

vRenderCommandBuffer* vRenderAPIGetCommandBuffer(u32 index);