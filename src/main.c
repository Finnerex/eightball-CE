/*
 *--------------------------------------
 * Program Name: 8ball
 * Author: Finnerex Dyer
 * License: me
 * Description: game for tieightyfourpluscegraphingcalculator
 *--------------------------------------
*/

#include <stdbool.h>
#include <math.h>
#include <tice.h>
#include <graphx.h>
#include <keypadc.h>
#include "collide.h"
#include "draw.h"
#include "gfx/gfx.h"

// cue
cue_data cue = {PI, 0}; // direction is pi, power is 0

// list of all of the balls - ball_data struct in collide.h
ball_data balls[16];

// game states
enum gamestates{start, setup, animate, run, scratch};
int gamestate = scratch;

bool start_of_setup = false;

// player turns
bool is_player_1_turn = true;
bool extra_turn = false;
gfx_sprite_t* player_1_type = NULL;

int winning_player = 0;

bool start_of_game = true;

// frame for animation of cue
int frame = 0;

// counter for ending run state
int num_stopped;

// speed multiplier for power and angle change
float speedmult = 1;


// basic funkies
void begin();
void end();
bool step();
void draw();

int main() {
    begin();                // No rendering allowed!

    gfx_Begin();
    gfx_SetPalette(global_palette, sizeof_global_palette, 0);

    gfx_SetDrawBuffer();    // Draw to the buffer to avoid rendering artifacts

    while (step()) {        // No rendering allowed in step!
        draw();             // As little non-rendering logic as possible
        gfx_SwapDraw();     // Queue the buffered frame to be displayed
    }

    gfx_End();
    end();

    return 0;
}



void begin(void){
    // initialize rotated table sprites
    init_table();

    // ball initilization
    gfx_sprite_t* init_ball_sprite[16] = {stripe, solid, stripe, stripe, eightball, solid, stripe, solid, solid, stripe, solid, stripe, solid, stripe, solid, qball};
    float init_ball_x[16] = {LCD_WIDTH/2 + table_tl_width/2 + 10, init_ball_x[0] + stripe_width, init_ball_x[0] + stripe_width, init_ball_x[2] + stripe_width, init_ball_x[2] + stripe_width, init_ball_x[2] + stripe_width, init_ball_x[5] + stripe_width, init_ball_x[5] + stripe_width, init_ball_x[5] + stripe_width, init_ball_x[5] + stripe_width, init_ball_x[9] + stripe_width, init_ball_x[9] + stripe_width, init_ball_x[9] + stripe_width, init_ball_x[9] + stripe_width, init_ball_x[9] + stripe_width, 85};
    float init_ball_y[16] = {table_l_height/2, init_ball_y[0] - stripe_height/2, init_ball_y[0] + stripe_height/2, init_ball_y[0] - stripe_height, init_ball_y[0], init_ball_y[0] + stripe_height, init_ball_y[0] - (5.0/3.0) * stripe_height + 2, init_ball_y[0] - stripe_height/2, init_ball_y[0] + stripe_height/2, init_ball_y[0] + (5.0/3.0) * stripe_height - 1, init_ball_y[0] - 2 * stripe_height, init_ball_y[0] - stripe_height, init_ball_y[0], init_ball_y[0] + stripe_height, init_ball_y[0] + 2 * stripe_height, table_l_height/2};
    
    for (int i = 0; i < 16; i++) {
        balls[i].sprite = init_ball_sprite[i];
        balls[i].x = init_ball_x[i];
        balls[i].y = init_ball_y[i];
        balls[i].vx = 0;
        balls[i].vy = 0;
        balls[i].collided = false;
        balls[i].pocketed = false;
    }

}

