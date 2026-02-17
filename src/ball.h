#ifndef BALL_H
#define BALL_H

#include <stdint.h>
#include <stdbool.h>
#include "touchpad.h"

typedef struct {
    int16_t x;      // позиция
    int16_t y;
    int16_t radius; // радиус
    float dx;       // скорость X
    float dy;       // скорость Y
    uint8_t initialized;
} Ball;

extern Ball ball;
extern uint8_t game_over;

void reset_ball(void);
void update_ball(uint16_t BALL_COLOR, uint16_t BG_COLOR, Platform *plat);

#endif