# CHIP-8 Emulator

A CHIP-8 interpreter/emulator written in C using SDL3 for graphics and input handling.

## What is CHIP-8?

CHIP-8 is a simple virtual machine designed in the 1970s for running games on early microcomputers. This emulator can run classic CHIP-8 games and programs, providing an authentic retro computing experience.

## Features

- **Complete CHIP-8 instruction set** - All 35 opcodes implemented
- **Accurate timing** - 500Hz CPU with 60Hz timers
- **Graphics rendering** - 64x32 pixel display with SDL3
- **Sound support** - Beep audio when sound timer is active
- **Keyboard input** - Full 16-key hexadecimal keypad mapping
- **Multiple ROM formats** - Supports standard .ch8 ROM files
- **Cross-platform** - Linux and Windows support via build scripts

## Controls

The CHIP-8 keypad is mapped to your keyboard as follows:

```
CHIP-8 Keypad:    Keyboard:
1 2 3 C           1 2 3 4
4 5 6 D           Q W E R  
7 8 9 E           A S D F
A 0 B F           Z X C V
```

Press `ESC` to quit the emulator.

## Building

### Linux
```bash
# Make sure you have SDL3 development libraries installed
# Ubuntu/Debian: sudo apt install libsdl3-dev
# Arch: sudo pacman -S sdl3

# I have a folder of SDL3 already anyway :D

# Switch to Linux build configuration
./z_archive/switch_build.sh linux

# Build and run
make run
```

### Windows
```bash
# Switch to Windows build configuration (uses bundled SDL3)
./z_archive/switch_build.sh windows

# Build and run
make run
```

The build system includes:
- Automatic dependency management
- Cross-compilation support 
- Bundled SDL3 libraries for Windows
- Clean separation between Linux/Windows configurations

## Usage

```bash
# Run with default ROM
make clean # to clean any leftover
make run

# Or run directly with a specific ROM
./bin/chip8 path/to/your/game.ch8
```

### Test ROMs

The emulator has been tested with 
https://github.com/Timendus/chip8-test-suite

## Architecture

```
src/
├── main.c          # Main loop, timing, and SDL initialization
├── chip8_core.c    # CHIP-8 system initialization and ROM loading  
├── chip8_cycle.c   # CPU instruction execution and decode
└── platform.c     # SDL3 graphics, audio, and input abstraction

include/
├── chip8.h         # CHIP-8 system state and function declarations
├── chip8Cycle.h    # CPU cycle execution
└── platform.h     # Platform abstraction layer
```

The codebase follows a clean modular design with:
- **Separated concerns** - Graphics, audio, CPU, and I/O in separate modules
- **Platform abstraction** - SDL3 dependencies isolated to platform layer
- **Clear interfaces** - Well-defined APIs between components

## Technical Details

- **CPU Speed**: 500Hz (configurable)
- **Timer Frequency**: 60Hz for delay and sound timers
- **Display**: 64x32 monochrome pixels
- **Memory**: 4KB RAM with program loading at 0x200
- **Stack**: 16 levels for subroutine calls
- **Audio**: Square wave beep at 800Hz

### CHIP-8 Quirks Support

This emulator implements modern CHIP-8 behavior:
- ✅ VF reset on logic operations (AND, OR, XOR)
- ✅ Memory operations increment index register
- ✅ Sprite clipping at screen edges
- ✅ Shift operations use source register
- ✅ Accurate arithmetic with proper flag handling

## ROM Compatibility

Supports standard CHIP-8 ROM files (.ch8). 

Place ROM files in the `roms/` directory or specify the full path when running. And change the roms name in `main.c` line 15 to whichever rom you want to run.

Popular ROM collections can be found online.

Example link: https://johnearnest.github.io/chip8Archive/

## Development

### Adding New Features
The modular design makes it easy to extend:
- **New instructions**: Add to `chip8_cycle.c` 
- **Enhanced graphics**: Modify `platform.c`
- **Different input devices**: Update key mapping in `platform_processInput()`

### Debugging
- **Debug output**: Instruction execution is logged to console
- **Memory dumps**: Built-in functions for inspecting system state
- **Test ROMs**: Use the included test ROMs to validate behavior

## References
- Cowgod chip8 guide: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM 
- Errata to Cowgod instruction: https://github.com/gulrak/cadmium/wiki/CTR-Errata
- Test Suite: https://github.com/Timendus/chip8-test-suite
- Discord link to an emulator community that help me to finish this project: https://discord.gg/dkmJAes  
- Copilot :D

## License

I dont even know whats license btw.

---

*Built with C and SDL3. Tested on Linux and Windows.*