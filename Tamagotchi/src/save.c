#include "build_timestamp.h"
#include "system_config.h"
#include "xprintf.h"
#include "save.h"
#include <stdint.h>
#include <string.h>

RTC_HandleTypeDef hrtc;

RTC_TimeTypeDef LastTime = {0};
RTC_TimeTypeDef CurrentTime = {0};
RTC_DateTypeDef CurrentDate = {0};

// void time_update(){
//     CurrentTime = HAL_RTC_GetTime(&hrtc);
//     uint32_t x_read = 0;
//     HAL_EEPROM_Read(&heeprom, 0x0C00, &x_read, 1, EEPROM_OP_TIMEOUT);
//     int x = (int)x_read;
//     if (x_read == 0xFFFFFFFF) x = 0;
//     else xprintf("x = %d\n", x);
//     x += 5;
//     xprintf("x+5 = %d\n", x);
//     uint32_t x_write = x;
//     HAL_EEPROM_Erase(&heeprom, 64000, 1, HAL_EEPROM_WRITE_SINGLE, EEPROM_OP_TIMEOUT);
//     HAL_EEPROM_Write(&heeprom, 64000, &x_write, 1, HAL_EEPROM_WRITE_SINGLE, EEPROM_OP_TIMEOUT);
//     xprintf("%d:%d:%d\n", CurrentTime.Hours, CurrentTime.Minutes, CurrentTime.Seconds);
//     //xprintf("%04d-%02d-%02d %02d:%02d:%02d\n",BUILD_YEAR, BUILD_MONTH, BUILD_DAY,BUILD_HOUR, BUILD_MINUTE, BUILD_SECOND);
// }

void time_update(){
    CurrentTime = HAL_RTC_GetTime(&hrtc);
    // uint8_t x_read = 0;
    // HAL_EEPROM_Read(&heeprom, 64000, &x_read, 1, EEPROM_OP_TIMEOUT);
    // int x = (int)x_read;
    // if (x_read == 0xFFFFFFFF) x = 0;
    // else xprintf("x = %d\n", x);
    // x += CurrentTime.Minutes;
    // xprintf("x+5 = %d\n", x);
    // uint32_t x_write = x;
    // HAL_EEPROM_Erase(&heeprom, 64000, 1, HAL_EEPROM_WRITE_SINGLE, EEPROM_OP_TIMEOUT);
    // HAL_EEPROM_Write(&heeprom, 64000, &x_write, 1, HAL_EEPROM_WRITE_SINGLE, EEPROM_OP_TIMEOUT);
    xprintf("%d:%d:%d\n", CurrentTime.Hours, CurrentTime.Minutes, CurrentTime.Seconds);
    //xprintf("%04d-%02d-%02d %02d:%02d:%02d\n",BUILD_YEAR, BUILD_MONTH, BUILD_DAY,BUILD_HOUR, BUILD_MINUTE, BUILD_SECOND);
}

void RTC_Init(void)
{
    __HAL_PCC_RTC_CLK_ENABLE();
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    hrtc.Instance = RTC;
    HAL_RTC_Disable(&hrtc);
    sTime.Hours = BUILD_HOUR;
    sTime.Minutes = BUILD_MINUTE;
    sTime.Seconds = BUILD_SECOND;
    HAL_RTC_SetTime(&hrtc, &sTime);
    sDate.Day = BUILD_DAY;
    sDate.Month = BUILD_MONTH;
    sDate.Year = BUILD_YEAR;
    HAL_RTC_SetDate(&hrtc, &sDate);
    HAL_RTC_Enable(&hrtc);
}
