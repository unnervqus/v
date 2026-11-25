#pragma once

#include "core/vtypes.h"

// Only supports one sound at a time
void vAudioInit();

void vAudioPlayMusic(const char* path, bool loop); // true — зациклить
void vAudioPauseMusic();
void vAudioResumeMusic();
void vAudioStopMusic();

void vAudioSetMusicVolume(f32 volume); // 0.0 — 1.0

void vAudioShutdown();