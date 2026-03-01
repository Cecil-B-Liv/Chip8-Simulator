#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdint.h>

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

// Platform structure
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    int windowWidth, windowHeight;
    int textureWidth, textureHeight;

    SDL_AudioDeviceID audio_device;
    bool audio_enabled;
} Platform;

// Function declarations
bool platform_init(Platform* p,
                   const char* title,
                   int windowWidth,
                   int windowHeight,
                   int textureWidth,
                   int textureHeight);

void platform_update(Platform* p, const void* buffer, int pitch);
void platform_destroy(Platform* p);
bool platform_processInput(uint8_t* keys);

void platform_start_beep(Platform* p);
void platform_stop_beep(Platform* p);

#endif  // PLATFORM_H