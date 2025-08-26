#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <chip8.h>
#include <platform.h>
#include <testRom.h>

#define CHIP8_HZ 500  // 500 Hz = 2 ms per cycle
#define CYCLE_DELAY (1000 / CHIP8_HZ)
const char* filename = "roms/4-flags.ch8";

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

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

    uint32_t lastCycleTime = SDL_GetTicks();  // milliseconds
    bool quit = false;
    int videoPitch = sizeof(chip8.display[0]) * DISPLAY_WIDTH;

    while (!quit) {
        quit = platform_processInput(chip8.keypad);

        uint32_t currentTime = SDL_GetTicks();
        uint32_t dt = currentTime - lastCycleTime;

        if (dt > CYCLE_DELAY) {  // count to 2ms
            lastCycleTime = currentTime;
            chip8Cycle(&chip8);

            platform_update(&platform, chip8.display, videoPitch);
        }
    }
    return 0;
}

// Keypad:      PC Key:
// 1 2 3 C      1 2 3 4
// 4 5 6 D      Q W E R
// 7 8 9 E      A S D F
// A 0 B F      Z X C V

// FOR TESTING
// romLoaderTest(&chip8, testRomStage1, sizeof(testRomStage1));
// debug_dump_memory(chip8.memory, 0x200, 32);
// debug_dump_memory(chip8.memory, 0x200, getRomSize(filename));

