#include <header.h>

// rom_stage1[] — put into memory at 0x200
uint8_t testRomStage1[] = {
    0x60,
    0x0A,  // 200: LD V0, 0x0A     ; V0 = 10
    0x70,
    0x05,  // 202: ADD V0, 0x05    ; V0 = 15
    0x22,
    0x08,  // 204: CALL 0x208      ; call subroutine at 208
    0x12,
    0x0C,  // 206: JP 20C          ; jump forward (skip 208 after return)

    // Subroutine:
    0x00,
    0xE0,  // 208: CLS             ; clear screen
    0x00,
    0xEE,  // 20A: RET             ; return to 206

    // After subroutine returns:
    0x60,
    0x01,  // 20C: LD V0, 0x01     ; overwrite V0 with 1
    0x70,
    0x02,  // 20E: ADD V0, 0x02    ; V0 = 3
    0x12,
    0x0C  // 210: JP 20C          ; infinite loop
};

// Stage 2 Test ROM (load at 0x200)
uint8_t stage2_rom[] = {
    // --- SE Vx, byte (skip next if equal) ---
    0x60,
    0x05,  // 200: LD V0, 0x05
    0x30,
    0x05,  // 202: SE V0, 0x05 (should skip next)
    0x60,
    0xFF,  // 204: LD V0, 0xFF (should be skipped)

    // --- SNE Vx, byte (skip next if NOT equal) ---
    0x40,
    0x06,  // 206: SNE V0, 0x06 (V0=5, so not equal → skip next)
    0x60,
    0xEE,  // 208: LD V0, 0xEE (skipped)

    // --- SE Vx, Vy (skip if equal) ---
    0x61,
    0x05,  // 20A: LD V1, 0x05
    0x50,
    0x10,  // 20C: SE V0, V1 (equal → skip next)
    0x60,
    0xDD,  // 20E: LD V0, 0xDD (skipped)

    // --- LD I, addr + JP V0, addr test (prep for stage 3, harmless now) ---
    0xA2,
    0x20,  // 210: LD I, 0x220 (for later)
    0xB2,
    0x20,  // 212: JP V0 + 0x220 (V0 = 5, should jump to 225)

    // Filler at 0x220+5 = 0x225
    0x60,
    0x77,  // 225: LD V0, 0x77
};

// Stage 3 Test ROM (load at 0x200)
uint8_t stage3_rom[] = {
    // --- LD Vx, Vy ---
    0x60,
    0x0A,  // 200: LD V0, 0x0A
    0x61,
    0x05,  // 202: LD V1, 0x05
    0x80,
    0x10,  // 204: LD V0, V1  (V0 = V1 = 5)

    // --- OR Vx, Vy ---
    0x62,
    0x0F,  // 206: LD V2, 0x0F
    0x63,
    0xF0,  // 208: LD V3, 0xF0
    0x82,
    0x31,  // 20A: OR V2, V3  (V2 = 0xFF)

    // --- AND Vx, Vy ---
    0x64,
    0xF0,  // 20C: LD V4, 0xF0
    0x65,
    0x0F,  // 20E: LD V5, 0x0F
    0x84,
    0x52,  // 210: AND V4, V5 (V4 = 0x00)

    // --- XOR Vx, Vy ---
    0x66,
    0xFF,  // 212: LD V6, 0xFF
    0x67,
    0x0F,  // 214: LD V7, 0x0F
    0x86,
    0x73,  // 216: XOR V6, V7 (V6 = 0xF0)

    // --- ADD Vx, Vy (with carry) ---
    0x68,
    0xF0,  // 218: LD V8, 0xF0
    0x69,
    0x20,  // 21A: LD V9, 0x20
    0x88,
    0x94,  // 21C: ADD V8, V9 (V8=0x10, VF=1)

    // --- SUB Vx, Vy ---
    0x6A,
    0x0A,  // 21E: LD VA, 0x0A
    0x6B,
    0x05,  // 220: LD VB, 0x05
    0x8A,
    0xB5,  // 222: SUB VA, VB (VA=5, VF=1)

    // --- SHR Vx ---
    0x6C,
    0x03,  // 224: LD VC, 0x03
    0x8C,
    0x06,  // 226: SHR VC (VC=1, VF=1)

    // --- SUBN Vx, Vy ---
    0x6D,
    0x02,  // 228: LD VD, 0x02
    0x6E,
    0x07,  // 22A: LD VE, 0x07
    0x8D,
    0xE7,  // 22C: SUBN VD, VE (VD=5, VF=1)

    // --- SHL Vx ---
    0x6F,
    0x81,  // 22E: LD VF, 0x81
    0x8F,
    0x0E,  // 230: SHL VF (VF=0x02, carry=1)

    // End (jump to self to halt)
    0x12,
    0x30  // 232: JP 0x230
};

// Simple CHIP-8 program to test DRW
// Loads the sprite for "0" into memory and draws it at (0,0)
uint8_t graphicTestRom[] = {0x60,
                            0x00,  // LD V0,0
                            0x61,
                            0x00,  // LD V1,0
                            0xA2,
                            0x0A,  // LD I,0x20A
                            0xD0,
                            0x15,  // DRW V0,V1,5
                            0x12,
                            0x06,  // JP 206 (instead of 200) → skip redraw

                            0xF0,
                            0x90,
                            0x90,
                            0x90,
                            0xF0};
