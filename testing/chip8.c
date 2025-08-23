#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
#include <SDL3/SDL.h>

#define MEM_SIZE 4096
#define GFX_WIDTH 64
#define GFX_HEIGHT 32
#define STACK_SIZE 16
#define KEYPAD_SIZE 16
#define FONTSET_SIZE 80

// // === CHIP-8 State ===
// typedef struct {
//     uint8_t memory[MEM_SIZE];         // 4KB Memory
//     uint8_t V[16];                    // 16 Registers V0-VF
//     uint16_t I;                       // Index Register
//     uint16_t pc;                      // Program Counter
//     uint16_t stack[STACK_SIZE];       // Call stack
//     uint8_t sp;                       // Stack Pointer
//     uint8_t delay_timer;              // Delay Timer
//     uint8_t sound_timer;              // Sound Timer
//     uint8_t gfx[GFX_WIDTH * GFX_HEIGHT]; // Display (64x32 pixels)
//     uint8_t keypad[KEYPAD_SIZE];      // Input (16 keys)
// } Chip8;

int main(){
    printf("Hello World");

    

    return 0;
}