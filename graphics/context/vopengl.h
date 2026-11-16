#pragma once

#include "platform/vwindow.h"

typedef struct vOpenGLContext* vOpenGLContext;

vOpenGLContext vOpenGLContextCreate(vWindow);
void vOpenGLContextDestroy(vOpenGLContext);
void vOpenGLContextMakeCurrent(vOpenGLContext);
void vOpenGLContextSwapBuffers(vOpenGLContext);