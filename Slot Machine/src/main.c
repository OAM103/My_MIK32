#include "mik32_hal_pcc.h"
#include "system_config.h"
#include "joystick.h"
#include "lcd.h"
#include "slot.h"
#include "symbols.h"
#include "uart_lib.h"
#include "xprintf.h"

#define BG_COLOR 0xAA70

int main()
{
    SystemClock_Config();
    HAL_Init();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    uint8_t dma_buffer[DMA_BUFFER_SIZE];
    set_dma_buf(&dma_buffer);

    GPIO_Init();
    SPI0_Init();
    SPI1_Init();
    //GPIO_Init();
    Lcd_Init();
    DMA_Init();

    ClearMassDMA(320, 480, 0, 0, BG_COLOR);
    while (1)
    {
        Joystick();
        if (axisY >= 2800) {
            InitReels(axisX);
            // xprintf("Y = %04d", axisY);
            //xprintf("X.10 = %04d", axisX%10);
            xprintf("X.100 = %04d", axisX%100);
            Spin3Slots(BG_COLOR);
        }
    }
}

