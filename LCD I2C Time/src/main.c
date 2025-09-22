#include "mik32_hal_rtc.h"
#include "mik32_hal_pcc.h"
#include "mik32_hal_i2c.h"
#include "mik32_memory_map.h"
#include "uart_lib.h"
#include "xprintf.h"

#include "lcd_i2c.h"


#include <stdint.h>
#include <string.h>


I2C_HandleTypeDef hi2c0;
RTC_HandleTypeDef hrtc;

RTC_TimeTypeDef LastTime = {0};
RTC_TimeTypeDef CurrentTime = {0};
RTC_DateTypeDef CurrentDate = {0};
void SystemClock_Config(void);

typedef struct {
    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} DateTime;

uint8_t month_str_to_num(const char* mon)
{
    if (memcmp(mon, "Jan", 3) == 0) return 1;
    if (memcmp(mon, "Feb", 3) == 0) return 2;
    if (memcmp(mon, "Mar", 3) == 0) return 3;
    if (memcmp(mon, "Apr", 3) == 0) return 4;
    if (memcmp(mon, "May", 3) == 0) return 5;
    if (memcmp(mon, "Jun", 3) == 0) return 6;
    if (memcmp(mon, "Jul", 3) == 0) return 7;
    if (memcmp(mon, "Aug", 3) == 0) return 8;
    if (memcmp(mon, "Sep", 3) == 0) return 9;
    if (memcmp(mon, "Oct", 3) == 0) return 10;
    if (memcmp(mon, "Nov", 3) == 0) return 11;
    if (memcmp(mon, "Dec", 3) == 0) return 12;
    return 0;
}

void parse_compile_datetime(DateTime* dt)
{
    char month[4];
    month[0] = __DATE__[0];
    month[1] = __DATE__[1];
    month[2] = __DATE__[2];
    month[3] = '\0';

    dt->month = month_str_to_num(month);

    // день
    dt->day = (__DATE__[4]-'0')*10 + (__DATE__[5]-'0');

    // год
    dt->year = (__DATE__[7]-'0')*1000 + (__DATE__[8]-'0')*100 + (__DATE__[9]-'0')*10 + (__DATE__[10]-'0');

    // TIME = "HH:MM:SS"
    dt->hour = (__TIME__[0]-'0')*10 + (__TIME__[1]-'0');
    dt->min  = (__TIME__[3]-'0')*10+2 + (__TIME__[4]-'0');
    dt->sec  = (__TIME__[6]-'0')*10 + (__TIME__[7]-'0');
}

uint8_t day_of_week(uint16_t y, uint8_t m, uint8_t d)
{
    if (m < 3) { m += 12; y -= 1; }
    uint16_t K = y % 100;
    uint16_t J = y / 100;
    return (d + 13*(m+1)/5 + K + K/4 + J/4 + 5*J) % 7;
}

static void I2C0_Init(void)
{
    // Общие настройки
    hi2c0.Instance = I2C_0;
    hi2c0.Init.Mode = HAL_I2C_MODE_MASTER;
    hi2c0.Init.DigitalFilter = I2C_DIGITALFILTER_OFF;
    hi2c0.Init.AnalogFilter = I2C_ANALOGFILTER_DISABLE;
    hi2c0.Init.AutoEnd = I2C_AUTOEND_ENABLE;
    // Настройка частоты
    hi2c0.Clock.PRESC = 5;
    hi2c0.Clock.SCLDEL = 10;
    hi2c0.Clock.SDADEL = 10;
    hi2c0.Clock.SCLH = 16;
    hi2c0.Clock.SCLL = 16;
    if (HAL_I2C_Init(&hi2c0) != HAL_OK)
    {
        xprintf("I2C_Init error\n");
    }
    else xprintf("I2C_Init OK\n");
}


static void RTC_Init(void)
{
    DateTime dt;
    parse_compile_datetime(&dt);
    uint8_t dow = day_of_week(dt.year, dt.month, dt.day);

    __HAL_PCC_RTC_CLK_ENABLE();
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    hrtc.Instance = RTC;
    /* Установка даты и времени RTC */
    sTime.Dow = dow;
    sTime.Hours = dt.hour;
    sTime.Minutes = dt.min;
    sTime.Seconds = dt.sec;
    HAL_RTC_SetTime(&hrtc, &sTime);
    sDate.Day = dt.day;
    sDate.Month = dt.month;
    sDate.Year = dt.year-2000;
    HAL_RTC_SetDate(&hrtc, &sDate);
    HAL_RTC_Enable(&hrtc);
}

