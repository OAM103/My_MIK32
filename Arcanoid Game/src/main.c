#include <stdlib.h>
#include <string.h>
#include "uart_lib.h"
#include "ball.h"
#include "system_config.h"
#include "lcd.h"
#include "touchpad.h"
#include "button.h"
#include "brick.h"
#include "cookie.h"

#define BG_COLOR 0xAA70

uint8_t game_over_flag = 0;
uint8_t won_flag = 0;
Platform *plat = &right_platform;

void Restart() {
    game_over = 0;
    game_over_flag = 0;
    won_flag = 0;
    bricks_live = BRICK_ROWS*BRICK_COLS;
    ClearMassDMA(480, 320, 0, 0, BG_COLOR);
    HAL_DelayMs(1);
    init_bricks(); 
    draw_bricks();
}


void game_over_screen(){
    game_over_flag = 1;
    bricks_live = BRICK_ROWS*BRICK_COLS;
    ClearMassDMA(481, 321, 0, 0, 0x07E0);
    drawText(180, 32, "GAME OVER", 0x0000, 0x07E0, 8);
    Button_Add(250, 167, 40, 140, 0x07E3,"RESTART", Restart);
}

void won_screen(){
    won_flag = 1;
    ClearMassDMA(480, 320, 0, 0, 0x00F1);
    drawText(180, 80, "YOU WON", 0x0000, 0x00F1, 8);
    drawText(220, 40, "Give this man a cookie!", 0x0000, 0x00F1, 3);
    PrintMassDMA_Optimized(&cookie[0], 70, 70, 210, 20, 0x03E0);
    Button_Add(250, 167, 40, 140, 0x02F0,"RESTART", Restart);
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

    ClearMassDMA(480, 320, 0, 0, BG_COLOR);

    reset_ball();

    init_bricks();
    draw_bricks(); 
    
    while (1) {
        //process_touchpad();
        
        process_platform_touch(0x1C00, BG_COLOR);
        update_ball(0x0003, BG_COLOR, plat); 
        check_ball_collision_with_bricks(ball.x, ball.y, ball.radius, &ball.dx, &ball.dy, BG_COLOR);

        if(game_over==1 && game_over_flag==0 && bricks_live>0) game_over_screen(); 
        if(game_over==1 && game_over_flag==1 && bricks_live>0) process_button();
        if(bricks_live==0 && won_flag==0) won_screen();
        if(bricks_live==0 && won_flag==1) process_button();
    }
}

