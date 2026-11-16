#pragma once

#include "core/vevents.h"

typedef void (*vEventHandle)(const vEvent* event);

void vEventQueuePublish(const vEvent ev);
void vEventQueueDispatch();

void vEventQueueSubscribe(vEventType event_type, vEventHandle handle);