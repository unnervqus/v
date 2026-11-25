#include "graphics/vrender_cmd_buffer.h"
#include "graphics/context/vopengl.h"
#include "graphics/vrenderer.h"
#include "graphics/vshader.h"
#include "graphics/vtexture_internal.h"
#include "graphics/vvertex.h"
#include "graphics/vfont_internal.h"

#include "core/vring_buffer.h"
#include "core/vmemory.h"
#include "core/vlogging.h"
#include "core/varray.h"

#include "ui/vui_element.h"

#include "platform/vwindow.h"

#include <GL/glew.h>

struct vRenderer {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    vShader shader;

    GLint projection_location;
    GLint texture_location;

    float projection[16];

    u32 width;
    u32 height;
    vOpenGLContext gl;

    vRenderCommandBuffer* rcmd_buf;
};

static void vRendererCreateProjection(
    float* matrix,
    float width,
    float height
) {
    for (u32 i = 0; i < 16; i++)
        matrix[i] = 0.0f;

    matrix[0] =  2.0f / width;
    matrix[5] = -2.0f / height;
    matrix[10] = -1.0f;
    matrix[12] = -1.0f;
    matrix[13] =  1.0f;
    matrix[15] =  1.0f;
}

vRenderer vRendererCreate(vWindow window) {
    if (!window) {
        vLog("[RENDERER] Create failed: window is null\n");
        return NULL;
    }

    vRenderer renderer = vMalloc(sizeof(*renderer));

    if (!renderer) {
        vLog("[RENDERER] Create failed: allocation failed\n");
        return NULL;
    }

    renderer->gl = vOpenGLContextCreate(window);
    renderer->width = vWindowGetWidth(window);
    renderer->height = vWindowGetHeight(window);

    renderer->rcmd_buf = NULL;

    vRendererCreateProjection(
        renderer->projection,
        (float)renderer->width,
        (float)renderer->height
    );

    renderer->shader = vShaderCreate(
        "assets/shaders/texture.vert",
        "assets/shaders/texture.frag"
    );

    if (!renderer->shader) {
        vLog("[RENDERER] Create failed: shader creation failed\n");
        vFree(renderer);
        return NULL;
    }

    renderer->projection_location =
        vShaderGetUniformLocation(
            renderer->shader,
            "uProjection"
        );

    renderer->texture_location =
        vShaderGetUniformLocation(
            renderer->shader,
            "uTexture"
        );

    const GLuint indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &renderer->vao);
    glGenBuffers(1, &renderer->vbo);
    glGenBuffers(1, &renderer->ebo);

    glBindVertexArray(renderer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vVertex2D) * 6,
        NULL,
        GL_DYNAMIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->ebo);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indices),
        indices,
        GL_STATIC_DRAW
    );

    // Position
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(vVertex2D),
        (void*)offsetof(vVertex2D, position)
    );

    glEnableVertexAttribArray(0);

    // Texture coordinates
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(vVertex2D),
        (void*)offsetof(vVertex2D, tex_coord)
    );

    glEnableVertexAttribArray(1);

    // Color
    glVertexAttribPointer(
        2,
        4,
        GL_FLOAT,
        GL_FALSE,
        sizeof(vVertex2D),
        (void*)offsetof(vVertex2D, color)
    );

    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return renderer;
}

void vRendererDestroy(vRenderer renderer) {
    if (!renderer)
        return;

    glDeleteBuffers(1, &renderer->vbo);
    glDeleteBuffers(1, &renderer->ebo);
    glDeleteVertexArrays(1, &renderer->vao);

    vShaderDestroy(renderer->shader);

    vFree(renderer);
}

void vRendererBeginFrame(vRenderer renderer) {
    if (!renderer)
        return;

    glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    vShaderUse(renderer->shader);

    vShaderSetMat4(
        renderer->shader,
        renderer->projection_location,
        renderer->projection
    );

    vShaderSetInt(
        renderer->shader,
        renderer->texture_location,
        0
    );
}

