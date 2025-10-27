#include "lcd.h"
#include "touchpad.h"
#include "transform.h"
#include "mik32_hal.h"
#include <stdlib.h>
#include <stdint.h>

// рисование контура прямоугольника
void draw_rectangle(const Rectangle *rect, uint16_t color)
{
    if (!rect) return;
    H_line(rect->x, rect->y, rect->width, color);                  // верх
    H_line(rect->x, rect->y + rect->height, rect->width, color);   // низ
    V_line(rect->x, rect->y, rect->height, color);                 // левая
    V_line(rect->x + rect->width, rect->y, rect->height, color);   // правая
}