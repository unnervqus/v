#pragma once

#include "math/vmath.h"

static inline bool vPointInsideRectangleF32(vVec2 point, vVec2 rect_pos, vVec2 rect_size) {
    return (point.x >= rect_pos.x && point.x <= rect_pos.x + rect_size.x &&
            point.y >= rect_pos.y && point.y <= rect_pos.y + rect_size.y);
}

static inline bool vPointInsideRectangleU32(vVec2 point, vVec2 rect_pos, vVec2 rect_size) {
    return (point.x >= rect_pos.x && point.x <= rect_pos.x + rect_size.x &&
            point.y >= rect_pos.y && point.y <= rect_pos.y + rect_size.y);
}