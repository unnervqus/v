#include "ui/vui_element.h"
#include "ui/_vui_element.h"
#include "core/vmemory.h"

typedef struct vUIElementRegistry {
    vArray elements;
    u32 count;
    u32 capacity;
} vUIElementRegistry;

static vUIElementRegistry g_ui_element_registry = { 0 };

void vUIElementRegistryInit() {
    g_ui_element_registry.capacity = V_UI_ELEMENT_REGISTRY_INITIAL_CAPACITY;
    g_ui_element_registry.count = 0;
    g_ui_element_registry.elements = vArrayCreate(sizeof(vUIElement*), g_ui_element_registry.capacity);
}

vUIElement* vUIElementCreate(
    u32 x, u32 y, 
    u32 width, u32 height,
    bool use_texture,
    void (*on_hover)(void),
    void (*on_click)(void),
    void (*on_press)(void),
    void (*on_release)(void)
) {
    vUIElement* element = vMalloc(sizeof(vUIElement));

    if (!element)
        return NULL;

    element->x = x;
    element->y = y;
    element->width = width;
    element->height = height;

    element->is_hovered = false;
    element->is_pressed = false;
    element->is_focused = false;
    element->is_visible = true;
    element->is_enabled = true;

    element->on_hover = on_hover;
    element->on_click = on_click;
    element->on_press = on_press;
    element->on_release = on_release;

    element->use_texture = use_texture;
    element->pressed_texture = NULL;
    element->hover_texture = NULL;
    element->normal_texture = NULL;
    return element;
}

void vUIElementHide(vUIElement* element) {
    if (element) {
        element->is_visible = false;
    }
}

void vUIElementShow(vUIElement* element) {
    if (element) {
        element->is_visible = true;
    }
}

void vUIElementEnable(vUIElement* element) {
    if (element) {
        element->is_enabled = true;
    }
}

void vUIElementDisable(vUIElement* element) {
    if (element) {
        element->is_enabled = false;
    }
}

bool vUIElementRegister(vUIElement* element) {
    if(!element) return false;

    if(g_ui_element_registry.count < g_ui_element_registry.capacity) {
        vArrayPush(g_ui_element_registry.elements, &element);
        g_ui_element_registry.count++;
        return true;
    }

    return false;
}

bool vUIElementSetTexture(vUIElement* element, vUIElementTextureType tex_type, vTexture texture) {
    if(!element && !texture) return false;
    
    switch (tex_type) { 
    case V_UIE_TEXTURE_HOVER:
        element->hover_texture = texture;
        break;
    case V_UIE_TEXTURE_NORMAL:
        element->normal_texture = texture;
        break;
    case V_UIE_TEXTURE_PRESSED:
        element->pressed_texture = texture;
        break;
    }
    return true;
}

vArray vUIElementGetAllRegisteredElements() {
    return g_ui_element_registry.elements;
}

u32 vUIElementGetRegisteredElementCount() {
    return g_ui_element_registry.count;
}