#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define WAV_PATH "PATH/TO/AUDIO.wav"

typedef struct {
    Uint32 audio_len;
    Uint8 *audio_pos;
} Audio_Data;

void audio_callback(void *userdata, Uint8 *stream, int len);

int main(int argc, char* argv[]){
    printf("Playing %s\n", WAV_PATH);

    // Init SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL init error: %s\n", SDL_GetError());
        return 1;
    }

    // @Question: can I get rid of these and just use the struct data???
    // local variables
    Uint32 wav_length; // length of our sample
    Uint8 *wav_buffer = 0; // buffer containing our audio file
    SDL_AudioSpec wav_spec; // the specs of our piece of music

    /* Load the WAV */
    // the specs, length and buffer of our wav are filled
    if (SDL_LoadWAV(WAV_PATH, &wav_spec, &wav_buffer, &wav_length) == NULL)
        return 1;

    // set the callback function
    wav_spec.callback = audio_callback;

    Audio_Data audio_data;
    audio_data.audio_pos = wav_buffer;
    audio_data.audio_len = wav_length;

    wav_spec.userdata = &audio_data;

    /* Open the audio device */
    if (SDL_OpenAudio(&wav_spec, NULL) < 0){
        printf("Couldn't open audio: %s\n", SDL_GetError());
        return 1;
    }

    /* Start playing */
    SDL_PauseAudio(0);

    // wait until we're done playing
    while (audio_data.audio_len > 0) {
        SDL_Delay(100);
    }

    // shut everything down
    SDL_CloseAudio();
    SDL_FreeWAV(wav_buffer);
}

void audio_callback(void *userdata, Uint8 *stream, int len) {

    Audio_Data *audio_data = (Audio_Data *) userdata;

    if (audio_data->audio_len ==0)
        return;

    len = (len > audio_data->audio_len ? audio_data->audio_len : len);
    memcpy(stream, audio_data->audio_pos, len);

    audio_data->audio_pos += len;
    audio_data->audio_len -= len;
}

