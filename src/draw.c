#include "draw.h"
#include <tice.h>
#include <graphx.h>
#include <math.h>
#include "gfx/gfx.h"
#include "collide.h"

// rotated side sprites
uint8_t table_tr_data[table_tl_height * table_tl_width + 2] = {table_tl_height, table_tl_width,};
gfx_sprite_t* table_tr = (gfx_sprite_t*) table_tr_data;

uint8_t table_r_data[table_l_height * table_l_width + 2] = {table_l_width, table_l_height,};
gfx_sprite_t* table_r = (gfx_sprite_t*) table_r_data;

uint8_t table_br_data[table_tl_height * table_tl_width + 2] = {table_tl_height, table_tl_width,};
gfx_sprite_t* table_br = (gfx_sprite_t*) table_br_data;

uint8_t table_bl_data[table_tl_height * table_tl_width + 2] = {table_tl_height, table_tl_width,};
gfx_sprite_t* table_bl = (gfx_sprite_t*) table_bl_data;

// create the rotated sprites
void init_table() {
    gfx_RotateSpriteHalf(table_l, table_r);
    gfx_FlipSpriteY(table_tl, table_tr);
    gfx_RotateSpriteHalf(table_tl, table_br);
    gfx_RotateSpriteHalf(table_tr, table_bl);
}

void draw_table() {
    gfx_SetColor(2);
    gfx_FillRectangle_NoClip(table_l_width, table_tl_height, LCD_WIDTH - 2 * table_l_width, (LCD_HEIGHT - 2 * table_tl_height) - 77);
    gfx_Sprite_NoClip(table_l, 0, 0);
    gfx_Sprite_NoClip(table_tl, table_l_width, 0);
    gfx_Sprite_NoClip(table_tr, table_l_width + table_tl_width, 0);
    gfx_Sprite_NoClip(table_bl, table_l_width, (LCD_HEIGHT - table_tl_height) - 77);
    gfx_Sprite_NoClip(table_br, table_l_width + table_tl_width, (LCD_HEIGHT - table_tl_height) - 77);
    gfx_Sprite_NoClip(table_r, LCD_WIDTH - table_l_width, 0);
}

void draw_setup(ball_data balls[16], cue_data* cue) {
    float cos_of = cosf(cue->dir); float sin_of = sinf(cue->dir);
    float qbx = balls[15].x; float qby = balls[15].y;


    // cue
    gfx_SetColor(5);
    gfx_Line(qbx + cos_of * (10 + cue->pow/4), qby + sin_of * (10 + cue->pow/4), qbx + cos_of * (100 + cue->pow/4), qby + sin_of * (100 + cue->pow/4));

    // power bar
    gfx_SetColor(3);
    gfx_Rectangle_NoClip(LCD_WIDTH/2 - 51, LCD_HEIGHT - 10, 103, 5);
    gfx_SetColor(4);
    gfx_FillRectangle_NoClip(LCD_WIDTH/2 - 50, LCD_HEIGHT - 9, cue->pow + 1, 3);


    // cast bounce prediction
    gfx_SetColor(1);
    float i_x, i_y;
    float end_x = -1, end_y = -1;
    float min_distance;

    raycast(20, 20, LCD_WIDTH - 20, 20, qbx, qby, qbx + cos_of * -LCD_WIDTH, qby + sin_of * -LCD_WIDTH, &i_x, &i_y); // top edge
    raycast(20, 142, LCD_WIDTH - 20, 142, qbx, qby, qbx + cos_of * -LCD_WIDTH, qby + sin_of * -LCD_WIDTH, &i_x, &i_y); // bottom
    raycast(20, 20, 20, 142, qbx, qby, qbx + cos_of * -LCD_WIDTH, qby + sin_of * -LCD_WIDTH, &i_x, &i_y); // left
    raycast(LCD_WIDTH - 20, 20, LCD_WIDTH - 20, 142, qbx, qby, qbx + cos_of * -LCD_WIDTH, qby + sin_of * -LCD_WIDTH, &i_x, &i_y); // right

    end_x = i_x; end_y = i_y;
    min_distance = sqrtf(powf(qbx - end_x, 2) + powf(qby - end_y, 2));

    for (int i = 14; i >= 0; i--) {
        ballcast(qbx, qby, qbx + cos_of * -LCD_WIDTH, qby + sin_of * -LCD_WIDTH, balls[i].x, balls[i].y, 8, &i_x, &i_y);
        min_distance = closest_pos(i_x, i_y, qbx, qby, min_distance, &end_x, &end_y);
    }


    if (end_x < 0 || end_y < 0)
        return;

    // prediction line
    gfx_Line_NoClip(qbx + cos_of * -5, qby + sin_of * -5, end_x + cos_of * 5, end_y + sin_of * 4);
    gfx_Circle(end_x, end_y, 4);

}

float closest_pos(float x, float y, float o_x, float o_y, float min_distance, float* end_x, float* end_y) {
    float distance = sqrtf(powf(o_x - x, 2) + powf(o_y - y, 2));

    if (distance < min_distance) {
        *end_x = x;
        *end_y = y;
        return distance;
    }

    return min_distance;

}

void draw_players(gfx_sprite_t* player_1_type, bool is_player_1_turn) {
    gfx_SetColor(3);
    gfx_SetTextFGColor(3);
    gfx_PrintStringXY("Player 1", 8, TABLE_HEIGHT + 20);
    gfx_PrintStringXY("Player 2", LCD_WIDTH - 60, TABLE_HEIGHT + 20);

    if (player_1_type != NULL) {
        gfx_TransparentSprite_NoClip(player_1_type, 66, TABLE_HEIGHT + 20);
        gfx_TransparentSprite_NoClip((player_1_type == solid) ? stripe : solid, LCD_WIDTH - 72, TABLE_HEIGHT + 20);
    }

    if (is_player_1_turn)
        gfx_HorizLine_NoClip(8, TABLE_HEIGHT + 29, 54);
    else
        gfx_HorizLine_NoClip(LCD_WIDTH - 60, TABLE_HEIGHT + 29, 55);
    
}