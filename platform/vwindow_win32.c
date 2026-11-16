#include "platform/vwindow.h"
#include "core/vmemory.h"
#include "core/vevents.h"
#include "core/vevent_queue.h"

#define WIN32_LEAN_AND_MEAN
#define UNICODE
#include <windows.h>
#include <windowsx.h>

struct vWindow {
    u32 h;
    u32 w;
    HWND handle;
    char* title;
};

static LPCWSTR v_class = L"VCLASS";

static void vHandleMouseButton(HWND hwnd, vEventType type, bool pressed, LPARAM lp) {
    vEvent ev = {
        .type = type,
        .mouse_x = (u32)(i32)GET_X_LPARAM(lp),
        .mouse_y = (u32)(i32)GET_Y_LPARAM(lp)
    };

    switch (type) {
        case V_EVENT_MOUSE_LBUTTON:
            ev.as.mouse_lbutton.pressed = pressed;
            ev.as.mouse_lbutton.released = !pressed;
            ev.as.mouse_lbutton.hold = pressed;
            break;

        case V_EVENT_MOUSE_RBUTTON:
            ev.as.mouse_rbutton.pressed = pressed;
            ev.as.mouse_rbutton.released = !pressed;
            ev.as.mouse_rbutton.hold = pressed;
            break;

        case V_EVENT_MOUSE_MBUTTON:
            ev.as.mouse_mbutton.pressed = pressed;
            ev.as.mouse_mbutton.released = !pressed;
            ev.as.mouse_mbutton.hold = pressed;
            break;

        default:
            return;
    }

    if (pressed)
        SetCapture(hwnd);
    else
        ReleaseCapture();

    vEventQueuePublish(ev);
}

static bool vIsKeyDown(int key) {
    return (GetKeyState(key) & 0x8000) != 0;
}

static void vGetModifiers(vEvent *ev) {
    ev->as.key_pressed.shift = vIsKeyDown(VK_SHIFT);
    ev->as.key_pressed.ctrl  = vIsKeyDown(VK_CONTROL);
    ev->as.key_pressed.alt   = vIsKeyDown(VK_MENU);
}

static void vGetModifiersReleased(vEvent *ev) {
    ev->as.key_released.shift = vIsKeyDown(VK_SHIFT);
    ev->as.key_released.ctrl  = vIsKeyDown(VK_CONTROL);
    ev->as.key_released.alt   = vIsKeyDown(VK_MENU);
}

static char vGetKeySymbol(WPARAM key, LPARAM lp) {
    BYTE keyboard[256];
    WCHAR chars[8];

    if (!GetKeyboardState(keyboard))
        return 0;

    UINT scan = (lp >> 16) & 0xFF;

    int count = ToUnicode((UINT)key, scan, keyboard, chars, 8, 0);

    if (count == 1 && chars[0] >= 32 && chars[0] <= 126)
        return (char)chars[0];

    return 0;
}

static void vHandleKeyDown(WPARAM wp, LPARAM lp) {
    if (lp & (1LL << 30))
        return;

    vEvent ev = {
        .type = V_EVENT_KEY_PRESSED
    };

    ev.as.key_pressed.key = (u32)wp;
    ev.as.key_pressed.symbol = vGetKeySymbol(wp, lp);

    vGetModifiers(&ev);

    vEventQueuePublish(ev);
}

static void vHandleKeyUp(WPARAM wp, LPARAM lp) {
    vEvent ev = {
        .type = V_EVENT_KEY_RELEASED
    };

    ev.as.key_released.key = (u32)wp;
    ev.as.key_released.symbol = vGetKeySymbol(wp, lp);

    vGetModifiersReleased(&ev);

    vEventQueuePublish(ev);
}

