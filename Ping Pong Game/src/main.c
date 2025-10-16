#include <stdlib.h>
#include <string.h>
#include "uart_lib.h"
#include "ball.h"
#include "system_config.h"
#include "lcd.h"
#include "touchpad.h"
#include "button.h"

int main() {
    SystemClock_Config();
    SPI0_Init();
    SPI1_Init();
    GPIO_Init();
    Lcd_Init();
    DMA_Init();

    ClearMass(480, 320, 0, 0, 0xAAA0);

    Platform *plat = &right_platform;
    //drawText(180, 34, "You lost!", 0x0000, 0xAAA0, 8);
    while (1) {
        //process_touchpad();
        process_platform_touch(0x0000, 0xAAA0);
        update_ball(0x0000, 0xAAA0, plat);
    }
}