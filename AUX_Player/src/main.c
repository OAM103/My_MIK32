#include "system_config.h"
#include "lcd.h"
#include "touchpad.h"
#include "button.h"
#include "music.h"
#include "tracks.h"
#include "icons.h"

#include "uart_lib.h"
#include "xprintf.h"

#define BG_COLOR 0x0DD0

#define VOL_X 265
#define VOL_Y 15 
#define VOL_W 40
#define VOL_H 440

void DrawVolumeBar()
{
    ClearMassDMA(VOL_X, VOL_Y, VOL_W, VOL_H, 0xFFFF); // белая рамка
    ClearMassDMA(VOL_X - 2, VOL_Y - 2, VOL_W + 1, VOL_H + 1, 0x0000);
    if (volume > 0){
        uint16_t fill = volume * 3 - 2;
        if (fill > 264) fill = 263;
        ClearMassDMA(fill, VOL_Y - 2, VOL_W + 1, VOL_H + 1, 0x0030);
    }
}

int is_in_volume_area(int x, int y) { //проверка попадания тача на регулирование громкости
    if ((x >= 0 && x <= 320) && (y >= 480 - VOL_H - 15 && y <= (480 - VOL_H + VOL_Y + 10))) {
        return 1; 
    }
    return 0; 
} 

int calc_volume_from_y(int x) { 
    x-=20;
    if (x < 10) x = 0; 
    else if (x > 260) x = 100;
    else x /= 3; 
    return x; 
}

uint8_t music_pause = 0;

void Music_Stop(void)
{
    if(music_pause == 0){
        music_pause = 1;
        hdac1.Instance_dac->VALUE = DAC_MID;
        HAL_Timer32_Stop(&htimer32);
        PrintMassDMA_Optimized(start_img, 40, 40, 140, 330, BG_COLOR);
    }
    else{
        music_pause = 0;
        HAL_Timer32_Start(&htimer32);
        PrintMassDMA_Optimized(pause_img, 40, 40, 140, 330, BG_COLOR);
    }
}

void Right_Button(void)
{
    xprintf("Right button\n");
}

void Left_Button(void)
{
    xprintf("Left button\n");
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    uint8_t dma_buffer[DMA_BUFFER_SIZE];
    set_dma_buf(&dma_buffer);

    GPIO_Init();
    SPI0_Init();
    SPI1_Init();
    DMA_Init();
    Lcd_Init();

    DAC_Init();
    Timer32_Init();

    HAL_DAC_Init(&hdac1);

    HAL_Timer32_Value_Clear(&htimer32);
    HAL_Timer32_Start(&htimer32);

    int next_buf = 1;
    int t = 0;

    ClearMassDMA(320, 480, 0, 0, BG_COLOR);

    fill_dac_buf(dac_buf0); // Первичная загрузка двух буферов
    fill_dac_buf(dac_buf1);

    DrawVolumeBar();
    PrintMassDMA_Optimized(volume_img, 24, 20, 8, 439, BG_COLOR);

    Button_Add_Standart(320, 120, 50, 50, Music_Stop);
    PrintMassDMA_Optimized(pause_img, 40, 40, 140, 330, BG_COLOR);

    PrintMassDMA_Optimized(right, 35, 20, 195, 342, BG_COLOR);
    PrintMassDMA_Optimized(left, 35, 20, 90, 340, BG_COLOR);
    Button_Add_Standart(330, 175, 30, 35, Right_Button);
    Button_Add_Standart(330, 70, 30, 35, Left_Button);

    Button_Add("1.What is love \n  Haddway", play_track0);
    Button_Add("2.Jump Around \n  House of Pain", play_track1);
    Button_Add("3.Ничего нового \n  Atlanda Project", play_track2);
    
    DrawButtons();
    while (1)
    {
        t++;
        if(t==10000){
            process_button();
            if (process_touchpad() == 500 && process_touchpad() != -1){
                if (is_in_volume_area(coord_x, coord_y)) { 
                    volume = calc_volume_from_y(coord_x); 
                    DrawVolumeBar(); 
                    if (volume == 0) PrintMassDMA_Optimized(muting_img, 24, 20, 8, 437, BG_COLOR);
                    else PrintMassDMA_Optimized(volume_img, 24, 20, 8, 437, BG_COLOR);
                    xprintf("vol = %d\n", volume);
                } 
            } 
            else ProcessTouchScroll();
            t = 0;
        }
        // Если DMA закончил текущий буфер - запускаем следующий
        if (HAL_DMA_GetChannelReadyStatus(&hdma_ch1) == 1)
        {
            if (next_buf == 0)
            {
                HAL_DMA_Start(&hdma_ch1, dac_buf0, &hdac1.Instance_dac->VALUE, sizeof(dac_buf0)-1);
                fill_dac_buf(dac_buf1); // пока dac_buf0 играет - готовим dac_buf1
                next_buf = 1;
            }
            else
            {
                HAL_DMA_Start(&hdma_ch1, dac_buf1, &hdac1.Instance_dac->VALUE, sizeof(dac_buf1)-1);
                fill_dac_buf(dac_buf0); // пока dac_buf1 играет - готовим dac_buf0
                next_buf = 0;
            }
        }
    }
}
