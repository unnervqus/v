#include "core/threading/vmutex.h"
#include "core/threading/vsemaphore.h"
#include "graphics/vrender_cmd_buffer.h"
#include "graphics/vrender_api.h"
#include "graphics/vrenderer.h"
#include "core/vring_buffer.h"
#include "core/vlogging.h"
#include "core/vmemory.h"

typedef struct vRenderData {
    vRenderer target;
    vRenderCommandBuffer* rcmd_buf[2];
    u32 active_cmd_buf;
} vRenderData;

vRenderAPI v_render = {
    .is_ready = false
};

void vRCMDClearScreen(vColor color) {
    vRenderCommand command = {
        .type = V_RCMD_CLEAR_SCREEN,
        .as.clear_screen = {
            .color = color
        }
    };
    vRenderCommandBufferPush(
        v_render.data->rcmd_buf[
            v_render.data->active_cmd_buf
        ], command);
}

void vRCMDDrawTexture(vTexture texture, vVec2 pos, vRenderAttributes* attrs) {
    vRenderCommand command = {
        .type = V_RCMD_DRAW_TEXTURE,
        .as.draw_texture = {
            .texture = texture,
            .attrs = attrs,
            .pos = pos
        }
    };

    vRenderCommandBufferPush(
        v_render.data->rcmd_buf[
            v_render.data->active_cmd_buf
        ], command);
}

void vRCMDCreateTexture(vTexture *texture_out, vTextureData texture_data) {
    vRenderCommand command = {
        .type = V_RCMD_CREATE_TEXTURE,
        .as.create_texture = {
            .texture = texture_out,
            .texture_data = texture_data
        }
    };

    vRenderCommandBufferPush(
        v_render.data->rcmd_buf[
            v_render.data->active_cmd_buf
        ], command);
}

static void vRCMDExecute() {
    vLog(
        "[RENDER API] Preparing rcmd #%u\n", 
        v_render.data->rcmd_buf[v_render.data->active_cmd_buf]->id
    );
    u32 submitted = v_render.data->active_cmd_buf;
    
    vSemaphorePost(v_render.data->rcmd_buf[submitted]->commands_executing);

    v_render.data->active_cmd_buf = (submitted + 1) % 2;
    
    vLog(
        "[RENDER API] Waiting for rcmd #%u\n", 
        v_render.data->rcmd_buf[v_render.data->active_cmd_buf]->id
    );
    vSemaphoreWait(v_render.data->rcmd_buf[v_render.data->active_cmd_buf]->commands_available);
}

void vRenderAPIInit(vRenderer renderer) {
    if(v_render.is_ready) return;

    v_render.data = vMalloc(sizeof(vRenderData));
    v_render.data->rcmd_buf[0] = vMalloc(sizeof(vRenderCommandBuffer));
    v_render.data->rcmd_buf[1] = vMalloc(sizeof(vRenderCommandBuffer));
    
    v_render.data->rcmd_buf[0]->buffer = vRingBufferCreate(
        sizeof(vRenderCommand), 
        V_RCMD_MAX_COUNT
    );

    v_render.data->rcmd_buf[1]->buffer = vRingBufferCreate(
        sizeof(vRenderCommand), 
        V_RCMD_MAX_COUNT
    );
    
    v_render.data->active_cmd_buf = 0;

    v_render.data->target = renderer;

    v_render.data->rcmd_buf[0]->commands_executing = vSemaphoreCreate(0);
    v_render.data->rcmd_buf[0]->commands_available = vSemaphoreCreate(1);
    v_render.data->rcmd_buf[1]->commands_executing = vSemaphoreCreate(0);
    v_render.data->rcmd_buf[1]->commands_available = vSemaphoreCreate(1);

    vSemaphoreWait(
        v_render.data->rcmd_buf[0]->commands_available
    );

    v_render.data->rcmd_buf[0]->id = 0;
    v_render.data->rcmd_buf[1]->id = 1;

    v_render.clear = vRCMDClearScreen;
    v_render.execute = vRCMDExecute;
    
    // render->text = vRCMDDrawText;
    // render->point = vRCMDDrawPoint;
    // render->text = vRCMDDrawText;
    // render->primitive = vRCMDDrawPrimitive;
    v_render.draw.texture = vRCMDDrawTexture;
    v_render.create.texture = vRCMDCreateTexture;
    
    // render->set_blend_mode = vRCMDSetBlendMode;
    // render->set_layer = vRCMDSetLayer;
    // render->set_scissor = vRCMDSetScissors;
    // render->set_viewport = vRCMDSetViewport;
    v_render.is_ready = true;
    vLog("[RENDERER THREAD] Render API initialized\n");
}

vRenderCommandBuffer* vRenderAPIGetCommandBuffer(u32 index) {
    return v_render.data->rcmd_buf[index];
}