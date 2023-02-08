#ifndef DRAW_H
#define DRAW_H
#include <graphx.h>
#include "gfx/gfx.h"
#include "collide.h"

#if __cplusplus
extern "C" {
#endif

void init_table(gfx_sprite_t* Table_r, gfx_sprite_t* Table_br, gfx_sprite_t* Table_bl);

void draw_table(gfx_sprite_t* Table_r, gfx_sprite_t* Table_br, gfx_sprite_t* Table_bl);

void draw_setup(ball_data balls[16], cue_data* queue);

#if __cplusplus
}
#endif

#endif