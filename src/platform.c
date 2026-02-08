#include <header.h>

bool platform_init(Platform* p,
                   const char* title,
                   int windowWidth,
                   int windowHeight,
                   int textureWidth,
                   int textureHeight) {
    SDL_Init(SDL_INIT_VIDEO);

    p->windowWidth = windowWidth;
    p->windowHeight = windowHeight;
    p->textureWidth = textureWidth;
    p->textureHeight = textureHeight;

    p->window = SDL_CreateWindow(title,
                                 windowWidth,
                                 windowHeight,
                                 SDL_WINDOW_RESIZABLE);

    p->renderer = SDL_CreateRenderer(p->window, NULL);
    p->texture = SDL_CreateTexture(p->renderer,
                                   SDL_PIXELFORMAT_RGBA8888,
                                   SDL_TEXTUREACCESS_STREAMING,
                                   textureWidth,
                                   textureHeight);

    SDL_SetTextureScaleMode(p->texture, SDL_SCALEMODE_NEAREST);
    return true;
}

void platform_update(Platform* p, const void* buffer, int pitch) {
    SDL_SetTextureScaleMode(p->texture, SDL_SCALEMODE_NEAREST);
    SDL_UpdateTexture(p->texture, NULL, buffer, pitch);
    SDL_RenderClear(p->renderer);
    SDL_RenderTexture(p->renderer, p->texture, NULL, NULL);  
    SDL_RenderPresent(p->renderer);
}

void platform_destroy(Platform* p) {
    if (p->texture) SDL_DestroyTexture(p->texture);
    if (p->renderer) SDL_DestroyRenderer(p->renderer);
    if (p->window) SDL_DestroyWindow(p->window);
    SDL_Quit();
}

bool platform_processInput(uint8_t* keys) {
    SDL_Event event;
    bool quit = false;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                quit = true;
                break;
            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP: {
                bool isDown = (event.type == SDL_EVENT_KEY_DOWN);
                switch (event.key.key) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_X:
                        keys[0x0] = isDown;
                        break;
                    case SDLK_1:
                        keys[0x1] = isDown;
                        break;
                    case SDLK_2:
                        keys[0x2] = isDown;
                        break;
                    case SDLK_3:
                        keys[0x3] = isDown;
                        break;
                    case SDLK_Q:
                        keys[0x4] = isDown;
                        break;
                    case SDLK_W:
                        keys[0x5] = isDown;
                        break;
                    case SDLK_E:
                        keys[0x6] = isDown;
                        break;
                    case SDLK_A:
                        keys[0x7] = isDown;
                        break;
                    case SDLK_S:
                        keys[0x8] = isDown;
                        break;
                    case SDLK_D:
                        keys[0x9] = isDown;
                        break;
                    case SDLK_Z:
                        keys[0xA] = isDown;
                        break;
                    case SDLK_C:
                        keys[0xB] = isDown;
                        break;
                    case SDLK_4:
                        keys[0xC] = isDown;
                        break;
                    case SDLK_R:
                        keys[0xD] = isDown;
                        break;
                    case SDLK_F:
                        keys[0xE] = isDown;
                        break;
                    case SDLK_V:
                        keys[0xF] = isDown;
                        break;
                    default:
                        break;
                }
                break;
            }
            default:
                break;
        }
    }
    return quit;
}