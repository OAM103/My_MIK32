#include "mik32_hal_pcc.h"
#include "mik32_hal_i2c.h"
#include "mik32_memory_map.h"
#include "uart_lib.h"
#include "xprintf.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_usart.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "lcd_i2c.h"

// структура I2C
I2C_HandleTypeDef hi2c0;

char symbol[8];

// Инициализация I2C0
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
    //Включаем PCC I2C тактирование
    __HAL_PCC_I2C_0_CLK_ENABLE();
    if (HAL_I2C_Init(&hi2c0) != HAL_OK)
    {
        xprintf("I2C_Init error\n");
    }
}

void SystemClock_Config();
void GPIO_Init();

// точки
void dot() {
    HAL_GPIO_TogglePin(GPIO_0, GPIO_PIN_3);
    HAL_DelayMs(250);
    HAL_GPIO_TogglePin(GPIO_0, GPIO_PIN_3);
    HAL_DelayMs(100);
}

// тире
void dash() {
    HAL_GPIO_TogglePin(GPIO_1, GPIO_PIN_3);
    HAL_DelayMs(650);
    HAL_GPIO_TogglePin(GPIO_1, GPIO_PIN_3);
    HAL_DelayMs(100);
}

typedef struct {
    char ch;       // символ
    unsigned char code; // биты 
    unsigned char len;  // длина последовательности
} MorseCode;


const MorseCode morse_table[] = {
    {'a', 0b01, 2},   // .-
    {'b', 0b1000, 4}, // -...
    {'c', 0b1010, 4}, // -.-.
    {'d', 0b100, 3},  // -..
    {'e', 0b0, 1},    // .
    {'f', 0b0010, 4}, // ..-.
    {'g', 0b110, 3},  // --.
    {'h', 0b0000, 4}, // ....
    {'i', 0b00, 2},   // ..
    {'j', 0b0111, 4}, // .---
    {'k', 0b101, 3},  // -.-
    {'l', 0b0100, 4}, // .-..
    {'m', 0b11, 2},   // --
    {'n', 0b10, 2},   // -.
    {'o', 0b111, 3},  // ---
    {'p', 0b0110, 4}, // .--.
    {'q', 0b1101, 4}, // --.-
    {'r', 0b010, 3},  // .-.
    {'s', 0b000, 3},  // ...
    {'t', 0b1, 1},    // -
    {'u', 0b001, 3},  // ..-
    {'v', 0b0001, 4}, // ...-
    {'w', 0b011, 3},  // .--
    {'x', 0b1001, 4}, // -..-
    {'y', 0b1011, 4}, // -.--
    {'z', 0b1100, 4}, // --..

};

// ищем символ в таблице
const MorseCode* find_morse(char c) {
    c = tolower((unsigned char)c);//преобразование в нижний регистр
    int n = sizeof(morse_table) / sizeof(morse_table[0]);//байт вся таб/ бфйт один эл = кол-во эл в табл
    for (int i = 0; i < n; i++) {
        if (morse_table[i].ch == c) return &morse_table[i]; //если найден возврат указ
    }
    return NULL;
}

// выводим символ в Морзе
char* toMorse(char c) {
    const MorseCode *m = find_morse(c);
    if (!m) {
        symbol[0] = '\0';   // пустая строка если не найдено
        return symbol;
    }

    int pos = 0;
    for (int i = 0; i < m->len && pos < sizeof(symbol)-1; i++) {
        if ((m->code >> (m->len - 1 - i)) & 1) {
            dash();                 // моргнули длинный
            symbol[pos++] = '-';    // записали в строку
        } else {
            dot();                  // моргнули короткий
            symbol[pos++] = '.';    // записали в строку
        }
    }
    symbol[pos] = '\0'; // конец строки
    return symbol;
}



int main(void)
{
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M | UART_CONTROL1_RE_M, 0, 0);

    GPIO_Init();
    SystemClock_Config();
    I2C0_Init();

    LCD_I2C lcd;
    LCD_Init(&lcd, &hi2c0, 0x27, 16, 2);
    LCD_BacklightOn(&lcd);

    while (1)
    {
        if (!UART_IsRxFifoEmpty(UART_0)) {
            uint16_t byte = UART_ReadByte(UART_0);

            LCD_Clear(&lcd);
            LCD_SetCursor(&lcd, 0, 0);
            LCD_Write(&lcd, (char)byte);     // сама буква

            char *res = toMorse((char)byte); // морзе в строку + мигание
            LCD_SetCursor(&lcd, 0, 1);
            LCD_Print(&lcd, res);            // печать кода Морзе
        }
        HAL_DelayMs(1000);
    }
    return 0;
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
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_AUTO;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&PCC_OscInit);
}

void GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;

    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIO_1, &GPIO_InitStruct);
}