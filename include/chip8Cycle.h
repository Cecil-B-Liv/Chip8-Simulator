#include <header.h>

void chip8Cycle(Chip8* chip8) {       // Execute one cycle of CHIP-8
    if (chip8->pc >= MEM_SIZE - 2) {  // -2 because we read 2 bytes for opcode
        printf("PC out of bounds! %04X\n", chip8->pc);
        exit(1);
    }

    // Fetch opcode (2 bytes)
    // Combine the two bytes into a single opcode
    uint16_t opcode = chip8->memory[chip8->pc] << 8 | chip8->memory[chip8->pc + 1];
    printf("PC: %04X  OPCODE: %04X\n", chip8->pc, opcode);
    chip8->pc += 2;  // Default PC advance

    // get the useful fields (nnn, kk, n, x, y)
    uint16_t nnn = opcode & 0x0FFF;      // Last 3 bytes
    uint8_t kk = opcode & 0x00FF;        // Last 2 bytes
    uint8_t n = opcode & 0x000F;         // Last 1 bytes
    uint8_t x = (opcode & 0x0F00) >> 8;  // Second byte
    uint8_t y = (opcode & 0x00F0) >> 4;  // Third byte

    // Decode
    switch (opcode & 0xF000) {
        // ---------------- Stage 1: Basics ----------------
        case 0x0000:
            switch (opcode) {
                case 0x00E0:  // CLS
                    printf("CLS (clear screen)\n");
                    memset(chip8->display,  // just clear the value in display
                           0,
                           DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(chip8->display[0]));
                    break;
                case 0x00EE:  // RET
                    printf("RET (return from subroutine)\n");
                    --chip8->sp;                          // pop from stack
                    chip8->pc = chip8->stack[chip8->sp];  // give the address back to the pc
                    break;
                default:  // 0NNN (SYS addr)  (legacy, usually ignored)
                    printf("SYS %03X (ignored)\n", nnn);
                    break;
            }
            break;

        case 0x1000:  // JP addr
            printf("JP %03X\n", nnn);
            chip8->pc = nnn;  // go to the nnn directly
            break;

        case 0x2000:  // CALL addr
            printf("CALL %03X\n", nnn);
            chip8->stack[chip8->sp] =
                chip8->pc;  // go to the nnn and save the returning address to the stack
            ++chip8->sp;    // to avoid overwrite on the line above
            chip8->pc = nnn;
            break;

        case 0x6000:  // LD Vx, byte
            printf("LD V%X, %02X\n", x, kk);
            chip8->V[x] = kk;  // write to the V register
            break;

        case 0x7000:  // ADD Vx, byte
            printf("ADD V%X, %02X\n", x, kk);
            chip8->V[x] += kk;  // add to the V register
            break;

        // // ---------------- Stage 2: Skips ----------------
        case 0x3000:  // SE Vx, byte
            printf("SE V%X, %02X\n", x, kk);
            if (chip8->V[x] == kk) {
                chip8->pc += 2;
            }
            break;

        case 0x4000:  // SNE Vx, byte
            printf("SNE V%X, %02X\n", x, kk);
            if (chip8->V[x] != kk) {
                chip8->pc += 2;
            }
            break;

        case 0x5000:  // SE Vx, Vy
            if (n == 0) {
                printf("SE V%X, V%X\n", x, y);
                if (chip8->V[x] == chip8->V[y]) {
                    chip8->pc += 2;
                }
            } else {
                printf("Unknown opcode: %04X\n", opcode);
            }

            break;

        case 0x9000:  // SNE Vx, Vy
            if (n == 0) {
                printf("SNE V%X, V%X\n", x, y);
                if (chip8->V[x] != chip8->V[y]) {
                    chip8->pc += 2;
                }
            } else
                printf("Unknown opcode: %04X\n", opcode);
            break;

        // // ---------------- Stage 3: Arithmetic & Logic ----------------
        case 0x8000:
            switch (n) {
                case 0x0:
                    printf("LD V%X, V%X\n", x, y);
                    chip8->V[x] = chip8->V[y];
                    break;  // LD Vx, Vy
                case 0x1:
                    printf("OR V%X, V%X\n", x, y);
                    chip8->V[x] |= chip8->V[y];
                    break;  // OR Vx, Vy
                case 0x2:
                    printf("AND V%X, V%X\n", x, y);
                    chip8->V[x] &= chip8->V[y];
                    break;  // AND Vx, Vy
                case 0x3:
                    printf("XOR V%X, V%X\n", x, y);
                    chip8->V[x] ^= chip8->V[y];
                    break;  // XOR Vx, Vy
                case 0x4:
                    printf("ADD V%X, V%X (with carry)\n", x, y);
                    uint16_t sum = chip8->V[x] + chip8->V[y];
                    if (sum > 255U) {
                        chip8->V[0xF] = 1;
                    } else {
                        chip8->V[0xF] = 0;
                    }
                    chip8->V[x] = sum & 0x00FF;
                    break;  // ADD Vx, Vy (with carry)
                case 0x5:
                    printf("SUB V%X, V%X\n", x, y);
                    if (chip8->V[x] > chip8->V[y]) {
                        chip8->V[0xF] = 1;
                    } else {
                        chip8->V[0xF] = 0;
                    }
                    chip8->V[x] -= chip8->V[y];
                    break;  // SUB Vx, Vy
                case 0x6:
                    printf("SHR V%X\n", x);
                    chip8->V[0xF] = (chip8->V[x] & 0x1u);
                    chip8->V[x] >>= 1;
                    break;  //  (quirk) // SHR Vx
                case 0x7:
                    printf("SUBN V%X, V%X\n", x, y);
                    if (chip8->V[y] > chip8->V[x]) {
                        chip8->V[0xF] = 1;
                    } else {
                        chip8->V[0xF] = 0;
                    }
                    chip8->V[x] = chip8->V[y] - chip8->V[x];
                    break;  // SUBN Vx, Vy
                case 0xE:
                    printf("SHL V%X\n", x);
                    chip8->V[0xF] = (chip8->V[x] & 0x80u) >> 7u;
                    chip8->V[x] <<= 1;
                    break;  //  (quirk) // SHL Vx
                default:
                    printf("Unknown opcode: %04X\n", opcode);
                    break;
            }
            break;

        // // ---------------- Stage 4: Index/Jumps/Random ----------------
        case 0xA000:  // LD I, addr
            printf("LD I, %03X\n", nnn);
            chip8->index = nnn;
            break;

        case 0xB000:  // JP V0, addr
            printf("JP V0, %03X\n", nnn);
            chip8->pc = chip8->V[0] + nnn;
            break;

        case 0xC000:  // RND Vx, byte
            printf("RND V%X, %02X\n", x, kk);
            uint8_t random_byte = rand() % 256;  // random 0-255
            chip8->V[x] = random_byte & kk;
            break;

        // ---------------- Stage 5: Graphics ----------------
        case 0xD000:  // DRW Vx, Vy, nibble
            printf("DRW V%X, V%X, %X\n", x, y, n);

            uint8_t xPos = chip8->V[x] % DISPLAY_WIDTH;
            uint8_t yPos = chip8->V[y] % DISPLAY_HEIGHT;

            chip8->V[0xF] = 0;  // reset

            for (unsigned int row = 0; row < n; row++) {
                uint8_t spriteByte = chip8->memory[chip8->index + row];

                for (unsigned int col = 0; col < 8; col++) {
                    uint8_t spritePixel =
                        spriteByte & (0x80u >> col);  // check each bit to see if 0 or 1
                    uint32_t* screenPixel =
                        &chip8->display[(yPos + row) * DISPLAY_WIDTH + (xPos + col)];

                    if (spritePixel) {
                        // Screen pixel also on - collision
                        if (*screenPixel == 0xFFFFFFFF) {
                            chip8->V[0xF] = 1;
                        }

                        // Effectively XOR with the sprite pixel
                        *screenPixel ^= 0xFFFFFFFF;
                    }
                }
            }
            dumpDisplay(chip8);
            break;

        // // ---------------- Stage 6: Input ----------------
        case 0xE000:
            uint8_t key = chip8->V[x];
            switch (kk) {
                case 0x9E:
                    printf("SKP V%X\n", x);
                    if (chip8->keypad[key]) chip8->pc += 2;
                    break;  //

                case 0xA1:
                    printf("SKNP V%X\n", x);
                    if (!chip8->keypad[key]) chip8->pc += 2;
                    break;  //

                default:
                    printf("Unknown opcode: %04X\n", opcode);
                    break;
            }
            break;

        // // ---------------- Stage 7: Timers & Memory ----------------
        case 0xF000:
            switch (kk) {
                case 0x07:
                    printf("LD V%X, DT\n", x);
                    chip8->V[x] = chip8->delay_timer;
                    break;  //
                case 0x0A:
                    printf("LD V%X, K (wait key)\n", x);
                    if (chip8->keypad[0]) {
                        chip8->V[x] = 0;
                    } else if (chip8->keypad[1]) {
                        chip8->V[x] = 1;
                    } else if (chip8->keypad[2]) {
                        chip8->V[x] = 2;
                    } else if (chip8->keypad[3]) {
                        chip8->V[x] = 3;
                    } else if (chip8->keypad[4]) {
                        chip8->V[x] = 4;
                    } else if (chip8->keypad[5]) {
                        chip8->V[x] = 5;
                    } else if (chip8->keypad[6]) {
                        chip8->V[x] = 6;
                    } else if (chip8->keypad[7]) {
                        chip8->V[x] = 7;
                    } else if (chip8->keypad[8]) {
                        chip8->V[x] = 8;
                    } else if (chip8->keypad[9]) {
                        chip8->V[x] = 9;
                    } else if (chip8->keypad[10]) {
                        chip8->V[x] = 10;
                    } else if (chip8->keypad[11]) {
                        chip8->V[x] = 11;
                    } else if (chip8->keypad[12]) {
                        chip8->V[x] = 12;
                    } else if (chip8->keypad[13]) {
                        chip8->V[x] = 13;
                    } else if (chip8->keypad[14]) {
                        chip8->V[x] = 14;
                    } else if (chip8->keypad[15]) {
                        chip8->V[x] = 15;
                    } else {
                        chip8->pc -= 2;
                    }
                    break;  //
                case 0x15:
                    printf("LD DT, V%X\n", x);
                    chip8->delay_timer = chip8->V[x];
                    break;  //
                case 0x18:
                    printf("LD ST, V%X\n", x);
                    chip8->sound_timer = chip8->V[x];
                    break;  //
                case 0x1E:
                    printf("ADD I, V%X\n", x);
                    chip8->index += chip8->V[x];
                    break;  //
                case 0x29:
                    printf("LD F, V%X (digit sprite)\n", x);
                    uint8_t digit = chip8->V[x];
                    chip8->index = FONTSET_START_ADDRESS + (5 * digit);
                    break;  //
                case 0x33:
                    printf("LD B, V%X (BCD)\n", x);
                    uint8_t value = chip8->V[x];
                    chip8->memory[chip8->index + 2] = value % 10;  // Ones-place
                    value /= 10;
                    chip8->memory[chip8->index + 1] = value % 10;  // Tens-place
                    value /= 10;
                    chip8->memory[chip8->index] = value % 10;  // Hundreds-place
                    break;
                case 0x55:
                    printf("LD [I], V0..V%X\n", x);
                    for (uint8_t i = 0; i <= x; ++i) {
                        chip8->memory[chip8->index + i] = chip8->V[i];
                    }
                    break;  //
                case 0x65:
                    printf("LD V0..V%X, [I]\n", x);
                    for (uint8_t i = 0; i <= x; ++i) {
                        chip8->V[i] = chip8->memory[chip8->index + i];
                    }
                    break;  //
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
    // [op][x][y][n]
    // Execute
}