
#include "mik32_hal_pcc.h"
#include "mik32_hal_i2c.h"
#include "mik32_memory_map.h"
#include "uart_lib.h"
#include "xprintf.h"

#include "lcd_i2c.h"

// структура I2C
I2C_HandleTypeDef hi2c0;

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
    if (HAL_I2C_Init(&hi2c0) != HAL_OK)
    {
        xprintf("I2C_Init error\n");
    }
    else xprintf("I2C_Init OK\n");
}

HAL_StatusTypeDef I2C_IsDeviceReady(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Timeout)
{
    // Пытаемся передать 0 байт на устройство
    return HAL_I2C_Master_Transmit(hi2c, DevAddress, NULL, 0, Timeout);
}

int main()
{


    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    xprintf("LCD I2C start\n");

    //Включаем PCC I2C/GPIO тактирование
    __HAL_PCC_I2C_0_CLK_ENABLE();
    __HAL_PCC_GPIO_0_CLK_ENABLE();

    I2C0_Init();

    LCD_I2C lcd;

    LCD_Init(&lcd, &hi2c0, 0x27, 16, 2); // адрес 0x27
    //LCD_BacklightOff(&lcd);
    //LCD_Clear(&lcd);
    LCD_SetCursor(&lcd, 0, 0);
    LCD_Print(&lcd, "Hello, World!");
    LCD_SetCursor(&lcd, 0, 1);
    LCD_Print(&lcd, "LCD test");
 
}