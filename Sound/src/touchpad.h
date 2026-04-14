#ifndef TOUCHPAD_H
#define TOUCHPAD_H
#include "mik32_hal.h"

//extern SPI_HandleTypeDef hspi1;

#define READ_X  0xD0
#define READ_Y  0x90

typedef struct {
    int x;
    int y;
    int active;
} MovingCircle;

#define MAX_CIRCLES 50

extern uint8_t touch_active;
extern uint8_t play_sound;

int read_coordinate(unsigned char command);
void rect(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w , uint16_t color);
void process_touchpad_active();
void process_touchpad();
void update_circles();

#endif