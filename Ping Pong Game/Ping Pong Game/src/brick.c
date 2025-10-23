#include "brick.h"
#include "lcd.h"

// размеры и отступы
#define BRICK_WIDTH   14
#define BRICK_HEIGHT  50
#define BRICK_GAP_X    5
#define BRICK_GAP_Y    5
#define BRICK_START_X 10
#define BRICK_START_Y 20

Brick bricks[BRICK_ROWS][BRICK_COLS];

uint8_t bricks_live= BRICK_ROWS*BRICK_COLS;

// инициализация массива кирпичей
void init_bricks(void)
{
    for (int row = 0; row < BRICK_ROWS; row++) {
        for (int col = 0; col < BRICK_COLS; col++) {
            Brick *b = &bricks[row][col];
            b->x = BRICK_START_X + col * (BRICK_WIDTH + BRICK_GAP_X);
            b->y = BRICK_START_Y + row * (BRICK_HEIGHT + BRICK_GAP_Y);
            b->width = BRICK_WIDTH;
            b->height = BRICK_HEIGHT;
            b->color = 0x0D0A;
            b->active = true;
        }
    }
}

// отрисовка кирпичей
void draw_bricks(void)
{
    for (int row = 0; row < BRICK_ROWS; row++) {
        for (int col = 0; col < BRICK_COLS; col++) {
            Brick *b = &bricks[row][col];
            if (b->active) {
                rect(b->height, b->width, b->y, b->x, b->color);
            }
        }
    }
}

// уничтожение кирпича
void destroy_brick(uint8_t row, uint8_t col, uint16_t BG_COLOR)
{
    if (row < BRICK_ROWS && col < BRICK_COLS && bricks[row][col].active) {
        bricks[row][col].active = false;
        rect(bricks[row][col].height, bricks[row][col].width, bricks[row][col].y, bricks[row][col].x, BG_COLOR); // очистка под фон
    }
}

// Возвращает 1, если было столкновение, 0 если нет
uint8_t check_ball_collision_with_bricks(float ball_x, float ball_y, float radius, float *dx, float *dy, uint16_t BG_COLOR) {
    for (int row = 0; row < BRICK_ROWS; row++) {
        for (int col = 0; col < BRICK_COLS; col++) {
            Brick *b = &bricks[row][col];
            if (!b->active) continue;

            float bx1 = (float)b->x;
            float by1 = (float)b->y;
            float bx2 = bx1 + (float)b->width;
            float by2 = by1 + (float)b->height;

            if ((ball_x + radius >= bx1) && (ball_x - radius <= bx2) &&
                (ball_y + radius >= by1) && (ball_y - radius <= by2))
            {
                // деактивируем кирпич
                b->active = false;
                rect(b->height, b->width, b->y, b->x, BG_COLOR);

                // отражаем мяч
                float overlap_left   = (ball_x + radius) - bx1;
                float overlap_right  = bx2 - (ball_x - radius);
                float overlap_top    = (ball_y + radius) - by1;
                float overlap_bottom = by2 - (ball_y - radius);

                float min_overlap = overlap_left;
                int axis = 0;

                if (overlap_right < min_overlap) { min_overlap = overlap_right; axis = 0; }
                if (overlap_top < min_overlap) { min_overlap = overlap_top; axis = 1; }
                if (overlap_bottom < min_overlap) { min_overlap = overlap_bottom; axis = 1; }

                // инвертируем скорость по оси столкновения
                if (axis == 0) *dx = -(*dx);
                else           *dy = -(*dy);
                bricks_live--;
                return 1; // столкновение было
            }
        }
    }
    return 0; // столкновений нет
}