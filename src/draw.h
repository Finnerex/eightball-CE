#ifndef DRAW_H
#define DRAW_H
#include <graphx.h>
#include "gfx/gfx.h"
#include "collide.h"

#if __cplusplus
extern "C" {
#endif

void init_table();

void draw_table();

float closest_pos(float x, float y, float o_x, float o_y, float min_distance, float* end_x, float* end_y);

void draw_setup(ball_data balls[16], cue_data* queue);

#if __cplusplus
}
#endif

#endif