static LRESULT CALLBACK vWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

    static i32 last_x = 0;
    static i32 last_y = 0;
    static i32 last_w = 0;
    static i32 last_h = 0;

    switch (msg) {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        vHandleKeyDown(wp, lp);
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
        vHandleKeyUp(wp, lp);
        break;

    case WM_LBUTTONDOWN:
        vHandleMouseButton(
            hwnd, V_EVENT_MOUSE_LBUTTON, true, lp
        );
        return 0;

    case WM_LBUTTONUP:
        vHandleMouseButton(
            hwnd, V_EVENT_MOUSE_LBUTTON, false, lp
        );
        return 0;

    case WM_RBUTTONDOWN:
        vHandleMouseButton(
            hwnd, V_EVENT_MOUSE_RBUTTON, true, lp
        );
        return 0;

    case WM_RBUTTONUP:
        vHandleMouseButton(
            hwnd, V_EVENT_MOUSE_RBUTTON, false, lp
        );
        return 0;

    case WM_MBUTTONDOWN:
        vHandleMouseButton(
            hwnd, V_EVENT_MOUSE_MBUTTON, true, lp
        );
        return 0;

    case WM_MBUTTONUP:
        vHandleMouseButton(
            hwnd, V_EVENT_MOUSE_MBUTTON, false, lp
        );
        return 0;

    case WM_MOUSEMOVE: {
        vEvent ev = {
            .type = V_EVENT_MOUSE_MOVED,
            .mouse_x = (u32)(i32)GET_X_LPARAM(lp),
            .mouse_y = (u32)(i32)GET_Y_LPARAM(lp)
        };

        vEventQueuePublish(ev);
        return 0;
    }
    case WM_MOVE: {
        i32 x = (i16)LOWORD(lp);
        i32 y = (i16)HIWORD(lp);

        vEvent ev = {
            .type = V_EVENT_WINDOW_MOVED
        };

        ev.as.window_moved.x = x;
        ev.as.window_moved.y = y;
        ev.as.window_moved.last_x = last_x;
        ev.as.window_moved.last_y = last_y;

        last_x = x;
        last_y = y;

        vEventQueuePublish(ev);
        return 0;
    }

    case WM_SIZE: {
        i32 w = (i32)LOWORD(lp);
        i32 h = (i32)HIWORD(lp);

        vEvent ev = {
            .type = V_EVENT_WINDOW_RESIZED
        };

        ev.as.window_resized.w = w;
        ev.as.window_resized.h = h;
        ev.as.window_resized.last_w = last_w;
        ev.as.window_resized.last_h = last_h;

        last_w = w;
        last_h = h;

        vEventQueuePublish(ev);
        return 0;
    }
    case WM_SETFOCUS: {
        vEvent ev = {
            .type = V_EVENT_WINDOW_GET_FOCUS
        };

        vEventQueuePublish(ev);
        return 0;
    }

    case WM_KILLFOCUS: {
        vEvent ev = {
            .type = V_EVENT_WINDOW_LOST_FOCUS
        };

        vEventQueuePublish(ev);
        return 0;
    }
    
    case WM_CLOSE: {
        vEvent ev = {
            .type = V_EVENT_WINDOW_CLOSED
        };

        vEventQueuePublish(ev);
        return DefWindowProcW(hwnd, msg, wp, lp);
    }

    case WM_DESTROY:
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wp, lp);
}

vWindow vWindowCreate(const char* title, u32 w, u32 h) {
    HINSTANCE h_instance = GetModuleHandleW(NULL);

    WNDCLASSW wc;
    
    wc.lpszClassName = v_class;
    wc.lpszMenuName = NULL;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.hInstance = h_instance;
    wc.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
    wc.hIcon = LoadIconW(NULL, IDI_APPLICATION);
    wc.lpfnWndProc = vWndProc;

    if(!RegisterClassW(&wc)) {
        return NULL;
    }

    RECT rect = {
        .left   = 0,
        .top    = 0,
        .right  = (LONG)w,
        .bottom = (LONG)h
    };

    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hwnd = CreateWindowW(
        v_class, 
        L"V", 
        WS_OVERLAPPEDWINDOW, 
        CW_USEDEFAULT, CW_USEDEFAULT, 
        rect.right - rect.left, 
        rect.bottom - rect.top, 
        NULL, NULL, 
        h_instance, 
        NULL
    );

    if(!hwnd) {
        UnregisterClassW(v_class, h_instance);
        return NULL;
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);

    vWindow window = vMalloc(sizeof(struct vWindow));
    window->handle = hwnd;
    window->w = w;
    window->h = h;
    window->title = "TEST_TITLE";

    return window;
}

void vWindowDestroy(vWindow w) {
    DestroyWindow(w->handle);
    vFree(w);
}

void* vWindowGetNative(vWindow w) {
    return w->handle;
}

void vWindowPollEvents(vWindow w) {
    MSG msg;
    while(PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

u32 vWindowGetWidth(vWindow w) {
    RECT rect;
    GetClientRect(w->handle, &rect);
    return (u32)(rect.right - rect.left);
}

u32 vWindowGetHeight(vWindow w) {
    RECT rect;
    GetClientRect(w->handle, &rect);
    return (u32)(rect.bottom - rect.top);
}