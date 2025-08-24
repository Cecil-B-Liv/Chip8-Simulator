#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <chip8.h>

const char* filename = "roms/1-chip8-logo.ch8";

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    Chip8 chip8;
    chip8_init(&chip8);
    printf("Rom size = %ld\n", getRomSize(filename));
    romLoaderHasMaloc(&chip8, filename);
    romLoaderNoMaloc(&chip8, filename);

    return 0;
}