void vRendererExecuteCommands(vRenderer renderer, vRenderCommandBuffer* rcmd_buf) {
    if (!renderer) {
        vLog("[RENDERER] Command execution skipped: renderer is null\n");
        return;
    }

    vSemaphoreWait(rcmd_buf->commands_executing);

    u32 commands_to_execute = vRingBufferGetCount(rcmd_buf->buffer);
    
    for (u32 i = 0; i < commands_to_execute; i++) {
        vRenderCommand* rcmd = vRenderCommandBufferPop(rcmd_buf);

        switch (rcmd->type) {
        case V_RCMD_CLEAR_SCREEN:
            vRendererClearScreen(renderer, rcmd->as.clear_screen.color);
            break;
        case V_RCMD_CREATE_TEXTURE:
            *rcmd->as.create_texture.texture = vTextureCreate(rcmd->as.create_texture.texture_data);
            break;
        case V_RCMD_DRAW_TEXTURE:
            vRendererDrawTexture(
                renderer, 
                rcmd->as.draw_texture.texture, 
                rcmd->as.draw_texture.pos, 
                (vVec2){
                    .x = vTextureGetWidth(rcmd->as.draw_texture.texture), 
                    .y = vTextureGetHeight(rcmd->as.draw_texture.texture)
                }
            );
            break;
        case V_RCMD_CREATE_FONT: {
            *rcmd->as.create_font.font = vFontCreate(rcmd->as.create_font.font_data);
            break;
        }
        case V_RCMD_DRAW_FONT: {
            vRendererDrawFont(
                renderer,
                rcmd->as.draw_font.font,
                rcmd->as.draw_font.pos
            );
            break;
        }
        case V_RCMD_DRAW_TEXT: {
            vRendererDrawText(
                renderer,
                rcmd->as.draw_text.font,
                rcmd->as.draw_text.text,
                rcmd->as.draw_text.pos
            );
            break;
        }
        case V_RCMD_SET_VIEWPORT: {
            vRendererResize(
                renderer, 
                rcmd->as.set_viewport.viewport.w, 
                rcmd->as.set_viewport.viewport.h);
            break;
        }
        default:
            break;
        }
    }

    vSemaphorePost(rcmd_buf->commands_available);
}

void vRendererEndFrame(vRenderer renderer) {
    if (!renderer)
        return;

    vOpenGLContextSwapBuffers(renderer->gl);
}

void vRendererDrawTexture(
    vRenderer renderer,
    vTexture texture,
    vVec2 position,
    vVec2 size
) {
    vRendererDrawTextureRegion(
        renderer,
        texture,
        (vVec2){ 0.0f, 0.0f },
        (vVec2){ (f32)vTextureGetWidth(texture), (f32)vTextureGetHeight(texture) },
        position,
        size
    );
}

void vRendererDrawTextureRegion(
    vRenderer renderer, vTexture texture, 
    vVec2 position_src, vVec2 size_src, 
    vVec2 position_dst, vVec2 size_dst
) {

    if (!renderer || !texture)
        return;

    f32 texture_width = (f32)vTextureGetWidth(texture);
    f32 texture_height = (f32)vTextureGetHeight(texture);
    
    vVec4 white = { 1.0f, 1.0f, 1.0f, 1.0f };
    vQuad2D quad = vQuad2DCreate(
        position_dst.x, position_dst.y,
        size_dst.x, size_dst.y,
        (vVec2){ 
            position_src.x / texture_width, 
            position_src.y / texture_height },
        (vVec2){
            (position_src.x + size_src.x) / texture_width, 
            (position_src.y + size_src.y) / texture_height },
        (vVec4[]){ 
            white, white, 
            white, white }
    );

    vShaderUse(renderer->shader);

    u32 loc = vShaderGetUniformLocation(renderer->shader, "uIsFont");
    glUniform1i(loc, GL_FALSE);

    vTextureBind(texture, 0);

    glBindVertexArray(renderer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        sizeof(quad),
        &quad
    );

    glDrawElements(
        GL_TRIANGLES,
        6,
        GL_UNSIGNED_INT,
        0
    );

    glBindVertexArray(0);
    vTextureUnbind();
}

