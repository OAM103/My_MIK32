// #include "mik32_hal_gpio.h"
// #include "mik32_hal_spi.h"
// #include "mik32_hal_dma.h"
#include <stdlib.h>
#include <string.h>
// #include "mik32_hal_lcd5110_bitmaps.h"
#include "uart_lib.h"
#include "xprintf.h"
#include "png.h"
#include "ball.h"
#include "system_config.h"
#include "lcd.h"
#include "touchpad.h"

// void DrawBall(uint16_t x, uint16_t y){

// }

int main() {
    SystemClock_Config();
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M | UART_CONTROL1_RE_M, 0, 0);
    xprintf("start\n");

    SPI0_Init();
    SPI1_Init();
    GPIO_Init();
    Lcd_Init();
    DMA_Init();

    ClearMass(480, 320, 0 , 0, 0xFFFF);
    //PrintMassDMA(&ball0[0], 100, 100, 200, 100);
    while (1) {
        PrintMassDMA(&ball1[0], 100, 100, 185, 100);
        PrintMassDMA(&ball2[0], 100, 100, 185, 100);
        PrintMassDMA(&ball3[0], 100, 100, 185, 100);
        PrintMassDMA(&ball4[0], 100, 100, 185, 100);
        PrintMassDMA(&ball5[0], 100, 100, 185, 100);
        PrintMassDMA(&ball4[0], 100, 100, 185, 100);
        PrintMassDMA(&ball3[0], 100, 100, 185, 100);
        PrintMassDMA(&ball2[0], 100, 100, 185, 100);
        // // PrintMassDMA(&ball1[0], 100, 100, 185, 100);
        // PrintMassDMA(&mikron[0],479,319,0,0);
        // HAL_DelayMs(500);
        // PrintMassDMA_DoubleBuffer(&img[0], 479, 319, 0, 0);
        // HAL_DelayMs(500);
        //PrintMassDMA_Optimized(&inf[0], 480, 320, 0, 0, 0x20E4, 0x0000);
        //HAL_DelayMs(500);
        //process_touchpad();
        
    }
}



