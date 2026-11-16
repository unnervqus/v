#include "graphics/vrenderer.h"
#include "graphics/context/vopengl.h"
#include "graphics/vrenderer.h"
#include "graphics/vshader.h"
#include "graphics/vvertex.h"
#include "platform/vwindow.h"
#include "core/vlogging.h"
#include "core/vmemory.h"

#include <GL/glew.h>
#include <stddef.h>

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
    if (!window)
        return NULL;

    vRenderer renderer = vMalloc(sizeof(*renderer));

    renderer->gl = vOpenGLContextCreate(window);

    if (!renderer)
        return NULL;

    renderer->width = vWindowGetWidth(window);
    renderer->height = vWindowGetHeight(window);

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
        sizeof(vVertex2D) * 4,
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

void vRendererEndFrame(vRenderer renderer) {
    if (!renderer)
        return;

    vOpenGLContextSwapBuffers(renderer->gl);
}

void vDrawTexture(
    vRenderer renderer,
    vTexture texture,
    vVec2 position,
    vVec2 size
) {
    if (!renderer || !texture)
        return;

    vVertex2D vertices[4] = {
        {
            { position.x, position.y },
            { 0.0f, 0.0f },
            { 1.0f, 1.0f, 1.0f, 1.0f }
        },
        {
            { position.x + size.x, position.y },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f, 1.0f, 1.0f }
        },
        {
            { position.x + size.x, position.y + size.y },
            { 1.0f, 1.0f },
            { 1.0f, 1.0f, 1.0f, 1.0f }
        },
        {
            { position.x, position.y + size.y },
            { 0.0f, 1.0f },
            { 1.0f, 1.0f, 1.0f, 1.0f }
        }
    };

    vShaderUse(renderer->shader);

    vTextureBind(texture, 0);

    glBindVertexArray(renderer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        sizeof(vertices),
        vertices
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