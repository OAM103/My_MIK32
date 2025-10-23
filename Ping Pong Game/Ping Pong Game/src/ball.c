#include "ball.h"
#include "lcd.h"
#include "brick.h"
#include "touchpad.h"
#include "brick.h"
#include <stdlib.h>
#include <math.h>

Ball ball;
uint8_t game_over = 0;

#define BALL_SPEED 15

static void ball_init_velocity(void) { //чтобы мяч не получал случайную сорость каждый раз
    float ang_deg = (float)((rand() % 60) - 30); //угол в градусах
    float ang = ang_deg * 3.1415926f / 180.0f; //угол в радианах

    ball.dx = -BALL_SPEED * cosf(ang); // направляем в лево, что бы не по прямой
    ball.dy =  BALL_SPEED * sinf(ang); 

    float speed = sqrtf(ball.dx*ball.dx + ball.dy*ball.dy); //нормализация скорости
    if (speed > 0.0001f) {
        ball.dx = ball.dx * (BALL_SPEED / speed);
        ball.dy = ball.dy * (BALL_SPEED / speed);
    }

    ball.initialized = 1;
}

void reset_ball(void) {
    ball.x = 160;
    ball.y = 240;
    ball.radius = 12;
    ball.dx = 0.0f;
    ball.dy = 0.0f;
    ball.initialized = 0;
}

void update_ball(uint16_t BALL_COLOR, uint16_t BG_COLOR, Platform *platform) {
    if (game_over==1 || bricks_live==0) {
        reset_ball();
        return;
    }

    if (!ball.initialized) ball_init_velocity();
    // Стираем старый мяч
    drawCircle(ball.x, ball.y, ball.radius, BG_COLOR);

    // Обновляем позицию
    ball.x += (int)ball.dx;
    ball.y += (int)ball.dy;

    // Столкновения с границами
    if (ball.y - ball.radius <= 0) { ball.y = ball.radius + 1; ball.dy = -ball.dy; }
    if (ball.y + ball.radius >= 480) { ball.y = 480 - ball.radius - 1; ball.dy = -ball.dy; }
    if (ball.x - ball.radius <= 0) { ball.x = ball.radius + 1; ball.dx = fabsf(ball.dx); }

    // Столкновение с платформой
    int plat_left   = platform->x;
    int plat_right  = platform->x + platform->width;
    int plat_top    = platform->y - platform->height / 2;
    int plat_bottom = platform->y + platform->height / 2;

    //AABB-пересечения
    if (ball.x + ball.radius >= plat_left &&
        ball.x - ball.radius <= plat_right &&
        ball.y + ball.radius >= plat_top &&
        ball.y - ball.radius <= plat_bottom) 
    {
        float rel = (ball.y - platform->y) / ((float)platform->height / 2.0f);// отражаем на угол, зависящий от касания платформы
        if (rel > 1.0f) rel = 1.0f;
        if (rel < -1.0f) rel = -1.0f;

        float angle = rel * (3.1415926f / 4.0f);
        float speed = sqrtf(ball.dx*ball.dx + ball.dy*ball.dy);

        ball.dx = -speed * cosf(angle);
        ball.dy =  speed * sinf(angle);

        ball.x = plat_left - ball.radius - 1; // чуть в сторону, чтобы без залипаний
    }

    // Столкновение c нижней стенкой -> конец игры
    if (ball.x + ball.radius > 320) game_over = 1;
    else drawCircle(ball.x, ball.y, ball.radius, BALL_COLOR);
}