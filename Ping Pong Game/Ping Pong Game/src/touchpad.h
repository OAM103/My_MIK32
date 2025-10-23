#ifndef TOUCHPAD_H
#define TOUCHPAD_H
#include "mik32_hal.h"

#define READ_X  0xD0
#define READ_Y  0x90

typedef struct {
    int x;
    int y;
    int width;
    int height;
}Platform;

extern Platform right_platform;

int read_coordinate(unsigned char command);
void rect(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w , uint16_t color);
void process_touchpad();
void process_platform_touch(const uint16_t PLATFORM_COLOR, const uint16_t BG_COLOR);
#endif