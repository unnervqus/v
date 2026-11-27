#include "graphics/vfont.h"
#include "core/vmemory.h"
#include "core/vfile.h"
#include "core/vlogging.h"

#include <GL/glew.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

struct vFontData {
    stbtt_fontinfo info;
    f32 scale; // Use proportional scale
    vGlyph glyphs[255];
    u8* atlas;
    u32 atlas_w;
    u32 atlas_h;
};

struct vFont {
    GLuint id;
    u32 width;
    u32 height;
    vGlyph glyphs[255];
};

static u32 ascii_range_start = 32;
static u32 ascii_range_end = 126;

static void vFontMakeAtlas(vFontData font_data) {
    const u32 padding = 2;

    i32 atlas_width = padding;
    i32 max_glyph_height = 0;

    for (u32 i = ascii_range_start; i <= ascii_range_end; i++) {
        vGlyph* glyph = &font_data->glyphs[i];

        atlas_width += glyph->w + padding;

        if (glyph->h > max_glyph_height)
            max_glyph_height = glyph->h;
    }

    i32 atlas_height = max_glyph_height + padding * 2;

    u64 atlas_size = (u64)atlas_width * atlas_height;

    u8* atlas = vMalloc(sizeof(u8) * atlas_size);
    if (!atlas) {
        return;
    }

    memset(atlas, 0, atlas_size);

    font_data->atlas_w = atlas_width;
    font_data->atlas_h = atlas_height;
    font_data->atlas = atlas;

    i32 current_x = padding;
    i32 current_y = padding;

    for (u32 i = ascii_range_start; i <= ascii_range_end; i++) {
        vGlyph* glyph = &font_data->glyphs[i];

        i32 glyph_x = current_x;
        i32 glyph_y = current_y;

        if (glyph->bitmap && glyph->w > 0 && glyph->h > 0) {
            for (i32 y = 0; y < glyph->h; y++) {
                
                u8* dst = atlas
                    + (u64)(glyph_y + y) * atlas_width
                    + glyph_x;

                u8* src = glyph->bitmap 
                    + (u64)y * glyph->w;

                memcpy(dst, src, glyph->w);
            }
        }

        glyph->u0 = (f32)glyph_x / atlas_width;
        glyph->v0 = (f32)glyph_y / atlas_height;

        glyph->u1 = (f32)(glyph_x + glyph->w) / atlas_width;
        glyph->v1 = (f32)(glyph_y + glyph->h) / atlas_height;

        current_x += glyph->w + padding;
    }
}

vFontData vFontLoad(const char* path, f32 size) {
    vFontData font_data = vMalloc(sizeof(struct vFontData));

    u32 file_len = vFileLength(path);

    u8* font_buffer = vMalloc(sizeof(u8) * file_len);

    vFileRead(path, font_buffer, file_len);

    i32 font_offset = stbtt_GetFontOffsetForIndex(font_buffer, 0);

    stbtt_InitFont(&font_data->info, font_buffer, font_offset);

    font_data->scale = stbtt_ScaleForPixelHeight(
        &font_data->info,
        size
    );

    // Get every ascii symbol
    for (u32 i = ascii_range_start; i <= ascii_range_end; i++) {
        
        vGlyph* glyph = &font_data->glyphs[i];
        i32 xoff, yoff;

        glyph->bitmap = stbtt_GetCodepointBitmap(
            &font_data->info,
            0,
            font_data->scale,
            i,
            &glyph->w,
            &glyph->h,
            &xoff,
            &yoff
        );

        glyph->bearingX = (f32)xoff;
        glyph->bearingY = (f32)-yoff;

        i32 unused;;
        i32 advance;

        stbtt_GetCodepointHMetrics(
            &font_data->info,
            i,
            &advance,
            &unused
        );

        glyph->advance = advance * font_data->scale;
    }

    vFontMakeAtlas(font_data);

    return font_data;
}

vFont vFontCreate(vFontData font_data) {
    if (!font_data) {
        vLog("[FONT] Create failed: font data is null\n");
        return NULL;
    }

    //TODO: implement vGLCreateTexture 
    // to use both for font and texture creating

    vFont font = vMalloc(sizeof(struct vFont));

    // Get new texture id
    glGenTextures(1, &font->id);

    // Set texture as current via its id
    glBindTexture(GL_TEXTURE_2D, font->id);

    // Use linear antialiasing when decreasing texute size
    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR
    );

    // Use linear antialiasing when increasing texute size
    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );

    // No repeat on x-axis
    glTexParameteri(
        GL_TEXTURE_2D, 
        GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE
    );

    // No repeat on y-axis
    glTexParameteri(
        GL_TEXTURE_2D, 
        GL_TEXTURE_WRAP_T,
        GL_CLAMP_TO_EDGE
    );

    GLint previous_alignment;

    glGetIntegerv(GL_UNPACK_ALIGNMENT, &previous_alignment);

    // TODO: ???
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Use R8?? 
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_R8,
        font_data->atlas_w,
        font_data->atlas_h,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        font_data->atlas
    );

    glPixelStorei(GL_UNPACK_ALIGNMENT, previous_alignment);

    // Unlink texture
    glBindTexture(GL_TEXTURE_2D, 0);

    font->width = font_data->atlas_w;
    font->height = font_data->atlas_h;
    memcpy(font->glyphs, font_data->glyphs, sizeof(vGlyph) * 255);

    return font;
}

u32 vFontGetID(vFont font) {
    return font->id;
}
u32 vFontGetAtlasWidth(vFont font) {
    return font->width;
}

u32 vFontGetAtlasHeight(vFont font) {
    return font->height;
}

vGlyph* vFontGetGlyph(vFont font, u32 glyph) {
    return &font->glyphs[glyph];
}