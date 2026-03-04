#ifndef TOUCHPAD_H
#define TOUCHPAD_H
#include "mik32_hal.h"

//extern SPI_HandleTypeDef hspi1;

#define READ_X  0xD0
#define READ_Y  0x90

#define WINDOW_Y 50
#define WINDOW_H 250

#define SCROLL_ZONE_H 270
#define SCROLL_ZONE_W 50

extern volatile int coord_x;
extern volatile int coord_y;

int process_touchpad(void);
uint16_t transform(uint16_t value, uint16_t r1_max, uint16_t r1_min, uint16_t r2_max, uint16_t r2_min);
int read_filtered_coordinate(uint8_t command, uint8_t samples);

#endif