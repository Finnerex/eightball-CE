//I sure do love collisions, hopefully this is how they work

#include "collide.h"
#include "gfx/gfx.h"
#include <graphx.h>
#include <tice.h>
#include <math.h>
#include <stdbool.h>

void collide_balls(ball_data* ball1, ball_data* ball2) {

    ball1->collided = true; ball2->collided = true;

    // change to local vars so they dont mutate the things outside
    float x1 = ball1->x; float x2 = ball2->x;
    float y1 = ball1->y; float y2 = ball2->y;

    // stole this from somewhere, have no idea how or why it might work
    x2 -= x1;
    y2 -= y1;
    x1 = x2 * x2 + y2 * y2, y1 = ball1->vx * x2 + ball1->vy * y2 - ball2->vx * x2 - ball2->vy * y2;
    x2 *= y1 / x1;
    y1 *= y2 / x1;

    ball1->vx -= x2;
    ball1->vy -= y1;
    ball2->vx += x2;
    ball2->vy += y1;

}

void check_pockets(ball_data* ball, bool* should_change_turn, bool is_player_1_turn, gfx_rletsprite_t** player_1_type, int* winning_player, 
                    int* num_solids, int* num_stripes, int picked_pocket, bool win_attempt) {

    static const int pocket_x[] = {14, LCD_WIDTH / 2, LCD_WIDTH - 14, 14,                LCD_WIDTH / 2,     LCD_WIDTH - 14};
    static const int pocket_y[] = {14, 10,            14,             TABLE_HEIGHT - 14, TABLE_HEIGHT - 10, TABLE_HEIGHT - 14};
    static int next_pocketed_x = 8;
    
    for (int i = 0; i < 6; i++) {
        if (sqrtf(powf(ball->x - pocket_x[i], 2) + powf(ball->y - pocket_y[i], 2)) < 12) {

            ball->pocketed = true;

            ball->vx = 0;
            ball->vy = 0;

            if (ball->sprite == eightball) {

                ball->x = LCD_WIDTH / 2;
                ball->y = TABLE_HEIGHT + 16;

                *winning_player = is_player_1_turn ? 2 : 1; // the player who didnt sink the eightball wins in default state

                if (win_attempt && i == picked_pocket) {
                    if (*num_solids == 0 && *num_stripes == 0)
                        *winning_player = is_player_1_turn ? 1 : 2;
                        
                    else if ((*num_solids == 0 && *player_1_type == solid) || (*num_stripes == 0 && *player_1_type == stripe))
                        *winning_player = 1;

                    else if ((*num_solids == 0 && *player_1_type == stripe) || (*num_stripes == 0 && *player_1_type == solid))
                        *winning_player = 2;
                }

            } else if (ball->sprite == qball) {
            
                ball->x = LCD_WIDTH - 10;
                ball->y = TABLE_HEIGHT + 10;
            
            } else {

                if (*player_1_type == NULL)
                    *player_1_type = (is_player_1_turn) ? ball->sprite : (ball->sprite == solid) ? stripe : solid;

                if ((*player_1_type == ball->sprite && is_player_1_turn) || (*player_1_type != ball->sprite && !is_player_1_turn))
                    *should_change_turn = false;
                else
                    *should_change_turn = true;


                if (ball->sprite == stripe)
                    --*num_stripes;
                else
                    --*num_solids;

                ball->x = next_pocketed_x;
                ball->y = TABLE_HEIGHT + 8;

                next_pocketed_x += 8;
            }

        }
    }
}

void collide_walls(ball_data* ball) {

    if (ball->pocketed)
        return;

    if (ball->x < 20) {
        ball->x = 20;
        ball->vx = -ball->vx;
    } else if (ball->x > LCD_WIDTH - 20){
        ball->x = LCD_WIDTH - 20;
        ball->vx = -ball->vx;
    }

    if (ball->y < 20) {
        ball->y = 20;
        ball->vy = -ball->vy;
    } else if (ball->y > TABLE_HEIGHT - 20) {
        ball->y = TABLE_HEIGHT - 20;
        ball->vy = -ball->vy;
    }
}

float time_of_collision(ball_data* ball1, ball_data* ball2) {
    float a = (-ball1->vx + ball2->vx) * (-ball1->vx + ball2->vx) + (-ball1->vy + ball2->vy) * (-ball1->vy + ball2->vy);
    float b = 2 * ((ball1->x - ball2->x) * (-ball1->vx + ball2->vx) + (ball1->y - ball2->y) * (-ball1->vy + ball2->vy));
    float c = (ball1->x - ball2->x) * (ball1->x - ball2->x) + (ball1->y - ball2->y) * (ball1->y - ball2->y) - 64;
    float d = b * b - 4 * a * c;

    if (b > 0 || d <= 0)
        return -1;

    float e = sqrtf(d);
    float t1 = (-b - e) / (2 * a);
    float t2 = (-b + e) / (2 * a);

    if (t1 < 0 && t2 > 0 && b <= 0)
        return 0;

    return t1;
}

void not_prune_sweep(ball_data balls[16]) {
    for (int i = 0; i < 16; i++) {

        for (int j = 0; j < 16; j++) {


            if (i != j && (balls[i].vx != 0 || balls[j].vx != 0 || balls[i].vy != 0 || balls[j].vy != 0)) {
                float t = time_of_collision(&balls[i], &balls[j]);

                if (t >= 0 && t <= 1) {
                    // move balls to time of collision
                    balls[i].x -= balls[i].vx * t;
                    balls[i].y -= balls[i].vy * t;
                    balls[j].x -= balls[j].vx * t;
                    balls[j].y -= balls[j].vy * t;

                    // resolve collision
                    collide_balls(&balls[i], &balls[j]);

                    balls[i].x -= balls[i].vx * (1 - t);
                    balls[i].y -= balls[i].vy * (1 - t);
                    balls[j].x -= balls[j].vx * (1 - t);
                    balls[j].y -= balls[j].vy * (1 - t);
                }
            }
        }

    }
}

bool raycast(float p0_x, float p0_y, float p1_x, float p1_y, 
    float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y) {
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

    float s, t;
    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        if (i_x != NULL)
            *i_x = p0_x + (t * s1_x);
        if (i_y != NULL)
            *i_y = p0_y + (t * s1_y);
        return true;
    }

    return false; // No collision
}

bool ballcast(float bx, float by, float cx, float cy, cue_data* cue, float* i_x, float* i_y) {
    float vx = cosf(cue->dir); float vy = sinf(cue->dir);

    ball_data cue_ball = { NULL, false, false, cx, cy, vx, vy }; // times ten bc idk
    ball_data ball2 = { NULL, false, false, bx, by, 0, 0 };

    float t = time_of_collision(&cue_ball, &ball2);

    if (t < 0)
        return false;

    *i_x = cx + vx * -t;
    *i_y = cy + vy * -t;

    return true;

}
