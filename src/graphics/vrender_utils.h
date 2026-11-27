#pragma once

#include "core/vtypes.h"

static inline f32 vUtilsPixelToUV(u32 pixel, u32 texture_size) {
    return (f32)pixel / (f32)texture_size;
}

static inline u32 vUtilsUVToPixel(f32 uv, u32 texture_size) {
    return (u32)(uv * (f32)texture_size);
}

static inline f32 vUtilsClamp(f32 value, f32 min, f32 max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

static inline bool vUtilsPointInRectF32(f32 x, f32 y, f32 rect_x, f32 rect_y, f32 rect_w, f32 rect_h) {
    return (x >= rect_x && x <= rect_x + rect_w) &&
           (y >= rect_y && y <= rect_y + rect_h);
}

static inline bool vUtilsPointInRectU32(u32 x, u32 y, u32 rect_x, u32 rect_y, u32 rect_w, u32 rect_h) {
    return (x >= rect_x && x <= rect_x + rect_w) &&
           (y >= rect_y && y <= rect_y + rect_h);
}