#include <header.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Audio configuration
#define SAMPLE_RATE 44100
#define FREQUENCY 800   // 800Hz beep
#define AMPLITUDE 3000  // Volume level

// Audio state for generating square wave
typedef struct {
    float phase;
    bool beeping;
} AudioData;

static AudioData audio_data = {0.0f, false};

// SDL3 Audio callback - different signature!
void audio_callback(void* userdata,
                    SDL_AudioStream* stream,
                    int additional_amount,
                    int total_amount) {
    (void)userdata;
    (void)additional_amount;

    // Calculate how many samples we need
    int samples_needed = total_amount / sizeof(float);
    float buffer[1024];  // Temporary buffer

    // Limit to buffer size
    if (samples_needed > 1024) samples_needed = 1024;

    for (int i = 0; i < samples_needed; i++) {
        if (audio_data.beeping) {
            // Generate square wave
            buffer[i] = (sinf(audio_data.phase) > 0) ? AMPLITUDE : -AMPLITUDE;
            audio_data.phase += (2.0f * M_PI * FREQUENCY) / SAMPLE_RATE;
            if (audio_data.phase >= 2.0f * M_PI) {
                audio_data.phase -= 2.0f * M_PI;
            }
        } else {
            buffer[i] = 0.0f;  // Silence
        }
    }

    // Put data into the stream
    SDL_PutAudioStreamData(stream, buffer, samples_needed * sizeof(float));
}

bool platform_init(Platform* p,
                   const char* title,
                   int windowWidth,
                   int windowHeight,
                   int textureWidth,
                   int textureHeight) {
    // Initialize SDL with video and audio
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }

    p->windowWidth = windowWidth;
    p->windowHeight = windowHeight;
    p->textureWidth = textureWidth;
    p->textureHeight = textureHeight;

    p->window = SDL_CreateWindow(title, windowWidth, windowHeight, SDL_WINDOW_RESIZABLE);

    p->renderer = SDL_CreateRenderer(p->window, NULL);
    p->texture = SDL_CreateTexture(p->renderer,
                                   SDL_PIXELFORMAT_RGBA8888,
                                   SDL_TEXTUREACCESS_STREAMING,
                                   textureWidth,
                                   textureHeight);

    SDL_SetTextureScaleMode(p->texture, SDL_SCALEMODE_NEAREST);

    SDL_AudioSpec spec;
    spec.freq = SAMPLE_RATE;
    spec.format = SDL_AUDIO_F32;  // 32-bit float
    spec.channels = 1;            // Mono

    // Open audio device
    p->audio_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);

    if (p->audio_device == 0) {
        printf("Failed to open audio device: %s\n", SDL_GetError());
        p->audio_enabled = false;
    } else {
        // Create audio stream
        SDL_AudioStream* stream = SDL_CreateAudioStream(&spec, &spec);
        if (stream == NULL) {
            printf("Failed to create audio stream: %s\n", SDL_GetError());
            SDL_CloseAudioDevice(p->audio_device);
            p->audio_enabled = false;
        } else {
            // Set the callback
            if (!SDL_SetAudioStreamGetCallback(stream, audio_callback, NULL)) {
                printf("Failed to set audio callback: %s\n", SDL_GetError());
                SDL_DestroyAudioStream(stream);
                SDL_CloseAudioDevice(p->audio_device);
                p->audio_enabled = false;
            } else {
                // Bind stream to device and start
                if (!SDL_BindAudioStream(p->audio_device, stream)) {
                    printf("Failed to bind audio stream: %s\n", SDL_GetError());
                    SDL_DestroyAudioStream(stream);
                    SDL_CloseAudioDevice(p->audio_device);
                    p->audio_enabled = false;
                } else {
                    // Resume audio device (start playing)
                    if (!SDL_ResumeAudioDevice(p->audio_device)) {
                        printf("Failed to resume audio device: %s\n", SDL_GetError());
                    }
                    p->audio_enabled = true;
                    printf("Audio initialized successfully!\n");
                }
            }
        }
    }

    return true;
}

void platform_start_beep(Platform* p) {
    if (p->audio_enabled && !audio_data.beeping) {
        printf("Starting beep\n");  // Debug
        audio_data.beeping = true;
        audio_data.phase = 0.0f;  // Reset phase
    }
}

void platform_stop_beep(Platform* p) {
    if (p->audio_enabled && audio_data.beeping) {
        printf("Stopping beep\n");  // Debug
        audio_data.beeping = false;
    }
}

void platform_update(Platform* p, const void* buffer, int pitch) {
    SDL_SetTextureScaleMode(p->texture, SDL_SCALEMODE_NEAREST);
    SDL_UpdateTexture(p->texture, NULL, buffer, pitch);
    SDL_RenderClear(p->renderer);
    SDL_RenderTexture(p->renderer, p->texture, NULL, NULL);
    SDL_RenderPresent(p->renderer);
}

void platform_destroy(Platform* p) {
    if (p->texture) SDL_DestroyTexture(p->texture);
    if (p->renderer) SDL_DestroyRenderer(p->renderer);
    if (p->window) SDL_DestroyWindow(p->window);
    if (p->audio_enabled) {
        SDL_CloseAudioDevice(p->audio_device);
    }
    SDL_Quit();
}

bool platform_processInput(uint8_t* keys) {
    SDL_Event event;
    bool quit = false;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                quit = true;
                break;
            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP: {
                bool isDown = (event.type == SDL_EVENT_KEY_DOWN);
                switch (event.key.key) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;

                    // CHIP-8 Keypad Layout (Standard):
                    // 1 2 3 C
                    // 4 5 6 D
                    // 7 8 9 E
                    // A 0 B F

                    // Top row: 1 2 3 C
                    case SDLK_1:
                        keys[0x1] = isDown;
                        break;
                    case SDLK_2:
                        keys[0x2] = isDown;
                        break;
                    case SDLK_3:
                        keys[0x3] = isDown;
                        break;
                    case SDLK_4:
                        keys[0xC] = isDown;
                        break;

                    // Second row: 4 5 6 D
                    case SDLK_Q:
                        keys[0x4] = isDown;
                        break;
                    case SDLK_W:
                        keys[0x5] = isDown;
                        break;
                    case SDLK_E:
                        keys[0x6] = isDown;
                        break;
                    case SDLK_R:
                        keys[0xD] = isDown;
                        break;

                    // Third row: 7 8 9 E
                    case SDLK_A:
                        keys[0x7] = isDown;
                        break;
                    case SDLK_S:
                        keys[0x8] = isDown;
                        break;
                    case SDLK_D:
                        keys[0x9] = isDown;
                        break;
                    case SDLK_F:
                        keys[0xE] = isDown;
                        break;

                    // Bottom row: A 0 B F
                    case SDLK_Z:
                        keys[0xA] = isDown;
                        break;
                    case SDLK_X:
                        keys[0x0] = isDown;  // This is correct
                        break;
                    case SDLK_C:
                        keys[0xB] = isDown;
                        break;
                    case SDLK_V:
                        keys[0xF] = isDown;
                        break;

                    default:
                        break;
                }
                break;
            }
            default:
                break;
        }
    }
    return quit;
}