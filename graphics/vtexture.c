#include "graphics/vtexture.h"
#include "core/vmemory.h"
#include "core/vlogging.h"

#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct vTexture {
    GLuint id;
    u32 width;
    u32 height;
};

static GLint vTextureGetGLFilter(vTextureFilter filter) {
    switch (filter) {
        case V_TEXTURE_FILTER_NEAREST:
            return GL_NEAREST;

        case V_TEXTURE_FILTER_LINEAR:
            return GL_LINEAR;

        default:
            return GL_LINEAR;
    }
}

static GLint vTextureGetGLWrap(vTextureWrap wrap) {
    switch (wrap) {
        case V_TEXTURE_WRAP_REPEAT:
            return GL_REPEAT;

        case V_TEXTURE_WRAP_CLAMP:
            return GL_CLAMP_TO_EDGE;

        default:
            return GL_CLAMP_TO_EDGE;
    }
}

vTexture vTextureCreate(const char* path) {
    vTextureSettings settings = {
        .filter = V_TEXTURE_FILTER_NEAREST,
        .wrap = V_TEXTURE_WRAP_CLAMP
    };

    return vTextureCreateEx(path, settings);
}

vTexture vTextureCreateEx(
    const char* path,
    vTextureSettings settings
) {
    if (!path)
        return NULL;

    int width = 0;
    int height = 0;
    int channels = 0;

    stbi_uc* pixels = stbi_load(
        path,
        &width,
        &height,
        &channels,
        STBI_rgb_alpha
    );

    if (!pixels) {
        vLog(
            "[TEXTURE] Failed to load '%s': %s\n",
            path,
            stbi_failure_reason()
        );
        return NULL;
    }

    vTexture texture = vMalloc(sizeof(*texture));

    if (!texture) {
        stbi_image_free(pixels);
        return NULL;
    }

    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        vTextureGetGLFilter(settings.filter)
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        vTextureGetGLFilter(settings.filter)
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        vTextureGetGLWrap(settings.wrap)
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        vTextureGetGLWrap(settings.wrap)
    );

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        pixels
    );

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(pixels);

    texture->width = (u32)width;
    texture->height = (u32)height;

    return texture;
}

void vTextureDestroy(vTexture texture) {
    if (!texture)
        return;

    glDeleteTextures(1, &texture->id);
    vFree(texture);
}

void vTextureBind(vTexture texture, u32 slot) {
    if (!texture)
        return;

    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture->id);
}

void vTextureUnbind(void) {
    glBindTexture(GL_TEXTURE_2D, 0);
}

u32 vTextureGetID(vTexture texture) {
    return texture ? texture->id : 0;
}

u32 vTextureGetWidth(vTexture texture) {
    return texture ? texture->width : 0;
}

u32 vTextureGetHeight(vTexture texture) {
    return texture ? texture->height : 0;
}