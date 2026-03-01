# ============================================================================
# CHIP-8 Simulator Makefile - Linux Version
# ============================================================================
# This Makefile compiles the CHIP-8 emulator for Linux systems
# ============================================================================

# --- Detect OS ---
UNAME_S := $(shell uname -s)

# --- Linux Configuration ---
ifeq ($(UNAME_S),Linux)
    # Use system SDL3 or pkg-config
    CFLAGS  = $(shell pkg-config --cflags sdl3 2>/dev/null || echo "-I/usr/include/SDL3") -Iinclude -Wall -Wextra -Werror
    LDFLAGS = $(shell pkg-config --libs sdl3 2>/dev/null || echo "-lSDL3")
    TARGET = bin/chip8
    RM_CMD = rm -f
    RMDIR_CMD = rm -rf
    MKDIR_CMD = mkdir -p
endif

# --- Fallback for Windows (original config) ---
ifeq ($(UNAME_S),MINGW64_NT)
    SDL_VERSION = 3.2.20
    ARCH        = x86_64-w64-mingw32
    SDL_PATH    = resources/SDL3-$(SDL_VERSION)/$(ARCH)
    CFLAGS  = -I$(SDL_PATH)/include -Iinclude -Wall -Wextra -Werror
    LDFLAGS = -L$(SDL_PATH)/lib -lSDL3
    TARGET = bin/chip8.exe
    RM_CMD = del /Q
    RMDIR_CMD = rmdir /S /Q
    MKDIR_CMD = mkdir
endif

# --- Source Files ---
SRCS = src/main.c src/chip8_core.c src/chip8_cycle.c src/platform.c
OBJS = $(patsubst src/%.c,bin/%.o,$(SRCS))

# ============================================================================
# BUILD RULES
# ============================================================================

# --- Default Target ---
all: $(TARGET)

# --- Link Object Files into Executable ---
$(TARGET): $(OBJS)
	@echo "Linking object files into $(TARGET)..."
	gcc $(OBJS) -o $@ $(LDFLAGS) -lm
	@echo "Build successful!"

# --- Compile Source Files into Object Files ---
bin/%.o: src/%.c | bin
	@echo "Compiling $<..."
	gcc -c $< -o $@ $(CFLAGS)

# --- Create bin Directory ---
bin:
	@echo "Creating bin directory..."
	$(MKDIR_CMD) bin

# --- Run the Program ---
run: $(TARGET)
	@echo "Running CHIP-8 emulator..."
	./$(TARGET)

# --- Clean Build Artifacts ---
clean:
	@echo "Cleaning build artifacts..."
	$(RM_CMD) bin/*.o bin/chip8 bin/chip8.exe 2>/dev/null || true
	$(RMDIR_CMD) bin 2>/dev/null || true
	@echo "Clean complete!"

# ============================================================================
# PHONY TARGETS
# ============================================================================
.PHONY: all run clean

# ============================================================================
# HOW TO USE THIS MAKEFILE
# ============================================================================
# make          - Builds the entire project
# make clean    - Removes all compiled files
# make run      - Builds and runs the emulator
# ============================================================================