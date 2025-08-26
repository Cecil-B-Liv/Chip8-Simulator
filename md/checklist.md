# CHIP-8 Emulator Roadmap (C + SDL)

## Step 0 — Pick stack + libs
- **Language**: C
- **Window/Input/Audio**: SDL2 or SDL3
- **Deliverable**: a blank SDL window with a fixed tick loop.

---

## Step 1 — Define the CHIP-8 machine state
Create a `struct` for the VM:

- `uint8_t mem[4096];` — memory
- `uint8_t V[16];` — registers V0–VF (VF = flag/carry)
- `uint16_t I;` — index register
- `uint16_t PC;` — program counter (starts at `0x200`)
- `uint16_t stack[16];`
- `uint8_t SP;` — stack pointer
- `uint8_t delay, sound;` — timers (decrement at 60Hz)
- `uint8_t gfx[64 * 32];` — display buffer
- `uint8_t keys[16];` — keypad (0–F)

**Init rules:**
- Zero everything
- `PC = 0x200`
- Load fontset into memory

**Deliverable**: construct + init function; nothing drawn yet.

---

## Step 2 — Load the fontset
- CHIP-8 needs 16 sprites (0–F), each **5 bytes tall**.
- Put them in memory starting at **0x050**.

**Check:** later, opcode `FX29` sets `I = font_base + 5*Vx`.

---

## Step 3 — Load a ROM
- Read binary into memory starting at `mem[0x200]`.
- Compute file size; ensure `<= 4096 - 0x200`.
- After loading: first opcode at `0x200` should equal first two ROM bytes.

---

## Step 4 — Fetch/Decode/Execute skeleton
- **Fetch**: `opcode = (mem[PC] << 8) | mem[PC+1];`
- **Default increment**: `PC += 2` (some opcodes override).
- **Decode fields**:
  - `nnn = opcode & 0x0FFF;`
  - `kk  = opcode & 0x00FF;`
  - `n   = opcode & 0x000F;`
  - `x   = (opcode & 0x0F00) >> 8;`
  - `y   = (opcode & 0x00F0) >> 4;`
- **Switch** on `(opcode & 0xF000)` → refine cases.

**Deliverable**: loop runs, decodes, but only implement a few opcodes first.

---

## Step 5 — Tiny, testable subset of opcodes
Start with:
- `00E0` — CLS
- `00EE` — RET
- `1NNN` — JP addr
- `6XKK` — LD Vx, byte
- `7XKK` — ADD Vx, byte
- `ANNN` — LD I, addr
- `DXYN` — DRW Vx, Vy, n

**Draw details**:
- Display: 64×32
- Sprite: `n` rows from `mem[I]..mem[I+n-1]`
- Each bit → 1 pixel (bit7 = leftmost)
- `gfx[(x+bit)%64 + (y+r)%32 * 64] ^= 1`
- If a pixel is erased (1→0), set VF=1
- Track a `draw_flag` for SDL renderer

**Deliverable**: run ROM (IBM logo) → pixels on screen.

---

## Step 6 — Input (hex keypad)
- Map PC keys → CHIP-8:
- CHIP-8: 1 2 3 C 4 5 6 D 7 8 9 E A 0 B F
- Keyboard: 1 2 3 4 Q W E R A S D F Z X C V

- Update `keys[0..15]` on SDL keydown/up.

Implement:
- `EX9E` — SKP Vx
- `EXA1` — SKNP Vx
- `FX0A` — LD Vx, K (wait for key press)

---

## Step 7 — Timers @ 60Hz
- Decrement `delay` and `sound` at **60Hz**.
- In loop, accumulate time → tick every 1/60s.
- If `sound > 0`: play beep.

Opcodes:
- `FX07` — Vx = delay
- `FX15` — delay = Vx
- `FX18` — sound = Vx

---

## Step 8 — More arithmetic/logic opcodes
Implement 8XY* family:
- `8XY0` LD
- `8XY1` OR
- `8XY2` AND
- `8XY3` XOR
- `8XY4` ADD (VF = carry)
- `8XY5` SUB (VF = NOT borrow)
- `8XY6` SHR
- `8XY7` SUBN
- `8XYE` SHL

Also:
- `3XKK` SE
- `4XKK` SNE
- `5XY0` SE
- `9XY0` SNE
- `BNNN` JP V0+addr
- `CXKK` RND
- `FX1E` I += Vx
- `FX29` I = sprite(Vx)
- `FX33` BCD
- `FX55` store V0..Vx
- `FX65` load V0..Vx

---

## Step 9 — Rendering in SDL
- Create 64×32 texture, scale ×10 (→ 640×320).
- Draw white for 1, black for 0.
- Only redraw when `draw_flag` set.

---

## Step 10 — Timing / CPU speed
- Run cycles at ~500–700 Hz.
- Keep timers at 60 Hz.
- Do this by stepping multiple cycles per frame.

---

## Step 11 — Debugging helpers
- Add disassembler to log.
- Add step mode (pause/step).
- Optionally display registers + PC.

---

## Common pitfalls
- **PC increments**: Always `+= 2`, unless jump/skip modifies.
- **Sprite wraparound**: Wrap at screen edges.
- **VF usage**: Only when opcode specifies.
- **FX0A (wait key)**: Must block until input.
- **Endianness**: Opcodes are big-endian in memory.