void vRendererDrawFont(
    vRenderer renderer, vFont font,
    vVec2 position
) {
    if (!renderer || !font)
        return;

    f32 w = vFontGetAtlasWidth(font);
    f32 h = vFontGetAtlasHeight(font);

    vVec4 white = {1.0f, 1.0f, 1.0f, 1.0f};
    
    vQuad2D quad = vQuad2DCreate(
        position.x, position.y, 
        w, h, 
        (vVec2){0.0f, 0.0f},
        (vVec2){1.0f, 1.0f},
        (vVec4[]){ 
            white, white, 
            white, white }
    );   

    vShaderUse(renderer->shader);

    u32 loc = vShaderGetUniformLocation(renderer->shader, "uIsFont");
    glUniform1i(loc, GL_TRUE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, vFontGetID(font));

    glBindVertexArray(renderer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        sizeof(quad),
        &quad
    );

    glDrawElements(
        GL_TRIANGLES,
        6,
        GL_UNSIGNED_INT,
        0
    );

    glBindVertexArray(0);
    vTextureUnbind();
}

#define V_MAX_TEXT_VERTICES 600
void vRendererDrawText(
    vRenderer renderer, vFont font,
    vString text, vVec2 position
) {
    if(!renderer || !font)
        return;

    vShaderUse(renderer->shader);

    u32 loc = vShaderGetUniformLocation(renderer->shader, "uIsFont");
    glUniform1i(loc, GL_TRUE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, vFontGetID(font));

    glBindVertexArray(renderer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);

    const char* symbols = vStringGetData(text);
    u32 symbol_count = vStringGetSize(text);

    f32 pos_x = position.x;
    f32 pos_y = position.y;

    vVec4 white = {1.0f, 1.0f, 1.0f, 1.0f};

    f32 offset_x = 0;
    f32 padding_x = 2;

    for(u32 i = 0; i < symbol_count; i++) {
        if(symbols[i] == ' ') { offset_x += 18; continue; }
        vGlyph* glyph = vFontGetGlyph(font, symbols[i]);
        vQuad2D quad = vQuad2DCreate(
            pos_x + offset_x, pos_y - glyph->bearingY,
            glyph->w, glyph->h,
            (vVec2){ glyph->u0, glyph->v0 },
            (vVec2){ glyph->u1, glyph->v1 },
            (vVec4[]){ 
                white, white, 
                white, white }
            );

        glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        sizeof(quad),
        &quad
        );

        glDrawElements(
            GL_TRIANGLES,
            6,
            GL_UNSIGNED_INT,
            0
        );
        offset_x += glyph->w + padding_x;
    }

    glBindVertexArray(0);
    vTextureUnbind();
}

void vRendererResize(vRenderer renderer, u32 width, u32 height) {
    if (!renderer || !width || !height)
        return;

    renderer->width = width;
    renderer->height = height;

    glViewport(0, 0, width, height);

    vRendererCreateProjection(
        renderer->projection,
        (float)width,
        (float)height
    );
}

void vRendererClearScreen(vRenderer renderer, vColor color) {
    _CRT_UNUSED(renderer); 
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void vRendererDrawUI(vRenderer renderer, vArray elements) {
    u32 element_count = vArrayGetSize(elements);

    for(u32 i = 0; i < element_count; ++i) {
        vTexture texture = NULL;
        // TODO: Render should not know what vUIELement is
        vUIElement *element = *(vUIElement**)vArrayGet(elements, i);
        
        if(element->is_visible) {
            if(element->use_texture) {
                if(element->is_pressed && element->pressed_texture) 
                texture = element->pressed_texture;
                else if(element->is_hovered && element->hover_texture)
                texture = element->hover_texture;
                else if(element->normal_texture)
                texture = element->normal_texture;
            } else {
                // Draw with standard style
            }
        }
        if(texture) {
            vRendererDrawTexture(
                renderer,
                texture,
                (vVec2){ (f32)element->x, (f32)element->y },
                (vVec2){ (f32)element->width, (f32)element->height }
            );
        }
    }

}