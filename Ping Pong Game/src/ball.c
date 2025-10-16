#include "ball.h"
#include "lcd.h"
#include "touchpad.h"
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

// Параметры мяча
static float ball_xf = 160.0f;   
static float ball_yf = 240.0f;
static int16_t ball_radius = 12;

// Скорость мяча
#define BALL_SPEED 15.0f 

static float dx = 0.0f;//изменение координаты x за один кадр
static float dy = 0.0f;//изменение координаты y за один кадр

// флаг для начальной скорости
static uint8_t ball_initialized = 0;

//рандомное направление в начале
static void ball_init_velocity()
{
    // случайный угол в диапазоне -30, 30
    float ang_deg = (float)( (rand() % 60) - 30 );
    float ang = ang_deg * 3.1415926f / 180.0f;

    //мяч в другую сторону от платформы в начале
    dx = -BALL_SPEED * cosf(ang);
    dy =  BALL_SPEED * sinf(ang);

    // нормализуем
    float cur_speed = sqrtf(dx*dx + dy*dy);
    if (cur_speed > 0.0001f) {
        dx = dx * (BALL_SPEED / cur_speed);
        dy = dy * (BALL_SPEED / cur_speed);
    }

    ball_initialized = 1;
}

// функция обновления мяча
void update_ball(uint16_t BALL_COLOR, uint16_t BG_COLOR, Platform *platform)
{
    // инициализация скорости при первом вызове
    if (!ball_initialized) ball_init_velocity();

    // Стираем старый мяч 
    drawCircle((int)ball_xf, (int)ball_yf, ball_radius, BG_COLOR);

    // Обновляем позицию
    ball_xf += dx;
    ball_yf += dy;

    // Столкновение с верх/низом -> инвертируем dy
    if (ball_yf - ball_radius <= 0.0f) {
        ball_yf = ball_radius + 1.0f;
        dy = -dy;
    } else if (ball_yf + ball_radius >= 480.0f) {
        ball_yf = 480.0f - ball_radius - 1.0f;
        dy = -dy;
    }

    // Отскок от левой стены
    if (ball_xf - ball_radius <= 0.0f) {
        ball_xf = ball_radius + 1.0f;
        dx = fabsf(dx); // направляем вправо
    }

    // Проверка столкновения с платформой
    float plat_left   = (float)(platform->x);
    float plat_right  = (float)(platform->x + platform->width);
    float plat_top    = (float)(platform->y - platform->height / 2);
    float plat_bottom = (float)(platform->y + platform->height / 2);

    // простая AABB проверка с учетом радиуса 
    if ( (ball_xf + ball_radius >= plat_left) && (ball_xf - ball_radius <= plat_right) && (ball_yf + ball_radius >= plat_top) && (ball_yf - ball_radius <= plat_bottom) )
    {
        // рассчитываем относительную точку удара в диапазоне -1;1
        float rel = (ball_yf - (float)platform->y) / ((float)platform->height / 2.0f);
        if (rel > 1.0f) rel = 1.0f;
        if (rel < -1.0f) rel = -1.0f;

        // угол отскока в диапазоне -45;45
        float angle = rel * (3.1415926f / 4.0f);

        // сохраняем модуль скорости и пересчитываем dx/dy так, чтобы модуль = BALL_SPEED
        float speed = sqrtf(dx*dx + dy*dy);

        // мяч отскакивает влево от правой платформы
        dx = -speed * cosf(angle);
        dy =  speed * sinf(angle);

        // Подвинуть мяч чуть влево от платформы, чтобы избежать повторных коллизий
        ball_xf = plat_left - (float)ball_radius - 1.0f;
    }
    // Проверка проигрыша (касание нижней границы)
    if (ball_yf + ball_radius >= 480.0f) {
        //game_over = true; // фиксируем конец игры
        drawText(180, 34, "You lost!", 0x0000, 0xAAA0, 8);
    }
    // Ограничения экрана
    if (ball_xf + ball_radius > 320.0f) ball_xf = 320.0f - ball_radius;
    if (ball_yf < ball_radius) ball_yf = ball_radius;
    if (ball_yf > 480.0f - ball_radius) ball_yf = 480.0f - ball_radius;

    // Рисуем новый мяч (с округлением координат для draw)
    drawCircle((int)ball_xf, (int)ball_yf, ball_radius, BALL_COLOR);
}