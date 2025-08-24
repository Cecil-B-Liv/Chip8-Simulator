SDL_VERSION = 3.2.20
ARCH        = x86_64-w64-mingw32
SDL_PATH = resources/SDL3-$(SDL_VERSION)/$(ARCH)

CFLAGS  = -I$(SDL_PATH)/include -Iinclude -Wall -Wextra -Werror
LDFLAGS = -L$(SDL_PATH)/lib -lSDL3 
#-mwindows

SRCS = $(wildcard src/*.c)
EXES = $(patsubst src/%.c,bin/%.exe,$(SRCS))

all: $(EXES) copy run

bin/%.exe: src/%.c | bin
	gcc $< -o $@ $(CFLAGS) $(LDFLAGS)

bin:
	mkdir bin

copy:
	copy "$(SDL_PATH)\\bin\\SDL3.dll" "bin\\SDL3.dll"

run:
	bin/chip8.exe

clean:
	del /Q bin\*.exe bin\SDL3.dll
	rmdir /S /Q bin
