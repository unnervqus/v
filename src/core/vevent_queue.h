#pragma once

#include "core/vevents.h"

typedef void (*vEventHandle)(const vEvent* event);

void vEventQueueInit();

void vEventQueuePublish(const vEvent ev);
void vEventQueueDispatch();

void vEventQueueSubscribe(vEventType event_type, vEventHandle handle);
void vEventQueueUnsubscribe(vEventType event_type, vEventHandle handle);