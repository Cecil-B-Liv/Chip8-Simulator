#define SDL_MAIN_HANDLED
#define TEXTURE_W 2000
#define TEXTURE_H 2000
#define SCREEN_W 1000
#define SCREEN_H 1000
#define MAX_POINTS 10000

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdlib.h>

// -------------------------
// PointBuffer = "Vector" of points
// -------------------------
typedef struct {
    SDL_FPoint data[MAX_POINTS];
    int count;
} PointBuffer;

void PointBuffer_init(PointBuffer* pb) { pb->count = 0; }

void pixel_add(PointBuffer* pb, float x, float y) {
    if (pb->count < MAX_POINTS) {
        pb->data[pb->count].x = x;
        pb->data[pb->count].y = y;
        pb->count++;
    }
}

void PointBuffer_clear(PointBuffer* pb) {
    memset(pb->data, 0, sizeof(pb->data)); // wipe memory to zero
    pb->count = 0;   // "forget" all points
    // for (int i = 0; i < MAX_POINTS; i++) {
    //     pb->data[i].x = 0;
    //     pb->data[i].y = 0;
    // }
}

// -------------------------
// Screen = "class" bundling SDL stuff
// -------------------------
typedef struct {
    SDL_Event e;
    SDL_Window* window;
    SDL_Renderer* renderer;
    PointBuffer points;
} Screen;

void Screen_init(Screen* s) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer("Rotating Cube",
                                640 * 2,
                                480 * 2,
                                0,           // flags
                                &s->window,  // assign into struct
                                &s->renderer);
    SDL_SetRenderScale(s->renderer, 2, 2);  // logical scaling
}

void show(Screen* s) {
    SDL_SetRenderDrawColor(s->renderer, 0, 0, 0, 255);
    SDL_RenderClear(s->renderer);
    
    SDL_SetRenderDrawColor(s->renderer, 255, 255, 255, 255);
    SDL_RenderPoints(s->renderer, s->points.data, s->points.count);
    
    SDL_RenderPresent(s->renderer);
}

void clear(Screen* s){
    PointBuffer_clear(&s->points);
}

void input(Screen* s) {
    while (SDL_PollEvent(&s->e)) {
        if (s->e.type == SDL_EVENT_QUIT) {
            SDL_Quit();
            exit(0);
        }
    }
}