HAL_StatusTypeDef I2C_IsDeviceReady(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Timeout)
{
    // Пытаемся передать 0 байт на устройство
    return HAL_I2C_Master_Transmit(hi2c, DevAddress, NULL, 0, Timeout);
}
void format_date(char *buf, RTC_DateTypeDef *date)
{
    int year = 2000 + date->Year;   // Год → полный формат (2025)

    buf[0] = '0' + (date->Day / 10);     // десятки дня
    buf[1] = '0' + (date->Day % 10);     // единицы дня
    buf[2] = '.';                        // разделитель

    buf[3] = '0' + (date->Month / 10);   // десятки месяца
    buf[4] = '0' + (date->Month % 10);   // единицы месяца
    buf[5] = '.';                        // разделитель

    buf[6] = '0' + (year / 1000) % 10;   // 2
    buf[7] = '0' + (year / 100) % 10;    // 0
    buf[8] = '0' + (year / 10) % 10;     // 2
    buf[9] = '0' + year % 10;            // 5
    buf[10] = '\0';                      // конец строки
}

void format_time(char *buf, RTC_TimeTypeDef *time)
{
    buf[0] = '0' + (time->Hours / 10);   // десятки часов
    buf[1] = '0' + (time->Hours % 10);   // единицы часов
    buf[2] = ':';                        // разделитель

    buf[3] = '0' + (time->Minutes / 10); // десятки минут
    buf[4] = '0' + (time->Minutes % 10); // единицы минут
    buf[5] = ':';                        // разделитель

    buf[6] = '0' + (time->Seconds / 10); // десятки секунд
    buf[7] = '0' + (time->Seconds % 10); // единицы секунд
    buf[8] = '\0';                       // конец строки
}

int main()
{
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    //Включаем PCC I2C/GPIO тактирование
    __HAL_PCC_I2C_0_CLK_ENABLE();
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    RTC_Init();
    I2C0_Init();
    

    LCD_I2C lcd;

    LCD_Init(&lcd, &hi2c0, 0x27, 16, 2); // адрес 0x27
    //LCD_BacklightOff(&lcd);
    LCD_Clear(&lcd);

    LastTime = HAL_RTC_GetTime(&hrtc);

    while (1)
    {
        CurrentTime = HAL_RTC_GetTime(&hrtc);
        if (CurrentTime.Seconds != LastTime.Seconds)
        {
            LastTime = CurrentTime;
            CurrentDate = HAL_RTC_GetDate(&hrtc);
            xprintf("%d.%02d.%d ", CurrentDate.Day, CurrentDate.Month, CurrentDate.Year+2000);
            char date_str[11];  // "DD.MM.YYYY" + '\0'
            format_date(date_str, &CurrentDate);
            LCD_SetCursor(&lcd, 0, 0);
            LCD_Print(&lcd, date_str);
            CurrentTime = HAL_RTC_GetTime(&hrtc);
            switch (CurrentTime.Dow)
            {
            case 2:
                LCD_Print(&lcd, "   Mon");
                //xprintf("Mon\n");
                break;
            case 3:
                LCD_Print(&lcd, "  Tues");
                //xprintf("Tues\n");
                break;
            case 4:
                LCD_Print(&lcd, "  Weds");
                //xprintf("Weds\n");
                break;
            case 5:
                LCD_Print(&lcd, " Thurs");
                xprintf("Thurs\n");
                break;
            case 6:
                LCD_Print(&lcd, "   Fri");
                //xprintf("Fri\n");
                break;
            case 0:
                LCD_Print(&lcd, "   Sat");
                //xprintf("Sat\n");
                break;
            case 1:
                LCD_Print(&lcd, "   Sun");
                //xprintf("Sun\n");
                break;
            }
            xprintf("%d:%d:%02d\n", CurrentTime.Hours, CurrentTime.Minutes, CurrentTime.Seconds);
            char time_str[9];  // "HH:MM:SS" + '\0'
            format_time(time_str, &CurrentTime);
            LCD_SetCursor(&lcd, 0, 1);
            LCD_Print(&lcd, time_str);
        }
       
    }
 
}

void SystemClock_Config(void)
{
    PCC_InitTypeDef PCC_OscInit = {0};
    PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_ALL;
    PCC_OscInit.FreqMon.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;
    PCC_OscInit.FreqMon.ForceOscSys = PCC_FORCE_OSC_SYS_UNFIXED;
    PCC_OscInit.FreqMon.Force32KClk = PCC_FREQ_MONITOR_SOURCE_OSC32K;
    PCC_OscInit.AHBDivider = 0;
    PCC_OscInit.APBMDivider = 0;
    PCC_OscInit.APBPDivider = 0;
    PCC_OscInit.HSI32MCalibrationValue = 128;
    PCC_OscInit.LSI32KCalibrationValue = 8;
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_OSC32K;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&PCC_OscInit);
}