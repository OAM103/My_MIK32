#include <stdlib.h>
#include <string.h>
#include "uart_lib.h"
#include "system_config.h"
#include "lcd.h"
#include "touchpad.h"
#include "transform.h"

#define BG_COLOR 0x0000
#define RECT_COLOR 0xFFFF

int main()
{
    SystemClock_Config();
    SPI0_Init();
    SPI1_Init();
    GPIO_Init();
    Lcd_Init();
    DMA_Init();

    ClearMass(480, 320, 0, 0, BG_COLOR);

    Rectangle rect = { 80, 80, 100, 60 };
    draw_rectangle(&rect, 0xFFFF);

    while (1)
    {
        dynamic_rectangle(&rect, BG_COLOR, RECT_COLOR);
    }
}

