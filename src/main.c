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
#include <ti/real.h>
#include "collide.h"
#include "gfx/gfx.h"

//rotated side sprites
uint8_t Table_r_data[Table_l_height * Table_l_width + 2] = {Table_l_width, Table_l_height,};
gfx_sprite_t* Table_r = (gfx_sprite_t*) Table_r_data;

uint8_t Table_br_data[Table_tr_height * Table_tr_width + 2] = {Table_tr_height, Table_tr_width,};
gfx_sprite_t* Table_br = (gfx_sprite_t*) Table_br_data;

uint8_t Table_bl_data[Table_tl_height * Table_tl_width + 2] = {Table_tl_height, Table_tl_width,};
gfx_sprite_t* Table_bl = (gfx_sprite_t*) Table_bl_data;

//queue power
int q_power = 0;

//list of all of the balls - ball_data struct in collide.h
ball_data balls[16];

//game states
enum gamestates{start, setup, animate, run};
int gamestate = setup;

//frame for animation of queue
int frame = 0;

//counter for ending run state
int zero_counter;

//speed multiplier for power and angle change
float speedmult = 1;

//queue starting pos
float q_dir = PI;

// acceleration
#define A 0.02

//basic funkies
void begin();
void end();
bool step();
void draw();



int main() {
    begin(); // No rendering allowed!

    gfx_Begin();
    gfx_SetPalette(global_palette, sizeof_global_palette, 0);

    gfx_SetDrawBuffer(); // Draw to the buffer to avoid rendering artifacts

    while (step()) { // No rendering allowed in step!
        draw(); // As little non-rendering logic as possible
        gfx_SwapDraw(); // Queue the buffered frame to be displayed
    }

    gfx_End();
    end();

    return 0;
}



void begin(void){
    //rotate edges
    gfx_RotateSpriteHalf(Table_l, Table_r);
    gfx_RotateSpriteHalf(Table_tl, Table_br);
    gfx_RotateSpriteHalf(Table_tr, Table_bl);

    //ball initilization
    gfx_sprite_t* initballsprite[16] = {stripe, solid, stripe, stripe, eightball, solid, stripe, solid, solid, stripe, solid, stripe, solid, stripe, solid, qball};
    float initballx[16] = {LCD_WIDTH/2 + Table_tr_width/2 + 10, initballx[0] + stripe_width, initballx[0] + stripe_width, initballx[2] + stripe_width, initballx[2] + stripe_width, initballx[2] + stripe_width, initballx[5] + stripe_width, initballx[5] + stripe_width, initballx[5] + stripe_width, initballx[5] + stripe_width, initballx[9] + stripe_width, initballx[9] + stripe_width, initballx[9] + stripe_width, initballx[9] + stripe_width, initballx[9] + stripe_width, Table_tl_width/2};
    float initbally[16] = {Table_l_height/2, initbally[0] - stripe_height/2, initbally[0] + stripe_height/2, initbally[0] - stripe_height, initbally[0], initbally[0] + stripe_height, initbally[0] - (5.0/3.0) * stripe_height + 2, initbally[0] - stripe_height/2, initbally[0] + stripe_height/2, initbally[0] + (5.0/3.0) * stripe_height - 1, initbally[0] - 2 * stripe_height, initbally[0] - stripe_height, initbally[0], initbally[0] + stripe_height, initbally[0] + 2 * stripe_height, Table_l_height/2};
    
    for (int i = 0; i < 16; i++) {
        balls[i].sprite = initballsprite[i];
        balls[i].x = initballx[i];
        balls[i].y = initbally[i];
        balls[i].vx = 0;
        balls[i].vy = 0;
    }

}

