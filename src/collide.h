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

typedef struct queue_data {
    float dir; // direction
    int pow; // power
} queue_data;

typedef struct xyid {
    int id;
    float x;
    float y;
} xyid;

void collideballs(ball_data* ball1, ball_data* ball2);
void collidewalls(ball_data* ball);

void prune_sweep(ball_data balls[16]);
int sort_x(const void *a, const void *b);

bool raycast(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y);
bool ballcast(float x1, float y1, float x2, float y2, float cx, float cy, float r, float *i_x, float *i_y);

#if __cplusplus
}
#endif

#endif