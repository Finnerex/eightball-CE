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
    float v;
    float d;
} ball_data;

void collideballs(ball_data* ball1, ball_data* ball2);
void collidewalls(ball_data* ball);

#if __cplusplus
}
#endif

#endif