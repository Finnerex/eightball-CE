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
#include <fileioc.h>
#include "collide.h"
#include "draw.h"
#include "gfx/gfx.h"

#define SAVE_APPVAR_NAME ("BALL8CES")

// cue
cue_data cue = {PI, 0}; // direction is pi, power is 0

// list of all of the balls - ball_data struct in collide.h
ball_data balls[16];

// game states
enum game_states{start, setup, animate, run, scratch, pick_pocket, pick_state};
int game_state = scratch;

int picked_pocket = 0;
bool win_attempt = false;

int num_stripes = 7;
int num_solids = 7;

// player turns
bool is_player_1_turn = true;
bool should_change_turn = false;
gfx_rletsprite_t* player_1_type = NULL;

int winning_player = 0;

bool start_of_game = true;

// frame for animation of cue
int frame = 0;

// counter for ending run state
int num_stopped;

// speed multiplier for power and angle change
float speed_mult = 1;


// basic funkies
void begin();
void end();
bool step();
void draw();
void try_load_settings(void);
void save_settings(void);
void clear_settings(void);

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
    gfx_rletsprite_t* init_ball_sprite[16] = {stripe, solid, stripe, stripe, eightball, solid, stripe, solid, solid, stripe, solid, stripe, solid, stripe, solid, qball};
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
    
    try_load_settings();
}

bool step(void) {
    kb_Scan();

    // quit
    if (kb_Data[1] & kb_Del || ((kb_Data[6] & kb_Clear) && winning_player != 0)) { // if someone has won, dont save
        clear_settings();
        return false;

    } else if (kb_Data[6] & kb_Clear) {
        save_settings();
        return false;
    } 
    

    if (winning_player != 0)
        return true;

    if (game_state == pick_state) {

        if (win_attempt) {
            game_state = pick_pocket;

            if (should_change_turn) {
                is_player_1_turn = !is_player_1_turn;
                should_change_turn = false;
            }

        } else if (balls[15].pocketed) {
            game_state = scratch;

            balls[15].pocketed = false;
            balls[15].x = 85;
            balls[15].y = TABLE_HEIGHT / 2;

            is_player_1_turn = !is_player_1_turn;
            should_change_turn = false;

        } else {
            game_state = setup;

            win_attempt = (should_change_turn ? !is_player_1_turn : is_player_1_turn) ?
            (player_1_type == solid ? num_solids == 0 : num_stripes == 0) :
            (player_1_type == stripe ? num_solids == 0 : num_stripes == 0);

            if (should_change_turn) {
                is_player_1_turn = !is_player_1_turn;
            }

            should_change_turn = true;
        }
        
        
    }

    if (game_state == setup || game_state == scratch) {
        // speed multiplier for power, angle, and cue ball movement
        if (kb_Data[1] & kb_2nd)
            speed_mult = 4;
        
        else if (kb_Data[2] & kb_Alpha)
            speed_mult = 0.2f;

        else
            speed_mult = 1;
    }

    if (game_state == setup) {

        // change angle
        if (kb_Data[7] & kb_Up)
            cue.dir += 0.02f * speed_mult;

        else if (kb_Data[7] & kb_Down)
            cue.dir -= 0.02f * speed_mult;
        

        // restrict direction range to (0, 2pi)
        cue.dir -= (cue.dir > 2 * PI) ? 2 * PI : ((cue.dir < 0) ? -2 * PI : 0);

        // change power
        int pc = 2 * (speed_mult + 0.3f); // power change var
        // power clamping
        if (kb_Data[7] & kb_Right)
            cue.pow = (cue.pow + pc) > 100 ? 100 : (cue.pow + pc);

        else if (kb_Data[7] & kb_Left)
            cue.pow = (cue.pow - pc) < 0 ? 0 : (cue.pow - pc);

        // ready
        if (kb_Data[6] & kb_Enter && cue.pow > 0) {
            frame = 0;
            game_state = animate;
        }
    }

    else if (game_state == scratch) {

        if (kb_Data[7] & kb_Up)
            balls[15].y -= 0.5f * speed_mult;

        else if (kb_Data[7] & kb_Down)
            balls[15].y += 0.5f * speed_mult;
        

        if (kb_Data[7] & kb_Left)
            balls[15].x -= 0.5f * speed_mult;
        
        else if (kb_Data[7] & kb_Right)
            balls[15].x += 0.5f * speed_mult;

        collide_walls(&balls[15]);

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
                game_state = pick_state;
                cue.pow = 0;
                start_of_game = false;
            }
        }

    }

    else if (game_state == pick_pocket) {

        static bool lr, ll, lu, ld, r, l, u, d;
        r = kb_Data[7] & kb_Right;
        l = kb_Data[7] & kb_Left;
        u = kb_Data[7] & kb_Up;
        d = kb_Data[7] & kb_Down;

        if ((!r && lr) && picked_pocket < 5)
            picked_pocket ++;
        else if ((!l && ll) && picked_pocket > 0)
            picked_pocket --;
        else if ((!d && ld) && picked_pocket < 3)
            picked_pocket += 3;
        else if ((!u && lu) && picked_pocket > 2)
            picked_pocket -= 3;

        lr = r;
        ll = l;
        lu = u;
        ld = d;

        if (kb_Data[6] & kb_Enter) {
            game_state = pick_state;
            cue.pow = 0;
            win_attempt = false;
        }

    }

    else if (game_state == animate) {
        frame ++;

        if (frame > 20) {
            frame = 0;
            game_state = run;
            balls[15].vx = cosf(cue.dir) * (cue.pow/4 + 1);
            balls[15].vy = sinf(cue.dir) * (cue.pow/4 + 1);
        }
    }

    else if (game_state == run) {
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

            check_pockets(&balls[i], &should_change_turn, is_player_1_turn, &player_1_type, &winning_player, &num_solids, &num_stripes, picked_pocket, win_attempt);

            collide_walls(&balls[i]); // this jawn worked well enough without multiple per frame

            // do the deceleration
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
            game_state = pick_state;

            // dont mind this
            win_attempt = (should_change_turn ? !is_player_1_turn : is_player_1_turn) ?
            (player_1_type == solid ? num_solids == 0 : num_stripes == 0) :
            (player_1_type == stripe ? num_solids == 0 : num_stripes == 0);

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
        gfx_RLETSprite_NoClip(balls[i].sprite, balls[i].x - balls[i].sprite->width/2, balls[i].y - balls[i].sprite->height/2);
        balls[i].collided = false;
    }

    if (game_state == setup) {
        draw_setup(balls, &cue);
    }

    // animate cue
    else if (game_state == animate) {
        gfx_SetColor(5);
        gfx_Line(balls[15].x + cosf(cue.dir) * (30 + cue.pow/4 - frame), balls[15].y + sinf(cue.dir) * (30 + cue.pow/4 - frame), balls[15].x + cosf(cue.dir) * (120 + cue.pow/4 - frame), balls[15].y + sinf(cue.dir) * (120 + cue.pow/4 - frame));
    }

    else if (game_state == pick_pocket) {
        draw_pocket_picking(picked_pocket);
    }

    if (winning_player != 0) {
        draw_winning(winning_player);
    }
}

