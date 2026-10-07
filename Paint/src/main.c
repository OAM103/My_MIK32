/**
 * @file    main.c
 * @brief   Прошивка для MIK32 Амур: LCD дисплей (480×320) и тачпад. 
 *          При касании экрана рисуется красная линия.
 */

/* Includes ------------------------------------------------------------------*/
#include "system_config.h"
#include "uart_lib.h"
#include "paint.h"

/* Private function prototypes -----------------------------------------------*/
static void periph_Init(void);

/* Private functions ---------------------------------------------------------*/
int main(void)
{
    SystemClock_Config();
    periph_Init();
    // ClearMassDMA(480, 320, 0, 0, 0xFFF5); // Заливка фона
    paint_init();
    
    while (1) {
        paint_process();
        // touch_debug_raw();
        HAL_DelayMs(20);
    }
}

/**
 * @brief Инициализация переферии
 */
static void periph_Init(void)
{
    HAL_Init();
    GPIO_Init();
    
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    SPI0_Init();
    SPI1_Init();

    Lcd_Init();
    DMA_Init();
}