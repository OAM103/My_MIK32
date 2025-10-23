#ifndef BRICK_H
#define BRICK_H

#include <stdint.h>
#include <stdbool.h>

// параметры сетки
#define BRICK_ROWS 8
#define BRICK_COLS 5

// структура кирпича
typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    uint16_t color;
    bool active;
} Brick;

// массив всех кирпичей
extern Brick bricks[BRICK_ROWS][BRICK_COLS];
extern uint8_t bricks_live;

// функции управления кирпичами
void init_bricks(void);
void draw_bricks(void);
void destroy_brick(uint8_t row, uint8_t col, uint16_t BG_COLOR);
uint8_t check_ball_collision_with_bricks(float ball_x, float ball_y, float radius, float *dx, float *dy, uint16_t BG_COLOR);

#endif