#pragma once

#include "core/vtypes.h"
#include "core/vevents.h"
#include "core/vstring.h"

#include "graphics/vrender_api.h"

void vInit(vString title, u32 w, u32 h);

void vRun(
    void (*application_init)(), 
    void (*application_run)()
);

void vShutdown();
bool vIsRunning();
vRenderAPI* vGetRenderAPI();

void vAddEventListener(vEventType type, void (*callback)(const vEvent*));
void vRemoveEventListener(vEventType type, void (*callback)(const vEvent*));