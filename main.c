#include "v.h"

void init() {
    vAudioPlayMusic("assets/music/bg.mp3", true);
}

void run() {
    v_render.execute();
}

int main(void) {

    vString title = vStringCreate("V v0.2", 10);
    vInit(title, 1000, 800);
    
    vRun(init, run);
    return 0;
}