#include "graphics/vrenderer.h"
#include "graphics/context/vopengl.h"
#include "core/vmemory.h"
#include "core/vlogging.h"

#include <GL/glew.h>

struct vRenderer {
    vOpenGLContext gl; 
};

vRenderer vRendererCreate(vWindow window) {
    vRenderer renderer = vMalloc(sizeof(struct vRenderer));

    renderer->gl = vOpenGLContextCreate(window);

    return renderer;
}

void vRendererBeginFrame(vRenderer r) {
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void vRendererEndFrame(vRenderer r) {
    vOpenGLContextSwapBuffers(r->gl);
}