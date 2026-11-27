#pragma once

#include "graphics/vfont.h"

vFont vFontCreate(vFontData font_data);

u32 vFontGetID(vFont);
u32 vFontGetAtlasWidth(vFont);
u32 vFontGetAtlasHeight(vFont);
vGlyph* vFontGetGlyph(vFont font, u32 glyph);
