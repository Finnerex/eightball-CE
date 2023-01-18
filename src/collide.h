//I hate header files, but this one might be good

#ifndef COLLIDE_H
#define COLLIDE_H
#define PI 3.14159265359
#include <graphx.h>

#if __cplusplus
extern "C" {
#endif

typedef struct ball_data {
    gfx_sprite_t* sprite;
    float x;
    float y;
    float vx;
    float vy;
} ball_data;

typedef struct xyid {
    int id;
    float x;
    float y;
} xyid;

void collideballs(ball_data* ball1, ball_data* ball2);
void collidewalls(ball_data* ball);

void prune_sweep(ball_data balls[16]);
int sort_x(const void *a, const void *b);

#if __cplusplus
}
#endif

#endif