SDL_VERSION = 3.2.20
ARCH        = x86_64-w64-mingw32
SDL_PATH    = resources/SDL3-$(SDL_VERSION)/$(ARCH)

CFLAGS  = -I$(SDL_PATH)/include -Iinclude -Wall -Wextra -Werror
LDFLAGS = -L$(SDL_PATH)/lib -lSDL3

# Source files
SRCS = src/main.c src/chip8_core.c src/chip8_cycle.c src/platform.c

# Object files (now in bin/)
OBJS = $(patsubst src/%.c,bin/%.o,$(SRCS))

TARGET = bin/chip8.exe

all: $(TARGET) copy

$(TARGET): $(OBJS)
	gcc $(OBJS) -o $@ $(LDFLAGS)

bin/%.o: src/%.c | bin
	gcc -c $< -o $@ $(CFLAGS)

bin:
	mkdir bin

copy:
	copy "$(SDL_PATH)\\bin\\SDL3.dll" "bin\\SDL3.dll"

run: $(TARGET)
	bin/chip8.exe

clean:
	del /Q bin\*.o bin\*.exe bin\SDL3.dll 2>nul
	rmdir /S /Q bin 2>nul

.PHONY: all copy run clean