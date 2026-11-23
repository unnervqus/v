#include "core/v.h"
#include "core/vevents.h"
#include "graphics/vtexture.h"

#include <stdio.h>

vTexture texture_0 = NULL;
vTextureData texture_data_0;
vRenderAPI* render;

void init() {
    texture_data_0 = vTextureLoad("vlogo_small.png");
    render->create.texture(&texture_0, texture_data_0);
}

vColor bg_color = { .r = 0.1f, 0.1f, 0.1f, 1.0f };

void run() {
    render->clear(bg_color);
    render->draw.texture(texture_0, (vVec2){.x = 450.0f, .y = 350.0f}, NULL);
    render->execute();
}

u32 engine_tick = 0;
void engine_tick_event(const vEvent *ev) {
    printf("Engine ticked %llu times\n\r", engine_tick++);
}

void key_event(const vEvent *ev) {
    if(ev->as.key_pressed.symbol == 'A') {
        vRemoveEventListener(V_EVENT_ENGINE_TICK, engine_tick_event);
    } else if(ev->as.key_pressed.symbol == 'S') {
        vAddEventListener(V_EVENT_ENGINE_TICK, engine_tick_event);
    }
}

int main(void) {
    vString title = vStringCreate("V v0.2", 10);

    vInit(title, 1000, 800);

    render = vGetRenderAPI();
    vAddEventListener(V_EVENT_KEY_PRESSED, key_event);
    vRun(init, run);

    return 0;
}