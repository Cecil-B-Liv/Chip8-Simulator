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

        // ---------------- Stage 2: Skip Instructions ----------------
        case 0x3000:  // SE Vx, byte (skip next if equal)
            printf("SE V%X, %02X\n", x, kk);
            if (chip8->V[x] == kk) {
                chip8->pc += 2;  // skip next instruction
            }
            break;

        case 0x4000:  // SNE Vx, byte (skip next if NOT equal)
            printf("SNE V%X, %02X\n", x, kk);
            if (chip8->V[x] != kk) {
                chip8->pc += 2;  // skip next instruction
            }
            break;

        case 0x5000:  // SE Vx, Vy (skip next if Vx == Vy)
            printf("SE V%X, V%X\n", x, y);
            if (chip8->V[x] == chip8->V[y]) {
                chip8->pc += 2;
            }
            break;

        case 0x6000:  // LD Vx, byte
            printf("LD V%X, %02X\n", x, kk);
            chip8->V[x] = kk;  // write to the V register
            break;

        case 0x7000:  // ADD Vx, byte
            printf("ADD V%X, %02X\n", x, kk);
            chip8->V[x] += kk;  // add to the V register
            break;

        // ---------------- Stage 3: Arithmetic/Logic (8XY_) ----------------
        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0:  // LD Vx, Vy
                    printf("LD V%X, V%X\n", x, y);
                    chip8->V[x] = chip8->V[y];
                    break;
                case 0x1:  // OR Vx, Vy
                    printf("OR V%X, V%X\n", x, y);
                    chip8->V[x] |= chip8->V[y];
                    break;
                case 0x2:  // AND Vx, Vy
                    printf("AND V%X, V%X\n", x, y);
                    chip8->V[x] &= chip8->V[y];
                    break;
                case 0x3:  // XOR Vx, Vy
                    printf("XOR V%X, V%X\n", x, y);
                    chip8->V[x] ^= chip8->V[y];
                    break;
                case 0x4:  // ADD Vx, Vy (with carry)
                    printf("ADD V%X, V%X\n", x, y);
                    {
                        uint16_t sum = chip8->V[x] + chip8->V[y];
                        chip8->V[0xF] = (sum > 0xFF) ? 1 : 0;
                        chip8->V[x] = sum & 0xFF;
                    }
                    break;
                case 0x5:  // SUB Vx, Vy
                    printf("SUB V%X, V%X\n", x, y);
                    chip8->V[0xF] = (chip8->V[x] >= chip8->V[y]) ? 1 : 0;
                    chip8->V[x] -= chip8->V[y];
                    break;
                case 0x6:  // SHR Vx
                    printf("SHR V%X\n", x);
                    chip8->V[0xF] = chip8->V[x] & 0x1;
                    chip8->V[x] >>= 1;
                    break;
                case 0x7:  // SUBN Vx, Vy
                    printf("SUBN V%X, V%X\n", x, y);
                    chip8->V[0xF] = (chip8->V[y] >= chip8->V[x]) ? 1 : 0;
                    chip8->V[x] = chip8->V[y] - chip8->V[x];
                    break;
                case 0xE:  // SHL Vx
                    printf("SHL V%X\n", x);
                    chip8->V[0xF] = (chip8->V[x] & 0x80) >> 7;
                    chip8->V[x] <<= 1;
                    break;
                default:
                    printf("Unknown 8XY_ opcode: %04X\n", opcode);
                    break;
            }
            break;

        case 0x9000:  // SNE Vx, Vy (skip if NOT equal)
            printf("SNE V%X, V%X\n", x, y);
            if (chip8->V[x] != chip8->V[y]) {
                chip8->pc += 2;
            }
            break;

        case 0xA000:  // LD I, addr
            printf("LD I, %03X\n", nnn);
            chip8->index = nnn;
            break;

        case 0xB000:  // JP V0, addr
            printf("JP V0 + %03X\n", nnn);
            chip8->pc = nnn + chip8->V[0];
            break;

        case 0xC000:  // RND Vx, byte
            printf("RND V%X, %02X\n", x, kk);
            chip8->V[x] = (rand() % 256) & kk;
            break;

        case 0xD000:  // DRW Vx, Vy, n (draw sprite)
            printf("DRW V%X, V%X, %X\n", x, y, n);
            {
                uint8_t xPos = chip8->V[x] % DISPLAY_WIDTH;
                uint8_t yPos = chip8->V[y] % DISPLAY_HEIGHT;
                chip8->V[0xF] = 0;

                for (uint8_t row = 0; row < n; row++) {
                    uint8_t spriteByte = chip8->memory[chip8->index + row];
                    for (uint8_t col = 0; col < 8; col++) {
                        if (spriteByte & (0x80 >> col)) {
                            uint8_t screenX = (xPos + col) % DISPLAY_WIDTH;
                            uint8_t screenY = (yPos + row) % DISPLAY_HEIGHT;
                            uint32_t* pixel = &chip8->display[screenY * DISPLAY_WIDTH + screenX];

                            if (*pixel == 0xFFFFFFFF) {
                                chip8->V[0xF] = 1;
                            }
                            *pixel ^= 0xFFFFFFFF;
                        }
                    }
                }
            }
            break;

        case 0xE000:
            switch (opcode & 0x00FF) {
                case 0x9E:  // SKP Vx (skip if key pressed)
                    printf("SKP V%X\n", x);
                    if (chip8->keypad[chip8->V[x]]) {
                        chip8->pc += 2;
                    }
                    break;
                case 0xA1:  // SKNP Vx (skip if key NOT pressed)
                    printf("SKNP V%X\n", x);
                    if (!chip8->keypad[chip8->V[x]]) {
                        chip8->pc += 2;
                    }
                    break;
                default:
                    printf("Unknown EX__ opcode: %04X\n", opcode);
                    break;
            }
            break;

        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x07:  // LD Vx, DT
                    printf("LD V%X, DT\n", x);
                    chip8->V[x] = chip8->delay_timer;
                    break;
                case 0x0A:  // LD Vx, K (wait for key press)
                    printf("LD V%X, K\n", x);
                    {
                        bool keyPressed = false;
                        for (int i = 0; i < 16; i++) {
                            if (chip8->keypad[i]) {
                                chip8->V[x] = i;
                                keyPressed = true;
                                break;
                            }
                        }
                        if (!keyPressed) {
                            chip8->pc -= 2;  // repeat this instruction
                        }
                    }
                    break;
                case 0x15:  // LD DT, Vx
                    printf("LD DT, V%X\n", x);
                    chip8->delay_timer = chip8->V[x];
                    break;
                case 0x18:  // LD ST, Vx
                    printf("LD ST, V%X\n", x);
                    chip8->sound_timer = chip8->V[x];
                    break;
                case 0x1E:  // ADD I, Vx
                    printf("ADD I, V%X\n", x);
                    chip8->index += chip8->V[x];
                    break;
                case 0x29:  // LD F, Vx (load font sprite)
                    printf("LD F, V%X\n", x);
                    chip8->index = FONTSET_START_ADDRESS + (chip8->V[x] * 5);
                    break;
                case 0x33:  // LD B, Vx (BCD)
                    printf("LD B, V%X\n", x);
                    chip8->memory[chip8->index] = chip8->V[x] / 100;
                    chip8->memory[chip8->index + 1] = (chip8->V[x] / 10) % 10;
                    chip8->memory[chip8->index + 2] = chip8->V[x] % 10;
                    break;
                case 0x55:  // LD [I], Vx (store V0-Vx)
                    printf("LD [I], V%X\n", x);
                    for (int i = 0; i <= x; i++) {
                        chip8->memory[chip8->index + i] = chip8->V[i];
                    }
                    break;
                case 0x65:  // LD Vx, [I] (load V0-Vx)
                    printf("LD V%X, [I]\n", x);
                    for (int i = 0; i <= x; i++) {
                        chip8->V[i] = chip8->memory[chip8->index + i];
                    }
                    break;
                default:
                    printf("Unknown FX__ opcode: %04X\n", opcode);
                    break;
            }
            break;

        default:
            printf("Unknown opcode: %04X\n", opcode);
            break;
    }
}