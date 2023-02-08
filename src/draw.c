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

void draw_setup(ball_data balls[16], cue_data* cue) {
    float cos_of = cosf(cue->dir); float sin_of = sinf(cue->dir);
    float qbx = balls[15].x; float qby = balls[15].y;
    // cue
    gfx_SetColor(5);
    gfx_Line(qbx + cos_of * (10 + cue->pow/4), qby + sin_of * (10 + cue->pow/4), qbx + cos_of * (100 + cue->pow/4), qby + sin_of * (100 + cue->pow/4));

    // cast bounce prediction
    gfx_SetColor(1);
    float i_x, i_y;

    raycast(20, 20, LCD_WIDTH - 20, 20, qbx, qby, qbx + cos_of * -LCD_WIDTH, qby + sin_of * -LCD_WIDTH, &i_x, &i_y);//top edge
    raycast(20, 142, LCD_WIDTH - 20, 142, qbx, qby, qbx + cos_of * -LCD_WIDTH, qby + sin_of * -LCD_WIDTH, &i_x, &i_y);//bottom
    raycast(20, 20, 20, 142, qbx, qby, qbx + cos_of * -LCD_WIDTH, qby + sin_of * -LCD_WIDTH, &i_x, &i_y);//left
    raycast(LCD_WIDTH - 20, 20, LCD_WIDTH - 20, 142, qbx, qby, qbx + cos_of * -LCD_WIDTH, qby + sin_of * -LCD_WIDTH, &i_x, &i_y);//right

    for (int i = 14; i >= 0; i--) {
        //raycast(balls[i].x + sin_of * 4, balls[i].y - cos_of * 4, balls[i].x - sin_of * 4, balls[i].y + cos_of * 4, qbx, qby, qbx + cos_of * -LCD_WIDTH, qby + sin_of * -LCD_WIDTH, &i_x, &i_y);
        //gfx_Line((balls[i].x + 4 * cos_of) - (sin_of * 1), (balls[i].y + 4 * sin_of) + (cos_of * 1), (balls[i].x + 4 * cos_of) + (sin_of * 1), (balls[i].y + 4 * sin_of) - (cos_of * 1));
        ballcast(qbx, qby, qbx + cos_of * -LCD_WIDTH, qby + sin_of * -LCD_WIDTH, balls[i].x, balls[i].y, 8, &i_x, &i_y);
        //gfx_Line(balls[i].x - (sin_of * 4), balls[i].y + (cos_of * 4), balls[i].x + (sin_of * 4), balls[i].y - (cos_of * 4));
    }

    gfx_Line_NoClip(qbx + cos_of * -5, qby + sin_of * -5, i_x + cos_of * 5, i_y + sin_of * 4);
    gfx_Circle(i_x, i_y, 4);

    // gfx_SetColor(0);
    // gfx_Line(qbx, qby, qbx + cos_of * -LCD_WIDTH, qby + sin_of * -LCD_WIDTH);

    // power bar
    gfx_SetColor(3);
    gfx_Rectangle_NoClip(LCD_WIDTH/2 - 51, LCD_HEIGHT - 10, 103, 5);
    gfx_SetColor(4);
    gfx_FillRectangle_NoClip(LCD_WIDTH/2 - 50, LCD_HEIGHT - 9, cue->pow + 1, 3);
}