#include <stdlib.h>
#include <string.h>
#include "uart_lib.h"
#include "xprintf.h"
#include "png.h"
//#include "ball.h"
//#include "parrot.h"
#include "system_config.h"
#include "lcd.h"
#include "touchpad.h"
#include "button.h"

void ShowImage1(void) {
    PrintMassDMA(&mikron[0], 479, 319, 0, 0);
}

void ShowImage2(void) {
    PrintMassDMA(&img[0], 479, 319, 0, 0);
}

int main() {
    SystemClock_Config();
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M | UART_CONTROL1_RE_M, 0, 0);
    xprintf("start\n");

    SPI0_Init();
    SPI1_Init();
    GPIO_Init();
    Lcd_Init();
    DMA_Init();

    ClearMass(480, 320, 0 , 0, 0xAA00);

    //Button_Add(10, 10, 50, 100, 0xF800, ShowImage1);
    //Button_Add(10, 120, 50, 100, 0x001F, ShowImage2); 

    while (1) {
        //PrintMassDMA(&mikron[0], 479, 319, 50, 0);
        
        process_touchpad();
        //process_button();
        
    }
}

