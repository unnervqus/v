#include "math/vmath.h"

inline vVec2 vVec2Multiply(vVec2 a, vVec2 b) {
    return (vVec2){ .x = a.x * b.x, .y = a.y * b.y };
}