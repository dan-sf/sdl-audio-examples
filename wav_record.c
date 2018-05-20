#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

// Record some audio, then play it back

#define SECONDS 6
#define CHANNELS 2

typedef struct {
    int size;
    int play_cursor;
    enum Buffer_Type {
        PLAY,
        RECORD
    } buffer_type;
    Sint16 *buffer;
} Audio_Data;

void audio_callback(void *userdata, Uint8 *stream, int len);

int main(int argc, char* argv[]){
    printf("Start recording audio\n");

    // Init SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL init error: %s\n", SDL_GetError());
        return 1;
    }

    int samples_per_second = 44100;
    int bytes_per_sample = sizeof(Uint16) * CHANNELS;
    int bytes_to_write = samples_per_second * bytes_per_sample * SECONDS;

    void *record_buffer = malloc(bytes_to_write);
    Sint16 *record_write = (Sint16 *)record_buffer;

    Audio_Data record;
    record.size = bytes_to_write;
    record.play_cursor = 0;
    record.buffer_type = RECORD;
    record.buffer = record_write;

    SDL_AudioSpec record_wanted_spec;
    SDL_AudioSpec record_obtained_spec;

    record_wanted_spec.freq = samples_per_second;
    record_wanted_spec.format = AUDIO_S16;
    record_wanted_spec.channels = CHANNELS;
    record_wanted_spec.samples = 4096; // SDL buffer sample size (same used for wav files)
    record_wanted_spec.callback = audio_callback;
    record_wanted_spec.userdata = &record;

    // Open the audio device
    int iscapture = 1; // We are opening the device for recording so iscapture = 1
    int allowed_changes = 0; // See docs for info on what this is for
    const char* record_device_name = SDL_GetAudioDeviceName(0, iscapture); // Gets the zero'th capture device name (see docs for more info)
    SDL_AudioDeviceID record_device = SDL_OpenAudioDevice(record_device_name, iscapture, &record_wanted_spec, &record_obtained_spec, allowed_changes);
    if (record_device == 0) {
        printf("SDL_OpenAudioDevice error: %s\n", SDL_GetError());
        return 1;
    }

    // Start recording
    SDL_PauseAudioDevice(record_device, 0);

    // Wait for SECONDS number of seconds
    SDL_Delay(SECONDS * 1000);

    // Stop recording
    SDL_PauseAudioDevice(record_device, 1);
    printf("Recording stopped\n");

    Audio_Data play;
    void *play_buffer = malloc(bytes_to_write);
    Sint16 *play_write = (Sint16 *)play_buffer;
    memcpy(play_buffer, record_buffer, bytes_to_write);

    play.size = bytes_to_write;
    play.play_cursor = 0;
    play.buffer_type = PLAY;
    play.buffer = play_write;

    SDL_AudioSpec play_wanted_spec;
    SDL_AudioSpec play_obtained_spec;
    play_wanted_spec = record_wanted_spec; // play and record specs are the same but just point to different userdata
    play_wanted_spec.userdata = &play;

    // Open the audio device for playing
    iscapture = 0; // We are not opening the device for recording so iscapture = 0
    const char* play_device_name = SDL_GetAudioDeviceName(0, iscapture); // Gets the zero'th non-capture device name (see docs for more info)
    SDL_AudioDeviceID play_device = SDL_OpenAudioDevice(play_device_name, iscapture, &play_wanted_spec, &play_obtained_spec, allowed_changes);
    if (play_device == 0) {
        printf("SDL_OpenAudioDevice error: %s\n", SDL_GetError());
        return 1;
    }

    // Start playing
    printf("Start Playing recorded audio\n");
    SDL_PauseAudioDevice(play_device, 0);

    // Wait for SECONDS number of seconds
    SDL_Delay(SECONDS * 1000);

    // Shut everything down
    SDL_CloseAudioDevice(record_device);
    SDL_CloseAudioDevice(play_device);
    free(record.buffer);
    free(play.buffer);
    SDL_Quit();

}

// This is our custom Audio callback function, SDL will call this function when
// it is ready for more data (audio) in or out of the audio device
void audio_callback(void *userdata, Uint8 *stream, int len) {

    // Cast the userdata to Audio_Data so we can use it
    Audio_Data *audio_data = (Audio_Data *) userdata;

    // Return if SDL asks for data past the end of the buffer
    if (audio_data->play_cursor + len > audio_data->size) return;

    // Copy data from the stream on record and copy data to the stream on play
    if (audio_data->buffer_type == RECORD)
        memcpy(((Uint8 *) audio_data->buffer + audio_data->play_cursor), stream, len);
    else if (audio_data->buffer_type == PLAY)
        memcpy(stream, ((Uint8 *) audio_data->buffer + audio_data->play_cursor), len);

    audio_data->play_cursor += len;
}

