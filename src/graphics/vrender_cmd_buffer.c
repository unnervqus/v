#include "graphics/vrender_cmd_buffer.h"
#include "core/threading/vmutex.h"
#include "core/vring_buffer.h"

void vRenderCommandBufferPush(vRenderCommandBuffer* rcmd_buf, vRenderCommand command) {
    vRingBufferPush(rcmd_buf->buffer, &command);
}

vRenderCommand* vRenderCommandBufferPop(vRenderCommandBuffer* rcmd_buf) {
    vRenderCommand* rcmd = (vRenderCommand*)vRingBufferPop(rcmd_buf->buffer);
    return rcmd;
}