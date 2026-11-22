#include "core/v.h"
#include "graphics/vtexture.h"
#include "core/vlogging.h"

vTexture texture_0 = NULL;
vTextureData texture_data_0;
vRenderAPI* render;

void init() {
    texture_data_0 = vTextureLoad("assets/textures/test.png");
    render->create.texture(&texture_0, texture_data_0);
}

void run() {
    render->clear((vColor){.r = 0.1f, 0.1f, 0.8f, 1.0f});
    render->draw.texture(texture_0, (vVec2){.x = 100.0f, .y = 100.0f}, NULL);
    render->execute();
}

int main(void) {
    vString title = vStringCreate("V v0.1", 10);

    vInit(title, 1000, 800);

    render = vGetRenderAPI();
    vRun(init, run);

    return 0;
}