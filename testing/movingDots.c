#define SDL_MAIN_HANDLED
#define TEXTURE_W 2000
#define TEXTURE_H 2000
#define SCREEN_W 1000
#define SCREEN_H 1000
#define NUM_POINTS 10000

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("MovingDots", SCREEN_W, SCREEN_H, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    SDL_Texture* texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, TEXTURE_W, TEXTURE_H);

    SDL_FRect source = {0, 0, SCREEN_W / 32, SCREEN_H / 32};  // x,y,size
    SDL_FRect dest = {10, 10, SCREEN_W - 20, SCREEN_H - 20};

    SDL_FPoint points[NUM_POINTS];
    srand((unsigned)time(NULL));
    for (int i = 0; i < NUM_POINTS; i++) {
        points[i].x = rand() % TEXTURE_W;
        points[i].y = rand() % TEXTURE_H;
    }

    int running = 1;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                running = 0;
            }
            if (e.type == SDL_EVENT_KEY_DOWN) {  // keyboard
                printf("Key pressed: %s\n", SDL_GetKeyName(e.key.key));
                switch (e.key.key) {
                    case SDLK_UP:
                        source.y -= 3;
                        break;
                    case SDLK_DOWN:
                        source.y += 3;
                        break;
                    case SDLK_LEFT:
                        source.x -= 3;
                        break;
                    case SDLK_RIGHT:
                        source.x += 3;
                        break;
                    case SDLK_1:
                        source.w *= 2;
                        source.h *= 2;
                        break;
                    case SDLK_2:
                        source.w /= 2;
                        source.h /= 2;
                        break;
                }
            }
        }

        SDL_SetRenderTarget(renderer, texture);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // black
        SDL_RenderClear(renderer);  // fill the screen with the chosen color

        for (int i = 0; i < NUM_POINTS; i++) {
            points[i].x += rand() % 3 - 1;
            points[i].y += rand() % 3 - 1;

            if (points[i].x < 0)  // cover exception cases
                points[i].x = 0;
            if (points[i].y < 0) points[i].y = 0;
            if (points[i].x >= TEXTURE_W) points[i].x = TEXTURE_W - 1;
            if (points[i].y >= TEXTURE_H) points[i].y = TEXTURE_H - 1;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderPoints(renderer, points, NUM_POINTS);  // white point

        SDL_SetRenderTarget(renderer, NULL);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // white color
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, &source, &dest);
        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
        SDL_RenderPresent(renderer);

        SDL_Delay(50);
    }

    return 0;
}
