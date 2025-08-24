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
    romLoaderNoMaloc(&chip8, filename);

    debug_dump_memory(chip8.memory, 0x200, getRomSize(filename));
    chip8Cycle(&chip8);
    return 0;
}

void chip8Cycle(Chip8* chip8) {
    // Fetch
    uint16_t opcode = chip8->memory[chip8->pc << 8] | chip8->memory[chip8->pc + 1];
    printf("Fetched opcode: %04X at PC: %03X\n", opcode, chip8->pc);
    // Default PC advance
    chip8->pc += 2;
    // get the useful fields (nnn, kk, n, x, y)
    uint16_t nnn = opcode & 0x0FFF;  // get the last 3 byte
    uint8_t kk = opcode & 0x00FF;
    uint8_t n = opcode & 0x000F;
    uint8_t x = opcode & 0x0F00 >> 8;
    uint8_t y = opcode & 0x00F0 >> 4;

    // Decode
    switch (opcode & 0xF000) {
        // ---------------- Stage 1: Basics ----------------
        case 0x0000:
            switch (opcode) {
                case 0x00E0:  // CLS ⭐
                    printf("CLS (clear screen)\n");
                    break;
                case 0x00EE:  // RET ⭐
                    printf("RET (return from subroutine)\n");
                    break;
                default:  // 0NNN (SYS addr) ⭐⭐ (legacy, usually ignored)
                    printf("SYS %03X (ignored)\n", opcode & 0x0FFF);
                    break;
            }
            break;

        case 0x1000:  // JP addr ⭐
            printf("JP %03X\n", nnn);
            break;

        case 0x2000:  // CALL addr ⭐⭐
            printf("CALL %03X\n", nnn);
            break;

        case 0x6000:  // LD Vx, byte ⭐
            printf("LD V%X, %02X\n", x, kk);
            break;

        case 0x7000:  // ADD Vx, byte ⭐
            printf("ADD V%X, %02X\n", x, kk);
            break;

        // ---------------- Stage 2: Skips ----------------
        case 0x3000:  // SE Vx, byte ⭐
            printf("SE V%X, %02X\n", x, kk);
            break;

        case 0x4000:  // SNE Vx, byte ⭐
            printf("SNE V%X, %02X\n", x, kk);
            break;

        case 0x5000:  // SE Vx, Vy ⭐
            if (n == 0)
                printf("SE V%X, V%X\n", x, y);
            else
                printf("Unknown opcode: %04X\n", opcode);
            break;

        case 0x9000:  // SNE Vx, Vy ⭐
            if (n == 0)
                printf("SNE V%X, V%X\n", x, y);
            else
                printf("Unknown opcode: %04X\n", opcode);
            break;

        // ---------------- Stage 3: Arithmetic & Logic ----------------
        case 0x8000:
            switch (n) {
                case 0x0:
                    printf("LD V%X, V%X\n", x, y);
                    break;  // ⭐
                case 0x1:
                    printf("OR V%X, V%X\n", x, y);
                    break;  // ⭐
                case 0x2:
                    printf("AND V%X, V%X\n", x, y);
                    break;  // ⭐
                case 0x3:
                    printf("XOR V%X, V%X\n", x, y);
                    break;  // ⭐
                case 0x4:
                    printf("ADD V%X, V%X (with carry)\n", x, y);
                    break;  // ⭐⭐
                case 0x5:
                    printf("SUB V%X, V%X\n", x, y);
                    break;  // ⭐⭐
                case 0x6:
                    printf("SHR V%X\n", x);
                    break;  // ⭐⭐ (quirk)
                case 0x7:
                    printf("SUBN V%X, V%X\n", x, y);
                    break;  // ⭐⭐
                case 0xE:
                    printf("SHL V%X\n", x);
                    break;  // ⭐⭐ (quirk)
                default:
                    printf("Unknown opcode: %04X\n", opcode);
                    break;
            }
            break;

        // ---------------- Stage 4: Index/Jumps/Random ----------------
        case 0xA000:  // LD I, addr ⭐
            printf("LD I, %03X\n", nnn);
            break;

        case 0xB000:  // JP V0, addr ⭐⭐
            printf("JP V0, %03X\n", nnn);
            break;

        case 0xC000:  // RND Vx, byte ⭐⭐
            printf("RND V%X, %02X\n", x, kk);
            break;

        // ---------------- Stage 5: Graphics ----------------
        case 0xD000:  // DRW Vx, Vy, nibble ⭐⭐⭐
            printf("DRW V%X, V%X, %X\n", x, y, n);
            break;

        // ---------------- Stage 6: Input ----------------
        case 0xE000:
            switch (kk) {
                case 0x9E:
                    printf("SKP V%X\n", x);
                    break;  // ⭐⭐
                case 0xA1:
                    printf("SKNP V%X\n", x);
                    break;  // ⭐⭐
                default:
                    printf("Unknown opcode: %04X\n", opcode);
                    break;
            }
            break;

        // ---------------- Stage 7: Timers & Memory ----------------
        case 0xF000:
            switch (kk) {
                case 0x07:
                    printf("LD V%X, DT\n", x);
                    break;  // ⭐
                case 0x0A:
                    printf("LD V%X, K (wait key)\n", x);
                    break;  // ⭐⭐
                case 0x15:
                    printf("LD DT, V%X\n", x);
                    break;  // ⭐
                case 0x18:
                    printf("LD ST, V%X\n", x);
                    break;  // ⭐
                case 0x1E:
                    printf("ADD I, V%X\n", x);
                    break;  // ⭐⭐
                case 0x29:
                    printf("LD F, V%X (digit sprite)\n", x);
                    break;  // ⭐⭐
                case 0x33:
                    printf("LD B, V%X (BCD)\n", x);
                    break;  // ⭐⭐⭐
                case 0x55:
                    printf("LD [I], V0..V%X\n", x);
                    break;  // ⭐⭐
                case 0x65:
                    printf("LD V0..V%X, [I]\n", x);
                    break;  // ⭐⭐
                default:
                    printf("Unknown opcode: %04X\n", opcode);
                    break;
            }
            break;

        default:
            printf("Unknown opcode: %04X\n", opcode);
            break;
    }

    // divide op code as 4 nibbles (4 bits)
    //[op][x][y][n]

    // Execute
}
