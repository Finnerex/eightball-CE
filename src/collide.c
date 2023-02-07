//I sure do love collisions, hopefully this is how they work

#include "collide.h"
#include <graphx.h>
#include <tice.h>
#include <math.h>
#include <stdbool.h>

void collideballs(ball_data* ball1, ball_data* ball2) {

    ball1->collided = true; ball2->collided = true;

    // change to local vars so they dont mutate the things outside
    float x1 = ball1->x; float x2 = ball2->x;
    float y1 = ball1->y; float y2 = ball2->y;
    float temp;

    // stole this from somewhere, have no idea how or why it might work
    x2 -= x1;
    y2 -= y1;
    x1 = x2 * x2 + y2 * y2, y1 = ball1->vx * x2 + ball1->vy * y2 - ball2->vx * x2 - ball2->vy * y2;
    x2 *= y1 / x1;
    y1 *= y2 / x1;

    // temp = ball1->vx - x2;

    // ball1->vy -= y1;
    // ball1->vx = ball2->vx + x2;
    // ball2->vy += y1;
    // ball2->vx = temp;

    ball1->vx -= x2;
    ball1->vy -= y1;
    ball2->vx += x2;
    ball2->vy += y1;

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

// im counting on you chat GPT 

float time_of_collision(ball_data* ball1, ball_data* ball2) {
    float distance = 64;
    float a = (-ball1->vx + ball2->vx) * (-ball1->vx + ball2->vx) + (-ball1->vy + ball2->vy) * (-ball1->vy + ball2->vy);
    float b = 2 * ((ball1->x - ball2->x) * (-ball1->vx + ball2->vx) + (ball1->y - ball2->y) * (-ball1->vy + ball2->vy));
    float c = (ball1->x - ball2->x) * (ball1->x - ball2->x) + (ball1->y - ball2->y) * (ball1->y - ball2->y) - distance;
    float d = b * b - 4 * a * c;

    if (b > 0 || d <= 0)
        return -1;

    float e = sqrt(d);
    float t1 = (-b - e) / (2 * a);
    float t2 = (-b + e) / (2 * a);

    if (t1 < 0 && t2 > 0 && b <= 0)
        return 0;

    return t1;
}



void prune_sweep(ball_data balls[16], float* time) {
    xyid s_balls[16];
    for (int i = 0; i < 16; i++) {
        s_balls[i].x = balls[i].x;
        s_balls[i].y = balls[i].y;
        s_balls[i].id = i;
    }

    // sort the balls by their x val
    qsort(s_balls, sizeof(s_balls)/sizeof(*s_balls), sizeof(*s_balls), sort_x);
    
    // check and execute collision
    for (int i = 0; i < 15; i++) {
        // if ((balls[s_balls[i].id].vx != 0 || balls[s_balls[i + 1].id].vx != 0 || balls[s_balls[i].id].vy != 0 || balls[s_balls[i + 1].id].vy != 0)
        // && (powf(s_balls[i].x - s_balls[i + 1].x, 2) + powf(s_balls[i].y - s_balls[i + 1].y, 2) <= 64)) {
        //     collideballs(&balls[s_balls[i].id], &balls[s_balls[i + 1].id]);
        // }
        int b1 = s_balls[i].id;
        int b2 = s_balls[i + 1].id;

        if (balls[b1].vx != 0 || balls[b2].vx != 0 || balls[b1].vy != 0 || balls[b2].vy != 0) {
            float t = time_of_collision(&balls[b1], &balls[b2]);
            
            if (t >= 0 && t <= 1) {
                // move balls to time of collision
                *time = t;
                balls[b1].x -= balls[b1].vx * t;
                balls[b1].y -= balls[b1].vy * t;
                balls[b2].x -= balls[b2].vx * t;
                balls[b2].y -= balls[b2].vy * t;
                // resolve collision
                collideballs(&balls[s_balls[i].id], &balls[s_balls[i + 1].id]);

                balls[b1].x -= balls[b1].vx * (1 - t);
                balls[b1].y -= balls[b1].vy * (1 - t);
                balls[b2].x -= balls[b2].vx * (1 - t);
                balls[b2].y -= balls[b2].vy * (1 - t);
            }
        }
    }

}

int sort_x(const void *a, const void *b) {
    xyid* b1 = (xyid *) a;
    xyid* b2 = (xyid *) b;

    if (b1->x < b2->x)
        return 1;
    else if (b1->x > b2->x)
        return -1;

    return 0;
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

bool ballcast(float x1, float y1, float x2, float y2, float cx, float cy, float r, float *i_x, float *i_y) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float fx = x1 - cx;
    float fy = y1 - cy;
    float a = dx*dx + dy*dy;
    float b = 2*fx*dx + 2*fy*dy;
    float c = fx*fx + fy*fy - r*r;
    float discriminant = b*b - 4*a*c;
    if (discriminant < 0) {
        return false;  // no intersection
    } else {
        float t1 = (-b + sqrtf(discriminant)) / (2*a);
        float t2 = (-b - sqrtf(discriminant)) / (2*a);
        float ix1 = x1 + t1*dx;
        float iy1 = y1 + t1*dy;
        float ix2 = x1 + t2*dx;
        float iy2 = y1 + t2*dy;

        if (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1) {
            // Two intersections
            float dist1 = sqrtf((x1 - ix1)*(x1 - ix1) + (y1 - iy1)*(y1 - iy1));
            float dist2 = sqrtf((x1 - ix2)*(x1 - ix2) + (y1 - iy2)*(y1 - iy2));
            if (dist1 < dist2) {
                *i_x = ix1;
                *i_y = iy1;
            } else {
                *i_x = ix2;
                *i_y = iy2;
            }
            return true;
        } else if (t1 >= 0 && t1 <= 1) {
            // One intersection
            *i_x = ix1;
            *i_y = iy1;
            return true;
        } else if (t2 >= 0 && t2 <= 1) {
            // One intersection
            *i_x = ix2;
            *i_y = iy2;
            return true;
        } else {
            return false;  // no intersection
        }
    }
}