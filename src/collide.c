//I sure do love collisions, hopefully this is how they work

#include "collide.h"
#include <graphx.h>
#include <tice.h>

void collideballs(ball_data* ball1, ball_data* ball2) {

    //change to local vars so they dont mutate the things outside (probably a better way of doing this)
    float x1 = ball1->x; float x2 = ball2->x;
    float y1 = ball1->x; float y2 = ball2->y;
    float temp;

    //stole this from somewhere, have no idea how or why it might work
    x2 -= x1;
    y2 -= y1;
    x1 = x2 * x2 + y2 * y2, y1 = ball1->vx * x2 + ball1->vy * y2 - ball2->vx * x2 - ball2->vy * y2;
    x2 *= y1 / x1;
    y1 *= y2 / x1;

    //an attempt at collision correction /// didnt fucking work who would ave guessed
    ball1->x += ball1->vx/2; ball1->y += ball1->vy/2;
    ball2->x += ball2->vx/2; ball2->y += ball2->vy/2;

    temp = ball1->vx - x2;

    ball1->vy -= y1;
    ball1->vx = ball2->vx + x2;
    ball2->vy += y1;
    ball2->vx = temp;

}

void collidewalls(ball_data* ball) {
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
    } else if (ball->y > 143) {
        ball->y = 143;
        ball->vy = -ball->vy;
    }
}