void end(void) {

}

typedef struct {
    enum {
        SERIALIZED_BALL_SPRITE_SOLID,
        SERIALIZED_BALL_SPRITE_STRIPE,
        SERIALIZED_BALL_SPRITE_EIGHTBALL
    } sprite;
    bool collided;
    bool pocketed;
    float x;
    float y;
    float vx;
    float vy;
} serialized_ball_data_t;

typedef struct {
    int version;

    cue_data cue;
    serialized_ball_data_t balls[16];

    int game_state;

    int picked_pocket;
    bool win_attempt;

    int num_stripes;
    int num_solids;

    // player turns
    bool is_player_1_turn;
    bool should_change_turn;
    enum {
        PLAYER_1_TYPE_NONE,
        PLAYER_1_TYPE_SOLID,
        PLAYER_1_TYPE_STRIPE,
    } player_1_type;

    int winning_player;

    bool start_of_game;
    int frame;
    int num_stopped;
} game_settings;

void unserialize_ball(serialized_ball_data_t* in, ball_data* out) {
    // unserialize sprite
    switch (in->sprite) {
        case SERIALIZED_BALL_SPRITE_SOLID:
            out->sprite = solid;
            break;
        case SERIALIZED_BALL_SPRITE_STRIPE:
            out->sprite = stripe;
            break;
        case SERIALIZED_BALL_SPRITE_EIGHTBALL:
            out->sprite = eightball;
            break;
    }

    // everything else
    out->collided = in->collided;
    out->pocketed = in->pocketed;
    out->x = in->x;
    out->y = in->y;
    out->vx = in->vx;
    out->vy = in->vy;
}

