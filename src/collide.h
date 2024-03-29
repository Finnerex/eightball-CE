//I hate header files, but this one might be good

#ifndef COLLIDE_H
#define COLLIDE_H
#include <graphx.h>
#include <stdbool.h>

#define PI 3.14159265359
// acceleration
#define A 0.4

#define TABLE_HEIGHT 163

#if __cplusplus
extern "C" {
#endif

typedef struct ball_data {
    gfx_rletsprite_t* sprite;
    bool collided;
    bool pocketed;
    float x;
    float y;
    float vx;
    float vy;
} ball_data;

typedef struct cue_data {
    float dir; // direction
    int pow; // power
} cue_data;

void collide_balls(ball_data* ball1, ball_data* ball2);
void collide_walls(ball_data* ball);

void check_pockets(ball_data* ball, bool* next_turn, bool is_player_1_turn, gfx_rletsprite_t** player_1_type, int* winning_player, 
                    int* num_solids, int* num_stripes, int picked_pocket, bool win_attempt);

float time_of_collision(ball_data *ball1, ball_data *ball2);

void prune_sweep(ball_data balls[16]);

void not_prune_sweep(ball_data balls[16]);

int sort_x(const void *a, const void *b);

bool raycast(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y);
bool ballcast(float bx, float by, float cx, float cy, cue_data* cue, float* i_x, float* i_y);

#if __cplusplus
}
#endif

#endif