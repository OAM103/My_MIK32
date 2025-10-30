#ifndef JOYSTIC_H
#define JOYSTIC_H

#include "mik32_hal.h"
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

extern float angle_x;
extern float angle_y;
extern uint8_t cube_update;

void Joystick_Init();

#endif