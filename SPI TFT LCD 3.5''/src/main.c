#include <stdlib.h>
#include <string.h>
#include "uart_lib.h"
#include "png.h"
#include "parrot.h"
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
    HAL_Init();
    SystemClock_Config();
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    uint8_t dma_buffer[DMA_BUFFER_SIZE];
    set_dma_buf(&dma_buffer);
    
    GPIO_Init();
    SPI0_Init();
    SPI1_Init();
    Lcd_Init();
    DMA_Init();

    ClearMassDMA(480, 320, 0, 0, 0xAAA0);

    //PrintMassDMA(&mikron[0], 479, 319, 0, 0);
    //PrintMassDMA(&img[0], 479, 319, 0, 0);
    //PrintMassDMA(&image[0], 480, 320, 0, 0);
    //PrintMassDMA(&mik32[0], 479, 319, 0, 0);
    //PrintMassDMA(&board[0], 479, 319, 0, 0);
    //PrintMassDMA(&inf[0], 480, 320, 0, 0);
    Button_Add(10, 10, 50, 100, 0xF800, ShowImage1);
    Button_Add(10, 120, 50, 100, 0x001F, ShowImage2);
    
    while (1) {
        //process_touchpad();
        process_button();
        // PrintMassDMA(&parrot0[0], 150, 150, 270, 170);
        // PrintMassDMA(&parrot1[0], 150, 150, 270, 170);
        // PrintMassDMA(&parrot2[0], 150, 150, 270, 170);
        // PrintMassDMA(&parrot3[0], 150, 150, 270, 170);
        // PrintMassDMA(&parrot4[0], 150, 150, 270, 170);
        // PrintMassDMA(&parrot5[0], 150, 150, 270, 170);
        // PrintMassDMA(&parrot6[0], 150, 150, 270, 170);
        // PrintMassDMA(&parrot7[0], 150, 150, 270, 170);
        // PrintMassDMA(&parrot8[0], 150, 150, 270, 170);
        // PrintMassDMA(&parrot9[0], 150, 150, 270, 170);
    }
}

