#include "graphics/vtexture_internal.h"
#include "core/vmemory.h"
#include "core/vlogging.h"

#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct vTextureData {
    i32 width;
    i32 height;
    u8* pixels;
};

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

vTextureData vTextureLoad(const char* path) {
    if (!path) {
        vLog("[TEXTURE] Load failed: path is null\n");
        return NULL;
    }

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

    vTextureData texture_data = vMalloc(sizeof(struct vTextureData));

    texture_data->pixels = (u8*)pixels;
    texture_data->width = (i32)width;
    texture_data->height = (i32)height;

    return texture_data;
}

vTexture vTextureCreate(const vTextureData texture_data) {
    vTextureSettings settings = {
        .filter = V_TEXTURE_FILTER_NEAREST,
        .wrap = V_TEXTURE_WRAP_CLAMP
    };

    return vTextureCreateEx(texture_data, settings);
}

vTexture vTextureCreateEx(
    const vTextureData texture_data,
    vTextureSettings settings
) {
    if (!texture_data) {
        vLog("[TEXTURE] Create failed: image data is null\n");
        return NULL;
    }

    vTexture texture = vMalloc(sizeof(*texture));

    if (!texture) {
        vLog("[TEXTURE] Create failed: allocation failed\n");
        stbi_image_free(texture_data->pixels);
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
        texture_data->width,
        texture_data->height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        texture_data->pixels
    );

    glBindTexture(GL_TEXTURE_2D, 0);

    texture->width = (u32)texture_data->width;
    texture->height = (u32)texture_data->height;

    stbi_image_free(texture_data->pixels);

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