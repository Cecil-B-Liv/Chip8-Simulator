#include <header.h>

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

void dumpDisplay(Chip8* chip8) {
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            uint32_t pixel = chip8->display[y * DISPLAY_WIDTH + x];
            putchar(pixel ? '#' : '.');  // '#' = ON, '.' = OFF
        }
        putchar('\n');
    }
    putchar('\n');
}

void chip8_screen_init() {}

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
    fclose(rom);
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

int romLoaderTest(Chip8* chip8, const uint8_t* rom, size_t romSize) {
    if (!rom) {
        fprintf(stderr, "Invalid ROM data\n");
        return -1;
    }

    // Read ROM directly into CHIP-8 memory
    memcpy(&chip8->memory[0x200], rom, romSize);
    printf("Load successfully.\n");
    return 1;
};

void debug_dump_memory(uint8_t* memory, int start, int length) {
    for (int i = 0; i < length; i += 16) {
        printf("%04X  ", start + i);  // print memory address

        // Hex part
        for (int j = 0; j < 16 && i + j < length; j++) {
            printf("%02X ", memory[start + i + j]);
        }

        // If fewer than 16 bytes, pad spacing
        for (int j = length - i; j < 16; j++) {
            printf("   ");
        }

        printf(" ");

        // ASCII part
        for (int j = 0; j < 16 && i + j < length; j++) {
            uint8_t c = memory[start + i + j];
            if (c >= 32 && c <= 126)  // printable range
                printf("%c", c);
            else
                printf(".");
        }

        printf("\n");
    }
}

void dumpRegisters(Chip8* chip8) {
    printf("==== V Registers ====\n");
    for (int i = 0; i < 16; i++) {
        printf("V%X: 0x%02X (%3d)\n", i, chip8->V[i], chip8->V[i]);
    }
    printf("=====================\n");
}

void chip8Cycle(Chip8* chip8) {
    if (chip8->pc >= MEM_SIZE - 2) {
        printf("PC out of bounds! %04X\n", chip8->pc);
        exit(1);
    }

    // Fetch
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