#pragma once

#include "graphics/vrender_api.h"
#include "core/vring_buffer.h"
#include "graphics/vtexture.h"
#include "math/vmath.h"

#include <core/threading/vsemaphore.h>

#define V_RCMD_MAX_COUNT 5000

typedef enum vRenderCommandType {
    V_RCMD_CREATE_TEXTURE,

    V_RCMD_CLEAR_SCREEN,
    V_RCMD_DRAW_TEXT,
    V_RCMD_DRAW_POINT,
    V_RCMD_DRAW_PRIMITIVE,
    V_RCMD_DRAW_TEXTURE,

    V_RCMD_DRAW_PRINT_SCENE,
    V_RCMD_DRAW_SCENE,

    V_RCMD_SET_LAYER,
    V_RCMD_SET_SCISSORS,
    V_RCMD_SET_VIEWPORT,
    V_RCMD_SET_BLEND_MODE,
    V_RCMD_EXECUTE,

    V_RCMD_COUNT
} vRenderCommandType;

typedef struct vRenderCommand {
    vRenderCommandType type;

    union {
        struct {
            vColor color;
        } clear_screen;

        struct {
            vTextureData texture_data;
            vTexture* texture;
        } create_texture;

        struct {
            vTexture texture;
            vVec2 pos;
            vRenderAttributes* attrs;
        } draw_texture;

    } as;

} vRenderCommand;

typedef struct vRenderCommandBuffer {
    vRingBuffer buffer;
    vSemaphore commands_executing;
    vSemaphore commands_available;
    u32 id;
} vRenderCommandBuffer;

void vRenderCommandBufferPush(vRenderCommandBuffer* rcmd_buf, vRenderCommand command);
vRenderCommand* vRenderCommandBufferPop(vRenderCommandBuffer* rcmd_buf);