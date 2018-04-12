#include <stdio.h>
#include <SDL2/SDL.h>

#define WAV_PATH "PATH/TO/AUDIO.wav"

// Play a wav file using SDL's queue audio function

int main(int argc, char* argv[]){
    printf("Playing %s\n", WAV_PATH);

    // Init SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL init error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_AudioSpec loaded_wav_spec;
    SDL_AudioSpec obtained_wav_spec; // SDL may change the wav spec depending on the hardware, this spec will contain that data

    Uint32 wav_length;
    Uint8 *wav_buffer;

    // Load the wav file. The specs, length, and buffer of our wav are filled by this function
    if (SDL_LoadWAV(WAV_PATH, &loaded_wav_spec, &wav_buffer, &wav_length) == NULL) {
        printf("SDL LoadWAV error: %s\n", SDL_GetError());
        return 1;
    }

    // Open the audio device
    int iscapture = 0; // We are not opening the device for recording so iscapture = 0
    int allowed_changes = 0; // See docs for info on what this is for
    const char* device_name = SDL_GetAudioDeviceName(0, iscapture); // Gets the zero'th non-capture device name (see docs for more info)
    SDL_AudioDeviceID device = SDL_OpenAudioDevice(device_name, iscapture, &loaded_wav_spec, &obtained_wav_spec, allowed_changes);
    if (device == 0) {
        printf("SDL_OpenAudioDevice error: %s\n", SDL_GetError());
        return 1;
    }

    // Queue the entire wav file
    if (SDL_QueueAudio(device, wav_buffer, wav_length) != 0) {
        printf("SDL_QueueAudio error: %s\n", SDL_GetError());
        return 1;
    }

    // Start playing
    SDL_PauseAudioDevice(device, 0);

    // Wait until we're done playing
    while (SDL_GetQueuedAudioSize(device) > 0) {
        SDL_Delay(100);
    }

    // Shut everything down
    SDL_CloseAudioDevice(device);
    SDL_FreeWAV(wav_buffer);
    SDL_Quit();
}

