#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>

#define MEM_SIZE 4096
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define STACK_SIZE 16
#define KEYPAD_SIZE 16
#define FONTSET_SIZE 80
#define FONTSET_START_ADDRESS 0x50

// === CHIP-8 State ===
typedef struct {
    uint8_t memory[MEM_SIZE];                          // 4KB Memory
    uint8_t V[16];                                     // 16 8-bits Registers V0-VF
    uint16_t index;                                    // Index Register (16 bit)
    uint16_t pc;                                       // Program Counter (starts at 0x200)
    uint16_t stack[STACK_SIZE];                        // Call stack
    uint8_t sp;                                        // Stack Pointer
    uint8_t delay_timer;                               // Delay Timer (8 bit timer)
    uint8_t sound_timer;                               // Sound Timer (8 bit timer)
    uint32_t display[DISPLAY_WIDTH * DISPLAY_HEIGHT];  // Display (64x32 pixels)
    uint8_t keypad[KEYPAD_SIZE];                       // Input (16 keys)
} Chip8;

// Function declarations
void chip8_init(Chip8* chip8);
void chip8_screen_init(void);
void dumpDisplay(Chip8* chip8);
long getRomSize(const char* filename);
int romLoaderHasMaloc(Chip8* chip8, const char* filename);
int romLoaderNoMaloc(Chip8* chip8, const char* filename);
int romLoaderTest(Chip8* chip8, const uint8_t* rom, size_t romSize);
void debug_dump_memory(const uint8_t* memory, size_t start, size_t length);
void dumpRegisters(Chip8* chip8);

#endif // CHIP8_H