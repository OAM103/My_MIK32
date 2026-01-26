#ifndef TOUCHPAD_H
#define TOUCHPAD_H
#include "mik32_hal.h"

//extern SPI_HandleTypeDef hspi1;

#define READ_X  0xD0
#define READ_Y  0x90

extern volatile int coord_x;
extern volatile int coord_y;

int process_touchpad(void);

#endif