#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <stdint.h>

// структура прямоугольника
typedef struct {
    int x;
    int y;
    int width;
    int height;
    int angle
} Rectangle;

void draw_rectangle(const Rectangle *rect, uint16_t color);
void get_rotated_vertices(Rectangle *rect, float vertices[4][2]);
void draw_rotated_rect(Rectangle *rect, uint16_t color);

#endif