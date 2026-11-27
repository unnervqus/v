#include "graphics/context/vopengl.h"
#include "core/vlogging.h"
#include "core/vmemory.h"

#include <GL/glew.h>
#include <GL/wglew.h>

#define UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct vOpenGLContext {
    HWND hwnd;
    HDC hdc;
    HGLRC hctx;
};

static void vOpenGLContextCleanup(vOpenGLContext gl) {
    if(!gl) return;

    if (gl->hctx) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(gl->hctx);
        gl->hctx = NULL;
    }

    if (gl->hdc && gl->hwnd) {
        ReleaseDC(gl->hwnd, gl->hdc);
        gl->hdc = NULL;
    }
}

vOpenGLContext vOpenGLContextCreate(vWindow window) {
    vLog("[OpenGL] Creating context for window %p\n", window);
    HWND hwnd = vWindowGetNative(window);
    HDC hdc = GetDC(hwnd);

    if (!hdc) {
        vLog("[RENDERER THREAD] [window=%p] Failed to get window device context\n", window);
        goto fail;
    }

    PIXELFORMATDESCRIPTOR pfd = {0};

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL |
                  PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixelFormat = ChoosePixelFormat(hdc, &pfd);

    if (!pixelFormat) {
        vLog("[RENDERER THREAD] [window=%p] Failed to choose pixel format for window \n", window);
        goto fail;
    }

    if (!SetPixelFormat(hdc, pixelFormat, &pfd)) {
        vLog("[RENDERER THREAD] [window=%p] Failed to set pixel format for window\n", window);
        goto fail;
    }

    HGLRC tempContext = wglCreateContext(hdc);

    if (!tempContext) {
       vLog("[RENDERER THREAD] [window=%p] Failed to create temporal context\n", window);
        goto fail;
    }

    if (!wglMakeCurrent(hdc, tempContext)) {
        vLog("[RENDERER THREAD] [OpenGL] Temporary context activation failed\n");
        wglDeleteContext(tempContext);
        goto fail;
    }

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(tempContext);
        goto fail;
    }

    if (!WGLEW_ARB_create_context) {
        vLog("[RENDERER THREAD] [OpenGL] WGL_ARB_create_context unavailable\n");
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(tempContext);
        goto fail;
    }

    const int attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB,
        WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    HGLRC hctx = wglCreateContextAttribsARB(
        hdc,
        NULL,
        attribs
    );

    if (!hctx) {
        vLog("[RENDERER THREAD] [OpenGL] OpenGL 3.3 context creation failed\n");
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(tempContext);
        goto fail;
    }

    wglMakeCurrent(NULL, NULL);

    if (!wglMakeCurrent(hdc, hctx)) {
        vLog("[RENDERER THREAD] [OpenGL] Context activation failed\n");
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(tempContext);
        goto fail;
    }

    if (glewInit() != GLEW_OK) {
        vLog("[RENDERER THREAD] [OpenGL] GLEW initialization failed\n");
        goto fail;
    }

    wglDeleteContext(tempContext);

    if (WGLEW_EXT_swap_control) {
        wglSwapIntervalEXT(1);
    }

    vLog("[RENDERER THREAD] [OpenGL] OpenGL Version: %s\n", glGetString(GL_VERSION));
    vLog("[RENDERER THREAD] [OpenGL] Renderer: %s\n", glGetString(GL_RENDERER));

    vOpenGLContext gl = vMalloc(sizeof(struct vOpenGLContext));
        gl->hctx = hctx;
        gl->hwnd = hwnd;
        gl->hdc = hdc;

    vLog("[OpenGL] Context ready: %p\n", gl);
    return gl;

fail:
    if(gl) {
        vOpenGLContextCleanup(gl);
        vFree(gl);
    }
    return NULL;  
}

void vOpenGLContextDestroy(vOpenGLContext gl) {
    wglDeleteContext(gl->hctx);
}

void vOpenGLContextMakeCurrent(vOpenGLContext gl) {
    wglMakeCurrent(gl->hdc, gl->hctx);
}

void vOpenGLContextSwapBuffers(vOpenGLContext  gl) {
    SwapBuffers(gl->hdc);
}