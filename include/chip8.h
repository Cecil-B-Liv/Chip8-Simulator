#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define MEM_SIZE 4096
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define STACK_SIZE 16
#define KEYPAD_SIZE 16
#define FONTSET_SIZE 80

// === CHIP-8 State ===
typedef struct {
    uint8_t memory[MEM_SIZE];                         // 4KB Memory
    uint8_t V[16];                                    // 16 8-bits Registers V0-VF
    uint16_t index;                                   // Index Register (16 bit)
    uint16_t pc;                                      // Program Counter (starts at 0x200)
    uint16_t stack[STACK_SIZE];                       // Call stack
    uint8_t sp;                                       // Stack Pointer
    uint8_t delay_timer;                              // Delay Timer (8 bit timer)
    uint8_t sound_timer;                              // Sound Timer (8 bit timer)
    uint8_t display[DISPLAY_WIDTH * DISPLAY_HEIGHT];  // Display (64x32 pixels)
    uint8_t keypad[KEYPAD_SIZE];                      // Input (16 keys)
} Chip8;

void chip8_init(Chip8* chip8) {
    memset(chip8->memory, 0, MEM_SIZE * sizeof(chip8->memory[0]));
    memset(chip8->V, 0, 16);
    chip8->index = 0;
    chip8->pc = 0x200;
    memset(chip8->stack, 0, STACK_SIZE * sizeof(chip8->stack[0]));
    chip8->sp = 0;
    chip8->delay_timer = 0;
    chip8->sound_timer = 0;
    memset(chip8->display, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(chip8->display[0]));
    memset(chip8->keypad, 0, KEYPAD_SIZE * sizeof(chip8->keypad[0]));

    static const uint8_t chip8_fontset[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
        0x20, 0x60, 0x20, 0x20, 0x70,  // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
        0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
        0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
        0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
        0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
        0xF0, 0x80, 0xF0, 0x80, 0x80   // F
    };

    memcpy(&chip8->memory[0x50], chip8_fontset, sizeof(chip8_fontset));
}

long getRomSize(const char* filename) {
    FILE* rom = fopen(filename, "rb");
    if (!rom) {
        perror("Failed to open ROM");
        return -1;
    }
    fseek(rom, 0, SEEK_END);
    long size = ftell(rom);
    return size;
}

int romLoaderHasMaloc(Chip8* chip8, const char* filename) {
    FILE* rom = fopen(filename, "rb");
    if (!rom) {
        perror("Failed to open ROM");
        return -1;
    }

    fseek(rom, 0, SEEK_END);
    size_t size = ftell(rom);
    fseek(rom, 0, SEEK_SET);

    // Make sure it fits into CHIP-8 memory (size + 0x200 <= 4096)
    if (size > (4096 - 0x200)) {
        fprintf(stderr, "ROM too large to fit in memory.\n");
        fclose(rom);
        return -1;
    }

    unsigned char* buffer = malloc(size);
    if (!buffer) {
        perror("Failed to allocate buffer");
        fclose(rom);
        return -1;
    }

    size_t bytesRead = fread(buffer, 1, size, rom);
    if (bytesRead != size) {
        perror("Failed to read ROM completely");
        free(buffer);
        fclose(rom);
        return -1;
    }

    // Load into memory starting at 0x200
    for (size_t i = 0; i < size; i++) {
        chip8->memory[0x200 + i] = buffer[i];
    }

    free(buffer);
    fclose(rom);
    printf("Load successfully.\n");
    return 1;
};

int romLoaderNoMaloc(Chip8* chip8, const char* filename) {
    FILE* rom = fopen(filename, "rb");
    if (!rom) {
        perror("Failed to open ROM");
        return -1;
    }
    fseek(rom, 0, SEEK_END);
    size_t size = ftell(rom);
    fseek(rom, 0, SEEK_SET);

    // Make sure it fits into CHIP-8 memory (size + 0x200 <= 4096)
    if (size > (4096 - 0x200)) {
        fprintf(stderr, "ROM too large to fit in memory.\n");
        fclose(rom);
        return -1;
    }

    // Read ROM directly into CHIP-8 memory
    size_t bytesRead = fread(&chip8->memory[0x200], 1, size, rom);
    if (bytesRead != size) {
        perror("Failed to read ROM completely");
        fclose(rom);
        return -1;
    }

    fclose(rom);
    printf("Load successfully.\n");
    return 1;
};