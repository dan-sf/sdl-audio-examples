#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define WAV_PATH "PATH/TO/AUDIO.wav"

// Play a wav audio file using SDL_mixer

int main(int argc, char* argv[]){
    printf("Playing %s\n", WAV_PATH);

    // Init SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL init error: %s\n", SDL_GetError());
        return 1;
    }

    // Init the mixer api
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL mix error: %s\n", Mix_GetError());
        return 1;
    }

    // Load the wav
    Mix_Music *audio = Mix_LoadMUS(WAV_PATH);
    if(!audio) {
        printf("SDL mix error: %s\n", Mix_GetError());
        return 1;
    }

    // Start playing audio
    if (Mix_PlayMusic(audio, 1) < 0) {
        printf("Mix_PlayMusic error: %s\n", Mix_GetError());
        return 1;
    }

    // Wait for the audio to finish
    while (Mix_PlayingMusic())
        SDL_Delay(100);

    // Clean up
    Mix_FreeMusic(audio);
    Mix_Quit();
    SDL_Quit();
}

