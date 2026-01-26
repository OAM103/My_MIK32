#ifndef SAVE_H
#define SAVE_H

#include "mik32_hal_rtc.h"
#include "xprintf.h"
#include <stdint.h>
#include <string.h>

extern RTC_HandleTypeDef hrtc;

extern RTC_TimeTypeDef LastTime;
extern RTC_TimeTypeDef CurrentTime;
extern RTC_DateTypeDef CurrentDate;

typedef struct {
    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} DateTime;

void time_update();
void RTC_Init(void);

#endif
