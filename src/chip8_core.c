#include <header.h>

void chip8_init(Chip8* chip8) {
    memset(chip8->memory, 0, MEM_SIZE * sizeof(chip8->memory[0]));  // clear memory
    memset(chip8->V, 0, 16);                                        // clear V registers
    chip8->index = 0;                                               // clear index register
    chip8->pc = 0x200;  // set program counter to start of program area
    memset(chip8->stack, 0, STACK_SIZE * sizeof(chip8->stack[0]));  // clear stack
    chip8->sp = 0;                                                  // clear stack pointer
    chip8->delay_timer = 0;                                         // clear delay timer
    chip8->sound_timer = 0;                                         // clear sound timer
    memset(chip8->display,
           0,
           DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(chip8->display[0]));  // clear display
    memset(chip8->keypad, 0, KEYPAD_SIZE * sizeof(chip8->keypad[0]));    // clear keypad

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

    memcpy(&chip8->memory[0x50], chip8_fontset, sizeof(chip8_fontset));  // load fontset into memory
}

void chip8_screen_init(void) {
    // Empty for now
}

void dumpDisplay(Chip8* chip8) {  // for debugging: print the display to console
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < DISPLAY_WIDTH; x++) {
            uint32_t pixel = chip8->display[y * DISPLAY_WIDTH + x];
            putchar(pixel ? '#' : '.');  // '#' = ON, '.' = OFF
        }
        putchar('\n');
    }
    putchar('\n');
}

long getRomSize(const char* filename) {  // get size of the rom file
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

int romLoaderHasMaloc(Chip8* chip8, const char* filename) {  // use malloc to load rom
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

    // Copy ROM into CHIP-8 memory starting at 0x200
    memcpy(&chip8->memory[0x200], buffer, size);
    free(buffer);
    fclose(rom);
    printf("Load successfully.\n");
    return 1;
}

int romLoaderNoMaloc(Chip8* chip8, const char* filename) {  // no malloc version
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
}

int romLoaderTest(Chip8* chip8, const uint8_t* rom, size_t romSize) {
    if (!rom) {
        fprintf(stderr, "Invalid ROM data\n");
        return -1;
    }

    // Read ROM directly into CHIP-8 memory
    memcpy(&chip8->memory[0x200], rom, romSize);
    printf("Test ROM loaded successfully.\n");
    return 1;
}

void debug_dump_memory(const uint8_t* memory, size_t start, size_t length) {
    printf("==== Memory Dump (0x%04zX - 0x%04zX) ====\n", start, start + length - 1);
    for (size_t i = 0; i < length; i += 16) {
        printf("%04zX: ", start + i);

        // Hex view
        for (size_t j = 0; j < 16; j++) {
            if (i + j < length)
                printf("%02X ", memory[start + i + j]);
            else
                printf("   ");
        }

        printf(" | ");

        // ASCII view
        for (size_t j = 0; j < 16 && i + j < length; j++) {
            uint8_t c = memory[start + i + j];
            if (c >= 32 && c <= 126)  // printable range
                printf("%c", c);
            else
                printf(".");
        }

        printf("\n");
    }
}

void dumpRegisters(Chip8* chip8) {  // for debugging: print the V registers to console
    printf("==== V Registers ====\n");
    for (int i = 0; i < 16; i++) {
        printf("V%X: 0x%02X (%3d)\n", i, chip8->V[i], chip8->V[i]);
    }
    printf("=====================\n");
}