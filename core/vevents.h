#pragma once

#include "core/vtypes.h"

typedef enum vEventType: int {
    V_EVENT_NONE = -1,

    V_EVENT_ENGINE_TICK,

    V_EVENT_MOUSE_RBUTTON,
    V_EVENT_MOUSE_LBUTTON,
    V_EVENT_MOUSE_MBUTTON,
    V_EVENT_MOUSE_MOVED,

    V_EVENT_KEY_RELEASED,
    V_EVENT_KEY_PRESSED,

    V_EVENT_WINDOW_MOVED,
    V_EVENT_WINDOW_CLOSED, // No special data
    V_EVENT_WINDOW_RESIZED,

    V_EVENT_WINDOW_GET_FOCUS, // No special data
    V_EVENT_WINDOW_LOST_FOCUS, // No special data

    V_EVENT_TYPE_COUNT
} vEventType;

typedef struct vEvent {
    vEventType type;
    u32 mouse_x;
    u32 mouse_y;
    union {
        struct {
            bool pressed;
            bool released;
        } mouse_rbutton;

        struct {
            bool pressed;
            bool released;
        } mouse_lbutton;

        struct {
            bool pressed;
            bool released;
        } mouse_mbutton;

        struct {
            u32 key;
            char symbol;
            bool shift;
            bool alt;
            bool ctrl;
        } key_pressed;

        struct {
            u32 key;
            char symbol;
            bool shift;
            bool alt;
            bool ctrl;
        } key_released;

        struct {
            i32 x;
            i32 y;
            i32 last_x;
            i32 last_y;
        } window_moved;

        struct {
            i32 w;
            i32 h;
            i32 last_w;
            i32 last_h;
        } window_resized;
    } as;
} vEvent;