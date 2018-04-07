#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define WAV_PATH "PATH/TO/AUDIO.wav"

typedef struct {
    // Length of the audio that has been output and pointer to the audio that
    // has been output
    Uint32 audio_len;
    Uint8 *audio_pos;

    // Total length of the wav sample and buffer containing our audio file
    Uint32 wav_length;
    Uint8 *wav_buffer;
} Audio_Data;

void audio_callback(void *userdata, Uint8 *stream, int len);

int main(int argc, char* argv[]){
    printf("Playing %s\n", WAV_PATH);

    // Init SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL init error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_AudioSpec wav_spec;
    Audio_Data audio_data;

    // Load the WAV, the specs, length, and buffer of our wav are filled by this function
    if (SDL_LoadWAV(WAV_PATH, &wav_spec, &audio_data.wav_buffer, &audio_data.wav_length) == NULL) {
        printf("SDL LoadWAV error: %s\n", SDL_GetError());
        return 1;
    }

    // Start the audio pos at the beginning of our sample and start the audio
    // len the same len as the sample played so far (nothing at this point)
    audio_data.audio_pos = audio_data.wav_buffer;
    audio_data.audio_len = 0;

    // Set the callback function and point the wav_spec to our audio data struct
    wav_spec.callback = audio_callback;
    wav_spec.userdata = &audio_data;

    // Open the audio device
    if (SDL_OpenAudio(&wav_spec, NULL) < 0) { // @Update: according to SDL docs this is legacy and SDL_OpenAudioDevice should be used
        printf("Couldn't open audio: %s\n", SDL_GetError());
        return 1;
    }

    // Start playing
    SDL_PauseAudio(0);

    // Wait until we're done playing
    while (audio_data.audio_len < audio_data.wav_length) {
        SDL_Delay(100);
    }

    // Shut everything down
    SDL_CloseAudio();
    SDL_FreeWAV(audio_data.wav_buffer);

}

// This is our custom Audio callback function, SDL will call this function when
// it is ready for more data (audio) to be output
void audio_callback(void *userdata, Uint8 *stream, int len) {

    // Cast the userdata to Audio_Data so we can use it
    Audio_Data *audio_data = (Audio_Data *) userdata;

    // Return if we have already finished playing the wav
    if (audio_data->audio_len >= audio_data->wav_length)
        return;

    // Clip the length of the buffer to the end of the wav file if we are in
    // the last call and SDL wants more data than we have
    if ((audio_data->audio_len + len) > audio_data->wav_length)
        len = audio_data->wav_length - audio_data->audio_len;

    // Copy a len sized part of the wav to the output stream
    memcpy(stream, audio_data->audio_pos, len);

    // Move the buffer pointer and the audio length played so far ahead by the
    // amount sent to SDL
    audio_data->audio_pos += len;
    audio_data->audio_len += len;
}

