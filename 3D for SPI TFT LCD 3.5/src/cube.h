#ifndef CUBE_H
#define CUBE_H

#include <stdint.h>

typedef struct {
    float x, y, z;
} Vec3;

typedef struct {
    int x, y;
} Vec2;

void draw_cube(uint16_t BG_COLOR, uint16_t FG_COLOR);

#endif