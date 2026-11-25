#include "graphics/vanimation.h"
#include "core/vmemory.h"
#include "graphics/vtexture_internal.h"

struct vAnimation {
    vTexture atlas;

    u32 current_frame;
    u32 frame_count;
    u32 frame_w, frame_h;
    u32 columns;
    u32 rows;
    u64 fps;
    u64 frame_duration;
};

vAnimation vAnimationCreate(
    const char* atlas_path, 
    u32 frame_w, u32 frame_h, 
    u64 fps, u32 frame_count,
    u32 rows, u32 columns
) {
    vAnimation a = vMalloc(sizeof(struct vAnimation));
    
    vTextureData tex_data = vTextureLoad(atlas_path);
    a->atlas = vTextureCreate(tex_data);

    a->frame_h = frame_h;
    a->frame_w = frame_w;
    a->fps = fps;
    a->frame_count = frame_count;
    a->rows = rows;
    a->columns = columns;
    a->current_frame = 0;
    a->frame_duration = 0;
    return a;
}

vVec2 vAnimationNextFrame(vAnimation a, u64 time_passed_ms) {
    a->frame_duration += time_passed_ms;
    if(a->frame_duration >= (1000 / a->fps)) {
        a->current_frame = (a->current_frame + 1) % a->frame_count;
        a->frame_duration = 0;
    }

    f32 frame_x = ((f32)a->frame_w) * (a->current_frame % a->columns);
    f32 frame_y = (f32)(a->frame_h) * (a->current_frame / a->columns);
    
    return (vVec2){
        .x = frame_x, 
        .y = frame_y
    };
}

void vAnimationDestroy(vAnimation a) {
    vFree(a->atlas);
    vFree(a);
}

vTexture vAnimationGetTexture(vAnimation a) {
    return a->atlas;
}

vVec2 vAnimationGetFrameSize(vAnimation a) {
    return (vVec2){ .x = a->frame_w, .y = a->frame_h };
}