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

void draw_players(gfx_sprite_t* player_1_type, bool is_player_1_turn);

void draw_winning(int winning_player);

void draw_pocket_picking(int picked_pocket);

#if __cplusplus
}
#endif

#endif