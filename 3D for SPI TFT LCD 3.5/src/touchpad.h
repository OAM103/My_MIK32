#ifndef TOUCHPAD_H
#define TOUCHPAD_H
#include "mik32_hal.h"
#include "transform.h"

#define READ_X  0xD0
#define READ_Y  0x90

extern float angle_x;
extern float angle_y;
extern int active;

typedef struct {
    int last_x;
    int last_y;
} TouchState;


int read_coordinate(unsigned char command);
void process_touchpad();
uint16_t transform(uint16_t value, uint16_t r1_max, uint16_t r1_min, uint16_t r2_max, uint16_t r2_min);
int read_filtered_coordinate(uint8_t command, uint8_t samples);
void dynamic_rectangle(Rectangle *rect, uint16_t BG_COLOR, uint16_t RECT_COLOR);
void rotate_rectangle(Rectangle *rect, uint16_t BG_COLOR, uint16_t RECT_COLOR);
//void process_touchpad_rectangle(Rectangle *rect, uint16_t BG_COLOR, uint16_t RECT_COLOR);
void rotate_cube_touch();
#endif