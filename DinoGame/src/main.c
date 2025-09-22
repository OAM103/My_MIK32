#include "uart_lib.h"
#include "xprintf.h"
#include "lcd_i2c.h"
#include <stdio.h>

#define BUTTON_PIN 8
#define BUTTON_PORT GPIO_0

// структура I2C
I2C_HandleTypeDef hi2c0;
void SystemClock_Config(void);
void GPIO_Init(void);

// Инициализация I2C0
static void I2C0_Init(void)
{
    hi2c0.Instance = I2C_0;
    hi2c0.Init.Mode = HAL_I2C_MODE_MASTER;
    hi2c0.Init.DigitalFilter = I2C_DIGITALFILTER_OFF;
    hi2c0.Init.AnalogFilter = I2C_ANALOGFILTER_DISABLE;
    hi2c0.Init.AutoEnd = I2C_AUTOEND_ENABLE;
    hi2c0.Clock.PRESC = 5;
    hi2c0.Clock.SCLDEL = 10;
    hi2c0.Clock.SDADEL = 10;
    hi2c0.Clock.SCLH = 16;
    hi2c0.Clock.SCLL = 16;

    if (HAL_I2C_Init(&hi2c0) != HAL_OK)
        xprintf("I2C_Init error\n");
    else
        xprintf("I2C_Init OK\n");
}

int isButtonPressed() {
    return (BUTTON_PORT->STATE & (1 << BUTTON_PIN)) != 0;
}

void intToStr(int num, char* str) {
    int i = 0;
    char buf[10];

    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    while (num > 0) {
        buf[i++] = (num % 10) + '0';
        num /= 10;
    }

    // переворачиваем строку
    for (int j = 0; j < i; j++) {
        str[j] = buf[i - j - 1];
    }
    str[i] = '\0';
}

int main()
{
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    __HAL_PCC_I2C_0_CLK_ENABLE();
    __HAL_PCC_GPIO_0_CLK_ENABLE();

    I2C0_Init();

    LCD_I2C lcd;
    LCD_Init(&lcd, &hi2c0, 0x27, 16, 2); // адрес 0x27

    // Создаем символы динозаврика и кактуса
    uint8_t dinoChar[8] = {0b01110, 0b11111, 0b11111, 0b10101, 0b10101, 0b10101, 0b11111, 0b01110};
    uint8_t cactusChar[8] = {0b00000, 0b00100, 0b00100, 0b1100, 0b01110, 0b01110, 0b11111, 0b11111};

    LCD_CreateChar(&lcd, 1, dinoChar);   // символ №1 — динозаврик
    LCD_CreateChar(&lcd, 2, cactusChar); // символ №2 — кактус

    int cactusPos = 15;
    int dinoRow = 1;          // 1 = на земле, 0 = в воздухе
    int jumpTimer = 0;
    const int jumpDuration = 3;
    int lastButtonState = 0;
    int score = 0;            // счёт игрока
    int speed = 300;          // начальная скорость игры (мс)

    while (1) {
        // Обработка кнопки как события
        int buttonState = isButtonPressed();
        if (buttonState && !lastButtonState && dinoRow == 1) {
            dinoRow = 0;          // прыжок вверх
            jumpTimer = jumpDuration;
        }
        lastButtonState = buttonState;

        //Таймер прыжка
        if (jumpTimer > 0) {
            jumpTimer--;
            if (jumpTimer == 0) dinoRow = 1; // возвращение на землю
        }

        // Отображение на LCD
        LCD_Clear(&lcd);

        LCD_SetCursor(&lcd, cactusPos, 1);
        LCD_Write(&lcd, 2);  // кактус

        LCD_SetCursor(&lcd, 1, dinoRow);
        LCD_Write(&lcd, 1);  // динозаврик

        // очки (в правом верхнем углу)
        char buf[6];
        intToStr(score, buf);
        LCD_SetCursor(&lcd, 13, 0);
        LCD_Print(&lcd, buf);

        //  Движение кактуса
        cactusPos--;
        if (cactusPos < 0) {
            cactusPos = 15;
            score++;   // увеличиваем счёт, когда кактус прошёл

            // ускоряем игру каждые 5 очков
            if (score % 5 == 0 && speed > 100) {
                speed -= 20;
            }
        }

        // Проверка столкновения 
        if (cactusPos == 0 && dinoRow == 1) {
            LCD_Clear(&lcd);
            LCD_SetCursor(&lcd, 3, 0);
            LCD_Print(&lcd, "GAME OVER");

            LCD_SetCursor(&lcd, 2, 1);
            LCD_Print(&lcd, "SCORE:");

            char buf[6];
            intToStr(score, buf);
            LCD_SetCursor(&lcd, 10, 1);
            LCD_Print(&lcd, buf);

            while(1);
        }

        HAL_DelayMs(speed);
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
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_AUTO;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&PCC_OscInit);
}

void GPIO_Init(void)
{
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();

}