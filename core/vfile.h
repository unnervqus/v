#pragma once

#include "core/vtypes.h"

u64 vFileLength(const char* path);

void vFileRead(const char* path, u8* buffer, u32 buffer_size);