#include "lcd.h"
#include "touchpad.h"
#include "transform.h"
#include "mik32_hal.h"
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

// рисование контура прямоугольника
void draw_rectangle(const Rectangle *rect, uint16_t color)
{
    if (!rect) return;
    H_line(rect->x, rect->y, rect->width, color);                  // верх
    H_line(rect->x, rect->y + rect->height, rect->width, color);   // низ
    V_line(rect->x, rect->y, rect->height, color);                 // левая
    V_line(rect->x + rect->width, rect->y, rect->height, color);   // правая
}

void draw_rotated_rect(Rectangle *r, uint16_t color)
{
    float angle = r->angle * 3.1415926f / 180.0f;
    float cosA = cosf(angle);
    float sinA = sinf(angle);

    // Центр прямоугольника
    int cx = r->x + r->width / 2;
    int cy = r->y + r->height / 2;

    // Вершины относительно центра
    int hw = r->width / 2;
    int hh = r->height / 2;

    int x1 = cx + (int)(-hw * cosA - (-hh) * sinA);
    int y1 = cy + (int)(-hw * sinA + (-hh) * cosA);
    int x2 = cx + (int)( hw * cosA - (-hh) * sinA);
    int y2 = cy + (int)( hw * sinA + (-hh) * cosA);
    int x3 = cx + (int)( hw * cosA - ( hh) * sinA);
    int y3 = cy + (int)( hw * sinA + ( hh) * cosA);
    int x4 = cx + (int)(-hw * cosA - ( hh) * sinA);
    int y4 = cy + (int)(-hw * sinA + ( hh) * cosA);

    // Соединяем линии
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x3, y3, color);
    draw_line(x3, y3, x4, y4, color);
    draw_line(x4, y4, x1, y1, color);
}