void serialize_ball(ball_data* in, serialized_ball_data_t* out) {
    // serialize sprite
    if (in->sprite == solid)
        out->sprite = SERIALIZED_BALL_SPRITE_SOLID;

    else if (in->sprite == stripe)
        out->sprite = SERIALIZED_BALL_SPRITE_STRIPE;

    else if (in->sprite == eightball)
        out->sprite = SERIALIZED_BALL_SPRITE_EIGHTBALL;
    
    
    // everything else
    out->collided = in->collided;
    out->pocketed = in->pocketed;
    out->x = in->x;
    out->y = in->y;
    out->vx = in->vx;
    out->vy = in->vy;
}

void try_load_settings(void) {
    uint8_t file_handle;

    // open file
    // if file not exist then skip this function
    if (!(file_handle = ti_Open(SAVE_APPVAR_NAME, "r")))
        return;
    
    // do stuff

    // variable to hold settings
    game_settings loaded_settings;

    // load data to the variable
    ti_Read(&loaded_settings, sizeof(game_settings), 1, file_handle);

    // exit if the version is wrong
    if (loaded_settings.version != 0) {
        ti_Close(file_handle);
        return;
    }

    // save the settings back
    cue = loaded_settings.cue;
    for (int i = 0; i < 16; i++) {
        unserialize_ball(&loaded_settings.balls[i], &balls[i]);
    }

    game_state = loaded_settings.game_state;

    picked_pocket = loaded_settings.picked_pocket;
    win_attempt = loaded_settings.win_attempt;

    num_stripes = loaded_settings.num_stripes;
    num_solids = loaded_settings.num_solids;

    is_player_1_turn = loaded_settings.is_player_1_turn;
    should_change_turn = loaded_settings.should_change_turn;

    //player_1_type = loaded_settings.player_1_type == PLAYER_1_TYPE_SOLID ? solid : loaded_settings.player_1_type == PLAYER_1_TYPE_STRIPE ? stripe : NULL;
    switch (loaded_settings.player_1_type) {
        case PLAYER_1_TYPE_NONE:
            player_1_type = NULL;
            break;
        case PLAYER_1_TYPE_SOLID:
            player_1_type = solid;
            break;
        case PLAYER_1_TYPE_STRIPE:
            player_1_type = stripe;
            break;
    }

    /*if (loaded_settings.player_1_type == PLAYER_1_TYPE_NONE)
        player_1_type = NULL;
    else if (loaded_settings.player_1_type == PLAYER_1_TYPE_SOLID)
        player_1_type = solid;
    else if (loaded_settings.player_1_type == PLAYER_1_TYPE_STRIPE)
        player_1_type = stripe;*/

    winning_player = loaded_settings.winning_player;

    start_of_game = loaded_settings.start_of_game;
    frame = loaded_settings.frame;
    num_stopped = loaded_settings.num_stopped;

    // close file
    ti_Close(file_handle);
}

void save_settings(void) {
    game_settings saved_settings;

    saved_settings.version = 0;

    // save each setting
    saved_settings.cue = cue;
    for (int i = 0; i < 16; i++) {
        serialize_ball(&balls[i], &saved_settings.balls[i]);
    }

    saved_settings.game_state = game_state;

    saved_settings.picked_pocket = picked_pocket;
    saved_settings.win_attempt = win_attempt;

    saved_settings.num_stripes = num_stripes;
    saved_settings.num_solids = num_solids;

    // player turns
    saved_settings.is_player_1_turn = is_player_1_turn;
    saved_settings.should_change_turn = should_change_turn;
    saved_settings.player_1_type = player_1_type == solid ? PLAYER_1_TYPE_SOLID : player_1_type == stripe ? PLAYER_1_TYPE_STRIPE : PLAYER_1_TYPE_NONE;
    /*if (player_1_type == NULL)
        saved_settings.player_1_type = PLAYER_1_TYPE_NONE;
    else if (player_1_type == solid)
        saved_settings.player_1_type = PLAYER_1_TYPE_SOLID;
    else if (player_1_type == stripe)
        saved_settings.player_1_type = PLAYER_1_TYPE_STRIPE;*/

    saved_settings.winning_player = winning_player;

    saved_settings.start_of_game = start_of_game;
    saved_settings.frame = frame;
    saved_settings.num_stopped = num_stopped;

    // save to a file
    uint8_t file_handle;
    if (!(file_handle = ti_Open(SAVE_APPVAR_NAME, "w")))
        return;

    
    ti_Write(&saved_settings, sizeof(game_settings), 1, file_handle);
    ti_Close(file_handle);
}

void clear_settings(void) {
    ti_Delete(SAVE_APPVAR_NAME);
}
