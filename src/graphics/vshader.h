#pragma once

#include "core/vtypes.h"
#include "math/vmath.h"

typedef struct vShader* vShader;

vShader vShaderCreate(
    const char* vertex_shader_source, 
    const char* fragment_shader_source
);
void vShaderDestroy(vShader);
void vShaderUse(vShader);
u32 vShaderGetID(vShader);


i32 vShaderGetUniformLocation(
    vShader shader,
    const char* name
);

void vShaderSetInt(vShader shader, i32 location, i32 value);
void vShaderSetFloat(vShader shader, i32 location, float value);

void vShaderSetVec2(vShader shader, i32 location, vVec2 value);
void vShaderSetVec3(vShader shader, i32 location, vVec3 value);
void vShaderSetVec4(vShader shader, i32 location, vVec4 value);

void vShaderSetMat3(vShader shader, i32 location, const float* value);
void vShaderSetMat4(vShader shader, i32 location, const float* value);