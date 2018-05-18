#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>

// Manually mix two sine waves together

#define SECONDS 6
#define CHANNELS 2
#define PI 3.141592

typedef struct {
    int size;
    int play_cursor;
    Sint16 *buffer;
} Audio_Data;

void audio_callback(void *userdata, Uint8 *stream, int len);

void gen_wav(Audio_Data *audio_data, int tone_hz, Uint16 tone_volume, int samples_per_second);

int main(int argc, char* argv[]){
    printf("Playing a sine wave octave.\n");

    // Init SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL init error: %s\n", SDL_GetError());
        return 1;
    }

    int samples_per_second = 44000;
    Uint16 tone_volume = 5000/2; // Half the volumn so we can safely add (mix) the waves

    int tone_hz_a = 440;
    Audio_Data sine_a;
    gen_wav(&sine_a, tone_hz_a, tone_volume, samples_per_second);

    int tone_hz_b = 220;
    Audio_Data sine_b;
    gen_wav(&sine_b, tone_hz_b, tone_volume, samples_per_second);

    // Mix the sine waves together by adding them
    int elements = sine_a.size / sizeof(Uint16);
    for (int i = 0; i < elements; i++)
        sine_a.buffer[i] += sine_b.buffer[i];

    SDL_AudioSpec wanted_spec;
    SDL_AudioSpec obtained_spec;

    wanted_spec.freq = samples_per_second;
    wanted_spec.format = AUDIO_S16;
    wanted_spec.channels = CHANNELS;
    wanted_spec.samples = 4096; // SDL buffer sample size (same used for wav files)
    wanted_spec.callback = audio_callback;
    wanted_spec.userdata = &sine_a;

    // Open the audio device
    int iscapture = 0; // We are not opening the device for recording so iscapture = 0
    int allowed_changes = 0; // See docs for info on what this is for
    const char* device_name = SDL_GetAudioDeviceName(0, iscapture); // Gets the zero'th non-capture device name (see docs for more info)
    SDL_AudioDeviceID device = SDL_OpenAudioDevice(device_name, iscapture, &wanted_spec, &obtained_spec, allowed_changes);
    if (device == 0) {
        printf("SDL_OpenAudioDevice error: %s\n", SDL_GetError());
        return 1;
    }

    // Start playing
    SDL_PauseAudioDevice(device, 0);

    // Wait for SECONDS number of seconds
    SDL_Delay(SECONDS * 1000);

    // Shut everything down
    SDL_CloseAudioDevice(device);
    free(sine_a.buffer);
    free(sine_b.buffer);
    SDL_Quit();

}

// This is our custom Audio callback function, SDL will call this function when
// it is ready for more data (audio) to be output from the audio device
void audio_callback(void *userdata, Uint8 *stream, int len) {

    // Cast the userdata to Audio_Data so we can use it
    Audio_Data *audio_data = (Audio_Data *) userdata;

    // Return if SDL asks for data past the end of the buffer
    if (audio_data->play_cursor + len > audio_data->size) return;

    memcpy(stream, ((Uint8 *) audio_data->buffer + audio_data->play_cursor), len);
    audio_data->play_cursor += len;
}

// Generate a sine wave and fill out the Audio_Data struct
void gen_wav(Audio_Data *audio_data, int tone_hz, Uint16 tone_volume, int samples_per_second) {

    int bytes_per_sample = sizeof(Uint16) * CHANNELS;

    int bytes_to_write = samples_per_second * bytes_per_sample * SECONDS;
    void *sound_buffer = malloc(bytes_to_write);
    Sint16 *sample_write = (Sint16 *)sound_buffer;

    audio_data->size = bytes_to_write;
    audio_data->play_cursor = 0;
    audio_data->buffer = sample_write;

    int sine_wave_period = samples_per_second / tone_hz;
    int sample_count = bytes_to_write/bytes_per_sample;
    Sint16 sample_value;

    for (int sample_index = 0; sample_index < sample_count; sample_index++) {

        // Create the sine wav
        double two_pi = 2.0 * PI;
        double t = (double)sample_index / (double)samples_per_second;
        double sval = tone_volume * sin(two_pi * (double)tone_hz * t);
        sample_value = (Sint16) sval;

        // Write the sample_value to the buffer for each channel
        for (int channel = 0; channel < CHANNELS; channel++) {
            *sample_write++ = sample_value;
        }
    }
}

