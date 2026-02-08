#ifndef HEADER_H
#define HEADER_H

// Standard C libraries
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Windows-specific (only if needed)
#ifdef _WIN32
    #include <windows.h>
#endif

// SDL3 - but NOT SDL_main.h here!
#include <SDL3/SDL.h>

// Project headers
#include "chip8.h"
#include "chip8Cycle.h"
#include "platform.h"

#endif // HEADER_H