bool step(void) {
    kb_Scan();

    // quit
    if (kb_Data[6] & kb_Clear)
        return false;

    if (winning_player != 0)
        return true;

    if (gamestate == setup || gamestate == scratch) {
        // speed multiplier for power, angle, and cue ball movement
        if (kb_Data[1] & kb_2nd)
            speedmult = 4;
        
        else if (kb_Data[2] & kb_Math/*JUST FOR TESTING - change back to kb_Alpha*/)
            speedmult = 0.3;

        else
            speedmult = 1;
        
    }

    if (gamestate == setup) {

        if (start_of_setup) {

            if (balls[15].pocketed) { // scratch
                balls[15].x = 85;
                balls[15].y = 81;

                gamestate = scratch;
                balls[15].pocketed = false;
                extra_turn = false;
                
            }

            // if player sunk own ball, dont switch  
            if (!extra_turn)
                is_player_1_turn = !is_player_1_turn;
            else
                extra_turn = false;

            start_of_setup = false;
        }

        // change angle
        if (kb_Data[7] & kb_Up)
            cue.dir += 0.02 * speedmult;

        else if (kb_Data[7] & kb_Down)
            cue.dir -= 0.02 * speedmult;
        

        // restrict direction range to (0, 2pi)
        cue.dir -= (cue.dir > 2 * PI) ? 2 * PI : ((cue.dir < 0) ? -2 * PI : 0);

        // change power
        int pc = 2 * speedmult; // power change var
        // power clamping
        if (kb_Data[7] & kb_Right)
            cue.pow = (cue.pow + pc) > 100 ? 100 : (cue.pow + pc);

        else if (kb_Data[7] & kb_Left)
            cue.pow = (cue.pow - pc) < 0 ? 0 : (cue.pow - pc);

        // ready
        if (kb_Data[6] & kb_Enter && cue.pow > 0) {
            frame = 0;
            gamestate = animate;
        }
    }

    else if (gamestate == scratch) {

        if (kb_Data[7] & kb_Up)
            balls[15].y -= 0.5 * speedmult;

        else if (kb_Data[7] & kb_Down)
            balls[15].y += 0.5 * speedmult;
        

        if (kb_Data[7] & kb_Left)
            balls[15].x -= 0.5 * speedmult;
        
        else if (kb_Data[7] & kb_Right)
            balls[15].x += 0.5 * speedmult;

        collidewalls(&balls[15]);

        if (start_of_game && balls[15].x > 90)
            balls[15].x = 90;


        if (kb_Data[6] & kb_Enter) {
            bool change_state = true;

            for (int i = 0; i < 15; i++) { // check if this ball is overlapping
                if (sqrtf(powf(balls[15].x - balls[i].x, 2) + powf(balls[15].y - balls[i].y, 2)) < 8) {
                    change_state = false;
                    break;
                }
            }

            if (change_state) {
                gamestate = setup;
                cue.pow = 0;
                start_of_game = false;
            }
        }

    }

    else if (gamestate == animate) {
        frame ++;

        if (frame > 20) {
            frame = 0;
            gamestate = run;
            balls[15].vx = cosf(cue.dir) * (cue.pow/4 + 1);
            balls[15].vy = sinf(cue.dir) * (cue.pow/4 + 1);
        }
    }

    else if (gamestate == run) {
        // init the counter to zero
        num_stopped = 0;
        
        // detect collisions
        // prune_sweep(balls);
        not_prune_sweep(balls);

        for (int i = 0; i < 16; i++) {

            // movement
            if (!balls[i].collided) { // check if collided this frame because that changes things
                balls[i].x -= balls[i].vx;
                balls[i].y -= balls[i].vy;
            }

            check_pockets(&balls[i], &extra_turn, is_player_1_turn, &player_1_type, &winning_player);

            collidewalls(&balls[i]); // this jawn worked well enough without multiple per frame

            // do the decceleration
            float atan_of = atan2f(balls[i].vy, balls[i].vx);
            float ax = A * fabsf(cosf(atan_of));
            float ay = A * fabsf(sinf(atan_of));

            balls[i].vx = (balls[i].vx - ax >= 0) ? balls[i].vx - ax : (balls[i].vx + ax <= 0) ? balls[i].vx + ax : 0;
            balls[i].vy = (balls[i].vy - ay >= 0) ? balls[i].vy - ay : (balls[i].vy + ay <= 0) ? balls[i].vy + ay : 0;

            // increment the counter if the x and y velocities are zero
            if (!(balls[i].vx || balls[i].vy))
                num_stopped ++;

        }

        // if the counter it equal to the number of balls, end the run state
        if (num_stopped == 16) {
            gamestate = setup;
            start_of_setup = true;
        }

    }

    return true;
}

void draw(void) {
    // draw table
    draw_table();

    // bottom background
    gfx_SetColor(1);
    gfx_FillRectangle_NoClip(0, TABLE_HEIGHT, LCD_WIDTH, LCD_HEIGHT - TABLE_HEIGHT);

    // draw player stuff
    draw_players(player_1_type, is_player_1_turn);

    // draw balls
    for (int i = 0; i < 16; i++) {
        gfx_TransparentSprite_NoClip(balls[i].sprite, balls[i].x - balls[i].sprite->width/2, balls[i].y - balls[i].sprite->height/2);
        balls[i].collided = false;
    }

    if (gamestate == setup) {
        draw_setup(balls, &cue);
    }

    // animate cue
    else if (gamestate == animate) {
        gfx_SetColor(5);
        gfx_Line(balls[15].x + cosf(cue.dir) * (30 + cue.pow/4 - frame), balls[15].y + sinf(cue.dir) * (30 + cue.pow/4 - frame), balls[15].x + cosf(cue.dir) * (120 + cue.pow/4 - frame), balls[15].y + sinf(cue.dir) * (120 + cue.pow/4 - frame));
    }

    if (winning_player != 0) {
        draw_winning(winning_player);
    }
}


void end(void) {
    
}