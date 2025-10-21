#ifndef BALL_H
#define BALL_H
#include <stdint.h>
#include <stdbool.h>
#include "touchpad.h"

extern bool game_over;

void update_ball(uint16_t BALL_COLOR, uint16_t BG_COLOR, Platform *plat);

#endif