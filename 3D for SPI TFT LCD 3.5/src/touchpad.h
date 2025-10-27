#ifndef TOUCHPAD_H
#define TOUCHPAD_H
#include "mik32_hal.h"
#include "transform.h"

int read_coordinate(unsigned char command);
void process_touchpad();
uint16_t transform(uint16_t value, uint16_t r1_max, uint16_t r1_min, uint16_t r2_max, uint16_t r2_min);
int read_filtered_coordinate(uint8_t command, uint8_t samples);
void dynamic_rectangle(Rectangle *rect, uint16_t BG_COLOR, uint16_t RECT_COLOR);
#endif