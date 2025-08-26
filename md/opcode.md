# CHIP-8 Emulator — Opcode Implementation Stages

## 🟢 Stage 1: Basics (control flow & immediate load)
These are easy to implement and don’t depend on graphics or input:

- **00E0** → CLS (clear screen)  
- **00EE** → RET (return from subroutine)  
- **0NNN** → SYS (ignored in most emulators)  
- **1NNN** → JP (jump)  
- **2NNN** → CALL (subroutine call)  
- **6XKK** → LD Vx, byte  
- **7XKK** → ADD Vx, byte  

👉 These get your **PC (program counter)** and **registers** working.

---

## 🟢 Stage 2: Register ops (arithmetic/logic between Vx, Vy)
All instructions with first nibble = `8`. They use X, Y, and sometimes set carry/borrow flags in `VF`.

- **8XY0** → LD Vx, Vy  
- **8XY1** → OR Vx, Vy  
- **8XY2** → AND Vx, Vy  
- **8XY3** → XOR Vx, Vy  
- **8XY4** → ADD Vx, Vy (set VF = carry)  
- **8XY5** → SUB Vx, Vy (set VF = NOT borrow)  
- **8XY7** → SUBN Vx, Vy  
- **8XY6** → SHR Vx {, Vy}  
- **8XYE** → SHL Vx {, Vy}  

👉 These test your **registers + flags logic**.

---

## 🟢 Stage 3: Memory & timers
Now you touch memory, index register `I`, and delay/sound timers:

- **ANNN** → LD I, addr  
- **BNNN** → JP V0, addr  
- **CXKK** → RND Vx, byte  
- **FX07** → LD Vx, DT  
- **FX15** → LD DT, Vx  
- **FX18** → LD ST, Vx  
- **FX1E** → ADD I, Vx  
- **FX29** → LD F, Vx (sprite addr)  
- **FX33** → LD B, Vx (BCD)  
- **FX55** → LD [I], V0..Vx  
- **FX65** → LD V0..Vx, [I]  

👉 Here you need working **RAM, index register I, and timers**.

---

## 🟢 Stage 4: Graphics & input
The “fun” parts:

- **DXYN** → DRW Vx, Vy, nibble (sprite drawing)  
- **EX9E** → SKP Vx (skip if key pressed)  
- **EXA1** → SKNP Vx (skip if not pressed)  

👉 This tests your **display buffer + input**.

---

## 🟢 Stage 5: Conditional jumps
Branch-like instructions:

- **3XKK** → SE Vx, byte  
- **4XKK** → SNE Vx, byte  
- **5XY0** → SE Vx, Vy  
- **9XY0** → SNE Vx, Vy  

👉 These are easy but need a solid **PC increment/jump system**.
