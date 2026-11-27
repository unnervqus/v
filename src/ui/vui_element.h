#pragma once

#include "core/vtypes.h"
#include "core/varray.h"
#include "graphics/vtexture.h"

typedef struct vUIElement {
    u32 x;
    u32 y;
    u32 width;
    u32 height;

    bool is_hovered; // Mouse cursor is currently over this element
    bool is_pressed; // Mouse button is currently pressed down on this element
    bool is_focused; // Should receive keyboard input
    bool is_visible; // Should be rendered
    bool is_enabled; // Should receive events

    void (*on_hover)(void); // Mouse cursor entered this element
    void (*on_click)(void); // Mouse button pressed and released on this element
    void (*on_press)(void); // Mouse button pressed down on this element
    void (*on_release)(void); // Mouse button released while over this element

    bool use_texture;
    vTexture pressed_texture;
    vTexture normal_texture;
    vTexture hover_texture;
} vUIElement;

typedef struct vUIButtonElement {
    vUIElement base;

    bool is_toggle;
    bool is_toggled;

    void (*on_toggle)(bool state);
} vUIButtonElement;

typedef enum vUIElementTextureType {
    V_UIE_TEXTURE_PRESSED,
    V_UIE_TEXTURE_NORMAL,
    V_UIE_TEXTURE_HOVER,
} vUIElementTextureType;

vUIElement* vUIElementCreate(
    u32 x, u32 y, 
    u32 width, u32 height,
    bool use_texture,
    void (*on_hover)(void),
    void (*on_click)(void),
    void (*on_press)(void),
    void (*on_release)(void)
);



void vUIElementDestroy(vUIElement*);

void vUIElementHide(vUIElement*);
void vUIElementShow(vUIElement*);

void vUIElementEnable(vUIElement*);
void vUIElementDisable(vUIElement*);

bool vUIElementRegister(vUIElement*);

bool vUIElementSetTexture(vUIElement* element, vUIElementTextureType tex_type, vTexture texture);

vArray vUIElementGetAllRegisteredElements();
u32 vUIElementGetRegisteredElementCount();