bool step(void) {
    kb_Scan();

    if (gamestate == setup) {

        //speed multiplier for power and angle
        if (kb_Data[1] & kb_2nd) {
            speedmult = 2;
        } else if (kb_Data[2] & kb_Math/*JUST FOR TESTING - change back to kb_Alpha*/) {
            speedmult = 0.5;
        } else {
            speedmult = 1;
        }

        //chnage angle
        if (kb_Data[7] & kb_Up) {
            q_dir += 0.04 * speedmult;
        }
        if (kb_Data[7] & kb_Down) {
            q_dir -= 0.04 * speedmult;
        }

        //change power with some funny logic so it doesnt go over or under
        if (kb_Data[7] & kb_Right && q_power < 100 - speedmult) {
            q_power += 2 * speedmult;
        } else if (kb_Data[7] & kb_Right && q_power < 100) {
            q_power = 100;
        }
        if (kb_Data[7] & kb_Left && q_power > 1 + speedmult) {
            q_power -= 2 * speedmult;
        } else if (kb_Data[7] & kb_Left && q_power > 0) {
            q_power = 0;
        }

        //ready
        if (kb_Data[6] & kb_Enter && q_power > 0) {
            frame = 0;
            gamestate = animate;
        }
    }

    if (gamestate == animate) {
        frame += 2; // I know this doesnt make sense but animation better

        if (frame > 40) {
            frame = 0;
            gamestate = run;
            balls[15].vx = cos(q_dir) * (q_power/5);
            balls[15].vy = sin(q_dir) * (q_power/5);
        }
    }

    if (gamestate == run && (kb_Data[6] & kb_Div)) {

        //test
        frame++;

        //init the counter to zero
        zero_counter = 0;
        
        //collisions and movement
        for (int i = 15; i >= 0; i--) {

            //movement
            balls[i].x -= balls[i].vx;
            balls[i].y -= balls[i].vy;

            //not using d does come with some downsides, also might be slow af
            real_t vy_over_vx = os_FloatToReal(balls[i].vy/balls[i].vx);
            real_t atan_of = os_RealAtanRad(&vy_over_vx);
            real_t cos_of = os_RealCosRad(&atan_of);
            real_t sin_of = os_RealSinRad(&atan_of);
            
            float ax = A * os_RealToFloat(&cos_of);
            float ay = A * os_RealToFloat(&sin_of);

            // if (balls[i].vx >= ax) { 
            //     balls[i].vx -= ax;
            // } else if (balls[i].vx <= -ax) {
            //     balls[i].vx += ax;
            // } else {
            //     balls[i].vx = 0;
            // }
            
            // if (balls[i].vy >= ay) {
            //     balls[i].vy -= ay;
            // } else if (balls[i].vy <= -ay) {
            //     balls[i].vy += ay;
            // } else {
            //     balls[i].vy = 0;
            // }

            collidewalls(&balls[i]);

            for (int j = 15; j >= 0; j--) {
                if (i != j) {
                    if(pow(balls[i].x - balls[j].x, 2) + pow(balls[i].y - balls[j].y, 2) <= 64 && (balls[i].vx != 0 || balls[j].vx != 0 || balls[i].vy != 0 || balls[j].vy != 0)) {
                        collideballs(&balls[i], &balls[j]);
                    }
                }
            }

            //increment the coumter if the x and y velocities are zero
            if (!(balls[i].vx || balls[i].vy)) {
                zero_counter ++;
            }
        }

        //if the counter it equal to the number of balls, end the run state
        if (zero_counter == 16) {
            gamestate = setup;
        }
    }

    if (kb_Data[6] & kb_Clear) {
        return false;
    }

    return true;
}



void draw(void) {
    //draw table
    gfx_SetColor(2);
    gfx_FillRectangle_NoClip(Table_l_width, Table_tr_height, LCD_WIDTH - 2 * Table_l_width, (LCD_HEIGHT - 2 * Table_tr_height) - 77);
    gfx_Sprite_NoClip(Table_l, 0, 0);
    gfx_Sprite_NoClip(Table_tl, Table_l_width, 0);
    gfx_Sprite_NoClip(Table_tr, Table_l_width + Table_tl_width, 0);
    gfx_Sprite_NoClip(Table_bl, Table_l_width, (LCD_HEIGHT - Table_tr_height) - 77);
    gfx_Sprite_NoClip(Table_br, Table_l_width + Table_tl_width, (LCD_HEIGHT - Table_tr_height) - 77);
    gfx_Sprite_NoClip(Table_r, LCD_WIDTH - Table_l_width, 0);

    //bottom background
    gfx_SetColor(1);
    gfx_FillRectangle_NoClip(0, Table_l_height, LCD_WIDTH, LCD_HEIGHT - Table_l_height);


    //draw balls
    for (int i = 15; i >= 0; i--) {
        gfx_TransparentSprite_NoClip(balls[i].sprite, balls[i].x - balls[i].sprite->width/2, balls[i].y - balls[i].sprite->height/2);
        //velocity vectors test
        //gfx_Line(balls[i].x, balls[i].y, balls[i].x - balls[i].vx, balls[i].y - balls[i].vy);
    }

    //test
    gfx_SetTextXY(10, 170);
    gfx_PrintString("test: ");
    gfx_PrintInt(zero_counter, 1);

    if (gamestate == setup) {
        //queue
        gfx_SetColor(5);
        gfx_Line(balls[15].x + cos(q_dir) * (10 + q_power/4), balls[15].y + sin(q_dir) * (10 + q_power/4), balls[15].x + cos(q_dir) * (100 + q_power/4), balls[15].y + sin(q_dir) * (100 + q_power/4));

        //power bar
        gfx_SetColor(3);
        gfx_Rectangle_NoClip(LCD_WIDTH/2 - 51, LCD_HEIGHT - 10, 103, 5);
        gfx_SetColor(4);
        gfx_FillRectangle_NoClip(LCD_WIDTH/2 - 50, LCD_HEIGHT - 9, q_power + 1, 3);
    }

    //animate queue
    if (gamestate == animate) {
        gfx_SetColor(5);
        gfx_Line(balls[15].x + cos(q_dir) * (30 + q_power/4 - frame), balls[15].y + sin(q_dir) * (30 + q_power/4 - frame), balls[15].x + cos(q_dir) * (120 + q_power/4 - frame), balls[15].y + sin(q_dir) * (120 + q_power/4 - frame));
    }
}



void end(void) {
    
}