#include "build_timestamp.h"

#include "mik32_hal_rtc.h"
#include "mik32_hal_pcc.h"
#include "mik32_hal.h"
#include "mik32_hal_irq.h"
#include "uart_lib.h"
#include "xprintf.h"

#include "system_config.h"
#include "lcd.h"
#include "touchpad.h"
#include "animation.h"
#include "sprite.h"
#include "button.h"

#include <stdint.h>
#include <string.h>

typedef struct {
    const uint16_t *img;
    uint16_t w;
    uint16_t h;
    uint8_t id;
} FoodItem;
FoodItem foods[] = {
    { egg,     32, 40, 3 },
    { chicken, 50, 50, 4 },
    { fish,    84, 55, 5 }
};

#define FOOD_COUNT (sizeof(foods)/sizeof(foods[0]))
#define TIMER 300

volatile uint8_t touch_active = 0;

uint8_t food = 0;
uint8_t food_choice = 0;
uint8_t sleep = 1;
uint8_t time = 0;
uint8_t sleep_entered = 0;
uint8_t was_sleep = 0;

uint32_t status_happy  = 700;
uint32_t status_food  = 700;
uint32_t status_sleep  = 0;

int drag_x = 0;
int drag_y = 0;

void food_up(void)
{
    ClearMassDMA(45, 45, 53, 95, BG_COLOR);
    food = 1;

    PrintMassDMA_Optimized(egg,     32, 40,  80, 420, BG_COLOR);
    PrintMassDMA_Optimized(chicken, 50, 50, 135, 420, BG_COLOR);
    PrintMassDMA_Optimized(fish,    84, 55, 210, 420, BG_COLOR);

    while (food) {
        PlayAnimation3();

        int tp = process_touchpad();

        for (int i = 0; i < FOOD_COUNT; i++) {
            if (tp == foods[i].id) {
                food_choice = foods[i].id;
                food = 0;
                ClearMassDMA(320, 60, 60, 420, BG_COLOR);
                return;
            }
        }

        if ((tp == 1 || tp == 2) && touch_active) {
            ClearMassDMA(320, 60, 60, 420, BG_COLOR);
            food = 0;
        }
    }
}

void ProcessFood(const FoodItem *f)
{
    int tp = process_touchpad();

    if (tp == 1) { // тач по лисе
        PrintMassDMA_Overlay(eat, 200, 200, f->img, f->w, f->h, 480 - 120 - coord_y - 20, coord_x - 60, 60, 120, BG_COLOR);
        ClearMassDMA(f->w, f->h, drag_x, drag_y, BG_COLOR);

        if (process_touchpad() == 0) {
            PrintMassDMA_Optimized(fox_stay, 200, 200, 60, 120, BG_COLOR);
            PlayAnimation4();
            if (status_happy >= 150)status_happy-= food_choice*20;
            else status_happy = 0;
            if (status_food >= 150)status_food-= food_choice*30;
            else status_food = 0;
            food_choice = 0;
            PrintMassDMA_Optimized(food_button, 40, 40, 10, 420, BG_COLOR);
        }
    }

    else if (tp == 2) { // тащим еду
        PrintMassDMA_Optimized(eat, 200, 200, 60, 120, BG_COLOR);
        ClearMassDMA(f->w, f->h, drag_x, drag_y, BG_COLOR);

        drag_x = coord_x;
        drag_y = 480 - coord_y;

        PrintMassDMA_Optimized(f->img, f->w, f->h, drag_x, drag_y, BG_COLOR);
    }
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    uint8_t dma_buffer[DMA_BUFFER_SIZE];
    set_dma_buf(&dma_buffer);

    GPIO_Init();
    HAL_EPIC_MaskLevelSet(HAL_EPIC_GPIO_IRQ_MASK);
    HAL_IRQ_EnableInterrupts();

    SPI0_Init();
    SPI1_Init();
    DMA_Init();
    Lcd_Init();

    ClearMassDMA(320, 480, 0, 0, BG_COLOR);

    Button_Add(420, 10, 40, 40, food_up);
    buttons[0].active = 1;

    PrintMassDMA_Optimized(food_button, 40, 40, 10, 420, BG_COLOR);
    //PrintMassDMA_Optimized(fox_stay, 200, 200, 60, 120, BG_COLOR);

    while (1)
    {
        PrintMassDMA_Optimized(happy, 10, 10, 5, 3, BG_COLOR);
        ClearMassDMA(status_happy/10, 7, 120-status_happy/10, 5, 0x0004); //Статискика Счастья
        ClearMassDMA(100-status_happy/10, 7, 20, 5, GetBackgroundColor(status_happy)); //Статискика Счастья

        PrintMassDMA_Optimized(food_status, 10, 10, 5, 16, BG_COLOR);
        ClearMassDMA(status_food/10, 7, 120-status_food/10, 18, 0x0004); //Статискика еды
        ClearMassDMA(100-status_food/10, 7, 20, 18, GetBackgroundColor(status_food)); //Статискика еды

        PrintMassDMA_Optimized(sleep_status, 10, 10, 5, 29, BG_COLOR);
        ClearMassDMA(status_sleep/10, 7, 120-status_sleep/10, 31, 0x0004); //Статискика сна
        ClearMassDMA(100-status_sleep/10, 7, 20, 31, GetBackgroundColor(status_sleep)); //Статискика сна
        
        if (process_touchpad() == 6) {
            if (sleep == 0) ClearMassDMA(200, 200, 60, 120, BG_COLOR);
            sleep = 1;
        }
        else if (process_touchpad() == 1 || process_touchpad() == 2) { //есть касание - лисёнок просыпается
            time = 0;
            sleep = 0;
        }
        else { //если касания нет, то проверяем время бодрствования, иначе засыпает
            time++;
            if (time >= TIMER || status_sleep >= 980){
                if (sleep == 0) ClearMassDMA(200, 200, 60, 120, BG_COLOR);
                sleep = 1;
            } 
        }
        if (sleep) { // сон
            PlayAnimation5();
            if (status_food < 980) status_food+=2;
            if (status_sleep >= 3) status_sleep -= 2;
            if (status_happy >= 2) status_happy -= 1;   
        }
        else {
            process_button();
            if (touch_active && process_touchpad() == 1 && food_choice == 0) { //если нажимаем на лису
                ClearMassDMA(45, 45, 53, 95, BG_COLOR);
                PlayAnimation2();//cute
                if (status_happy >= 11) status_happy -= 10;
            }
            else if (food_choice) { //если сработала кнопка еды
                for (int i = 0; i < FOOD_COUNT; i++) {
                    if (foods[i].id == food_choice) {
                        ProcessFood(&foods[i]);
                        break;
                    }
                }
            }
            else { //Просто стоим
                ClearMassDMA(15, 15, 170, 100, BG_COLOR);
                ClearMassDMA(45, 45, 53, 95, BG_COLOR);
                if (status_happy < 980) status_happy+=2;
                if (status_food < 980) status_food+=3;
                if (status_sleep < 980) status_sleep++;
                if (status_happy >= 750 || status_food >= 750) PlayAnimation6(); // долго стояли, настроение упало
                else if (status_sleep >= 750) PlayAnimation7(); //хочет спать
                else PlayAnimation1();
            }
        }
    }
}

void trap_handler()
{
    if (EPIC_CHECK_GPIO_IRQ())
    {
        if (HAL_GPIO_LineInterruptState(GPIO_LINE_3)) touch_active = 1;  // коснулись экрана
        HAL_GPIO_ClearInterrupts();
    }
    HAL_EPIC_Clear(0xFFFFFFFF);
}