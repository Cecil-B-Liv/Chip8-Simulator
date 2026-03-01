# ============================================================================
# CHIP-8 Simulator Makefile
# ============================================================================
# This Makefile compiles the CHIP-8 emulator with proper separation of
# header files (.h) and implementation files (.c)
# ============================================================================

# --- SDL3 Configuration ---
# SDL3 version and architecture settings
SDL_VERSION = 3.2.20
ARCH        = x86_64-w64-mingw32
SDL_PATH    = resources/SDL3-$(SDL_VERSION)/$(ARCH)

# --- Compiler Flags ---
# CFLAGS: Compiler flags for compilation stage
#   -I$(SDL_PATH)/include : Add SDL3 include directory
#   -Iinclude            : Add our project's include directory
#   -Wall                : Enable all warnings
#   -Wextra              : Enable extra warnings
#   -Werror              : Treat warnings as errors
CFLAGS  = -I$(SDL_PATH)/include -Iinclude -Wall -Wextra -Werror

# LDFLAGS: Linker flags for linking stage
#   -L$(SDL_PATH)/lib : Add SDL3 library directory
#   -lSDL3            : Link with SDL3 library
LDFLAGS = -L$(SDL_PATH)/lib -lSDL3

# --- Source Files ---
# List all .c source files that need to be compiled
# Each file contains the implementation of functions declared in headers
SRCS = src/main.c src/chip8_core.c src/chip8_cycle.c src/platform.c

# --- Object Files ---
# Transform source file paths to object file paths
# Example: src/main.c -> bin/main.o
# Object files (.o) are intermediate compiled files before final linking
OBJS = $(patsubst src/%.c,bin/%.o,$(SRCS))

# --- Target Executable ---
# The final executable program
TARGET = bin/chip8.exe

# ============================================================================
# BUILD RULES
# ============================================================================

# --- Default Target ---
# Running 'make' or 'make all' will build the executable and copy SDL3.dll
all: $(TARGET) copy

# --- Link Object Files into Executable ---
# Combines all .o files into the final .exe
# $(OBJS) expands to: bin/main.o bin/chip8_core.o bin/chip8_cycle.o bin/platform.o
# The linker (gcc) links them together with SDL3 library
$(TARGET): $(OBJS)
	@echo "Linking object files into $(TARGET)..."
	gcc $(OBJS) -o $@ $(LDFLAGS)
	@echo "Build successful!"

# --- Compile Source Files into Object Files ---
# Pattern rule: for each src/*.c file, create a bin/*.o file
# $< refers to the source file (e.g., src/main.c)
# $@ refers to the target file (e.g., bin/main.o)
# | bin ensures the bin directory exists before compiling
bin/%.o: src/%.c | bin
	@echo "Compiling $<..."
	gcc -c $< -o $@ $(CFLAGS)

# --- Create bin Directory ---
# Creates the bin/ directory if it doesn't exist
bin:
	@echo "Creating bin directory..."
	mkdir bin

# --- Copy SDL3 DLL ---
# Copies the SDL3.dll from resources to bin/ folder
# The executable needs SDL3.dll to run
copy:
	@echo "Copying SDL3.dll to bin directory..."
	copy "$(SDL_PATH)\\bin\\SDL3.dll" "bin\\SDL3.dll"

# --- Run the Program ---
# Builds the program (if needed) and runs it
run: $(TARGET)
	@echo "Running CHIP-8 emulator..."
	bin/chip8.exe

# --- Clean Build Artifacts ---
# Removes all compiled files and the bin directory
# 2>nul suppresses error messages if files don't exist
clean:
	@echo "Cleaning build artifacts..."
	del /Q bin\*.o bin\*.exe bin\SDL3.dll 2>nul
	rmdir /S /Q bin 2>nul
	@echo "Clean complete!"

# ============================================================================
# PHONY TARGETS
# ============================================================================
# These targets don't create files, they're just commands
# Declaring them as .PHONY prevents conflicts with files of the same name
.PHONY: all copy run clean

# ============================================================================
# HOW TO USE THIS MAKEFILE
# ============================================================================
# make          - Builds the entire project (compiles and links)
# make clean    - Removes all compiled files
# make run      - Builds and runs the emulator
# make copy     - Only copies SDL3.dll (rarely needed separately)
# ============================================================================

# ============================================================================
# BUILD PROCESS EXPLANATION
# ============================================================================
# 1. Compilation (src/*.c -> bin/*.o):
#    Each .c file is compiled separately into an object file
#    Object files contain machine code but aren't yet executable
#
# 2. Linking (bin/*.o -> bin/chip8.exe):
#    All object files are linked together with SDL3 library
#    Creates the final executable
#
# 3. Why separate compilation?
#    - Faster rebuilds (only changed files are recompiled)
#    - Better code organization (separate concerns)
#    - Proper C programming practice
# ============================================================================