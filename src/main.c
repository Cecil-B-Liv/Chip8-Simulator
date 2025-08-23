#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "screen.h"

#define NUM_POINTS 8

typedef struct {
    float x, y, z;
} vec3;

typedef struct {
    int a, b;
} connection;

void fps_counter() {
    static Uint64 last_time = 0;
    static int frames = 0;
    static float fps = 0.0f;

    Uint64 now = SDL_GetTicks();  // milliseconds
    frames++;

    if (now - last_time >= 1000) {  // 1 second passed
        fps = frames * 1000.0f / (now - last_time);
        printf("FPS: %.2f\n", fps);  // print to console
        last_time = now;
        frames = 0;
    }
}

void rotate(vec3* point, float x, float y, float z) {
    if (x == 0) x = 1;  // manually set defaults
    if (y == 0) y = 1;
    if (z == 0) z = 1;
    float rad = 0;

    rad = x;
    point->y = cos(rad) * point->y - sin(rad) * point->z;
    point->z = sin(rad) * point->y + cos(rad) * point->z;
    rad = y;
    point->x = cos(rad) * point->x - sin(rad) * point->z;
    point->z = sin(rad) * point->x + cos(rad) * point->z;
    rad = z;
    point->x = cos(rad) * point->x - sin(rad) * point->y;
    point->y = sin(rad) * point->x + cos(rad) * point->y;

}

void line(Screen* screen, float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float length = sqrt(dx * dx + dy * dy);
    float angle = atan2(dy, dx);

    for (float i = 0; i < length; i++) {
        pixel_add(&screen->points, x1 + cos(angle) * i, y1 + sin(angle) * i);
    }
}

int main() {
    int counter_fps = 0;

    srand((unsigned)time(NULL));
    Screen screen;
    Screen_init(&screen);

    for (int i = 0; i < 100; i++) {
        pixel_add(&screen.points, rand() % 640, rand() % 480);
    }

    vec3 vecPoints[NUM_POINTS] = {{250, 250, 300},
                                  {350, 250, 300},
                                  {350, 350, 300},
                                  {250, 350, 300},
                                  {250, 250, 400},
                                  {350, 250, 400},
                                  {350, 350, 400},
                                  {250, 350, 400}};

    connection connections[] = {
        {0, 4},
        {1, 5},
        {2, 6},
        {3, 7},

        {0, 1},
        {1, 2},
        {2, 3},
        {3, 0},

        {4, 5},
        {5, 6},
        {6, 7},
        {7, 4},
    };

    vec3 centroid = {0, 0, 0};
    for (int i = 0; i < NUM_POINTS; i++) {
        centroid.x += vecPoints[i].x;
        centroid.y += vecPoints[i].y;
        centroid.z += vecPoints[i].z;
    }

    centroid.x /= NUM_POINTS;
    centroid.y /= NUM_POINTS;
    centroid.z /= NUM_POINTS;

    while (true) {
        for (int i = 0; i < NUM_POINTS; i++) {
            vecPoints[i].x -= centroid.x;
            vecPoints[i].y -= centroid.y;
            vecPoints[i].z -= centroid.z;
            rotate(&vecPoints[i], 0.004, 0.002, 0.008);

            vecPoints[i].x += centroid.x;
            vecPoints[i].y += centroid.y;
            vecPoints[i].z += centroid.z;
            pixel_add(&screen.points, vecPoints[i].x, vecPoints[i].y);
        }

        for (int i = 0; i < sizeof(connections) / sizeof(connections[0]); i++) {
            line(&screen,
                 vecPoints[connections[i].a].x,
                 vecPoints[connections[i].a].y,
                 vecPoints[connections[i].b].x,
                 vecPoints[connections[i].b].y);
        }

        show(&screen);
        clear(&screen);
        input(&screen);
        fps_counter();
        SDL_Delay(10);
    }
    return 0;
}
