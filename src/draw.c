#include "draw.h"
#include <tice.h>
#include <graphx.h>
#include <math.h>
#include "gfx/gfx.h"
#include "collide.h"

// create the rotated sprites
void init_table(gfx_sprite_t* Table_r, gfx_sprite_t* Table_br, gfx_sprite_t* Table_bl) {
    gfx_RotateSpriteHalf(Table_l, Table_r);
    gfx_RotateSpriteHalf(Table_tl, Table_br);
    gfx_RotateSpriteHalf(Table_tr, Table_bl);
}

void draw_table(gfx_sprite_t* Table_r, gfx_sprite_t* Table_br, gfx_sprite_t* Table_bl) {
    gfx_SetColor(2);
    gfx_FillRectangle_NoClip(Table_l_width, Table_tr_height, LCD_WIDTH - 2 * Table_l_width, (LCD_HEIGHT - 2 * Table_tr_height) - 77);
    gfx_Sprite_NoClip(Table_l, 0, 0);
    gfx_Sprite_NoClip(Table_tl, Table_l_width, 0);
    gfx_Sprite_NoClip(Table_tr, Table_l_width + Table_tl_width, 0);
    gfx_Sprite_NoClip(Table_bl, Table_l_width, (LCD_HEIGHT - Table_tr_height) - 77);
    gfx_Sprite_NoClip(Table_br, Table_l_width + Table_tl_width, (LCD_HEIGHT - Table_tr_height) - 77);
    gfx_Sprite_NoClip(Table_r, LCD_WIDTH - Table_l_width, 0);
}

void draw_setup(ball_data balls[16], float q_dir, int q_power) {
    // queue
    gfx_SetColor(5);
    gfx_Line(balls[15].x + cosf(q_dir) * (10 + q_power/4), balls[15].y + sinf(q_dir) * (10 + q_power/4), balls[15].x + cosf(q_dir) * (100 + q_power/4), balls[15].y + sinf(q_dir) * (100 + q_power/4));

    // power bar
    gfx_SetColor(3);
    gfx_Rectangle_NoClip(LCD_WIDTH/2 - 51, LCD_HEIGHT - 10, 103, 5);
    gfx_SetColor(4);
    gfx_FillRectangle_NoClip(LCD_WIDTH/2 - 50, LCD_HEIGHT - 9, q_power + 1, 3);
}