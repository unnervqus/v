#include "audio/vaudio.h"
#include "core/vlogging.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

typedef struct vAudioContext {
    ma_engine engine;
    ma_sound music;
    bool music_loaded;
    bool initialized;
    ma_uint64 position;
} vAudioContext;

static vAudioContext v_audio = {
    .initialized = false
};

void vAudioInit() {
    if(v_audio.initialized) return;
    ma_engine_init(NULL, &v_audio.engine);
}

void vAudioPlayMusic(const char* path, bool loop) {
    ma_sound_init_from_file(
        &v_audio.engine,
        path,
        MA_SOUND_FLAG_STREAM,
        NULL,
        NULL,
        &v_audio.music
    );

    v_audio.music_loaded = true;

    if(loop) 
        ma_sound_set_looping(&v_audio.music, MA_TRUE);
    
    ma_sound_start(&v_audio.music);
    vLog("[AUDIO] Playing music from '%s'\n", path);
}

void vAudioPauseMusic() {
    ma_sound_get_cursor_in_pcm_frames(&v_audio.music, &v_audio.position);
    ma_sound_stop(&v_audio.music);
}

void vAudioResumeMusic() {
    ma_sound_seek_to_pcm_frame(&v_audio.music, v_audio.position);
    ma_sound_start(&v_audio.music);
}

void vAudioStopMusic() {
    ma_sound_stop(&v_audio.music);
    ma_sound_seek_to_pcm_frame(&v_audio.music, 0);
}

void vAudioSetMusicVolume(f32 volume) {
    if(volume < 0) volume = 0.0f;
    else if(volume > 1.0f) volume = 1.0f;
    ma_sound_set_volume(&v_audio.music, volume);
}

void vAudioShutdown() {
    ma_sound_uninit(&v_audio.music);
    ma_engine_uninit(&v_audio.engine);
}