#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <header.h>

#define CHIP8_HZ 500  // 500 Hz = 2 ms per cycle
#define CYCLE_DELAY (1000 / CHIP8_HZ)

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    const char* filename = "roms/4-flags.ch8";

    Platform platform;
    platform_init(&platform,
                  "CHIP-8 Emulator",
                  DISPLAY_WIDTH * 10,
                  DISPLAY_HEIGHT * 10,
                  DISPLAY_WIDTH,
                  DISPLAY_HEIGHT);

    Chip8 chip8;
    chip8_init(&chip8);
    romLoaderNoMaloc(&chip8, filename);

    uint32_t lastCycleTime = SDL_GetTicks();
    bool quit = false;
    int videoPitch = sizeof(chip8.display[0]) * DISPLAY_WIDTH;

    while (!quit) {
        quit = platform_processInput(chip8.keypad);

        uint32_t currentTime = SDL_GetTicks();
        uint32_t dt = currentTime - lastCycleTime;

        if (dt > CYCLE_DELAY) {
            lastCycleTime = currentTime;
            chip8Cycle(&chip8);
            platform_update(&platform, chip8.display, videoPitch);
        }
    }

    platform_destroy(&platform);
    return 0;
}