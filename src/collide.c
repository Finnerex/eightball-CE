//I sure do love collisions, hopefully this is how they work

#include "collide.h"
#include <math.h>
#include <graphx.h>
#include <tice.h>

void collideballs(ball_data* ball1, ball_data* ball2) {

    //change to local vars so they dont mutate the things outside (probably a better way of doing this)
    float x1 = ball1->x; float x2 = ball2->x;
    float y1 = ball1->x; float y2 = ball2->y;
    float vx1 = ball1->v * cos(ball1->d); float vx2 = ball2->v * cos(ball2->d);
    float vy1 = ball1->v * sin(ball1->d); float vy2 = ball2->v * sin(ball2->d);

    //stole this from somewhere, have no idea how or why it might work
    x2 -= x1;
    y2 -= y1;
    x1 = x2 * x2 + y2 * y2, y1 = vx1 * x2 + vy1 * y2 - vx2 * x2 - vy2 * y2;
    x2 *= y1 / x1;
    vx1 -= x2;
    y1 *= y2 / x1;
    vy1 -= y1;
    vx2 += x2;
    vy2 += y1;

    ball1->v = sqrt(pow(vx1, 2) + pow(vy1, 2));
    ball2->v = sqrt(pow(vx2, 2) + pow(vy2, 2));

    ball1->d = atan(vy1 / vx1);
    ball2->d = atan(vy2 / vx2);
}

void collidewalls(ball_data* ball) {
    if (ball->x < 20) {
        ball->x = 21;
        ball->d -= PI;
    } else if (ball->x > LCD_WIDTH - 20){
        ball->x = LCD_WIDTH - 21;
        ball->d -= PI;
    }

    if (ball->y < 20) {
        ball->y = 21;
        ball->d = -ball->d;
    } else if (ball->y > 143) {
        ball->y = 142;
        ball->d = -ball->d;
    }
}
