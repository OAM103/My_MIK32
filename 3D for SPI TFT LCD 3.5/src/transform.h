#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <stdint.h>

// структура прямоугольника
typedef struct {
    int x;
    int y;
    int width;
    int height;
} Rectangle;

void draw_rectangle(const Rectangle *rect, uint16_t color);

#endif