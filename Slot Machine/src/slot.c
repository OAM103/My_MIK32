#include "mik32_hal.h"
#include "xprintf.h"
#include "lcd.h"
#include "slot.h"
#include "symbols.h"
#include <stdlib.h>

#define IMG_COUNT 6
#define IMG_WIDTH  90
#define IMG_HEIGHT 90
#define SCREEN_CENTER_X 210
#define SCREEN_CENTER_Y 190

const uint16_t *slot_images[IMG_COUNT] = {&bell[0], &cherry[0], &clover[0], &lemon[0], &orange[0], &seven[0]};

//кадр вращения
// void DrawSlotFrame(int down, uint16_t BG_COLOR) {
//     int next = (_img + 1) % IMG_COUNT; //какое изображение следующее, % IMG_COUNT чтобы был цикл
//     // текущее изображение
//     PrintMassDMA_Optimized(slot_images[_img], IMG_HEIGHT, IMG_WIDTH, SCREEN_CENTER_Y + down, SCREEN_CENTER_X, BG_COLOR);
//     //ClearMassDMA(IMG_HEIGHT,IMG_WIDTH,SCREEN_CENTER_Y + down, SCREEN_CENTER_X, 0x0000);
//     // следующее изображение
//     PrintMassDMA_Optimized(slot_images[next], IMG_HEIGHT, IMG_WIDTH, SCREEN_CENTER_Y + down - IMG_HEIGHT, SCREEN_CENTER_X, BG_COLOR);
//     //ClearMassDMA(IMG_HEIGHT, IMG_WIDTH, SCREEN_CENTER_Y + down - IMG_HEIGHT, SCREEN_CENTER_X, 0x0000);
// }

// // вращения барабана
// void SpinSlot(uint16_t X, uint16_t BG_COLOR){
//     int down = 0; //прокрутка вниз (сдвиг)
//     int speed = 80; // начальная скорость прокрутки
//     int slow = 1;   // коэффициент замедления

//     while (speed > 7) {
//         down += speed;

//         if (down >= IMG_HEIGHT) {//текущее изображение полностью прокрутилось, переключ. на следующее
//             down = 0;
//             _img = (_img + 1) % IMG_COUNT; // следующее изображение, % IMG_COUNT чтобы был цикл
//         }

//         DrawSlotFrame(down, BG_COLOR); // обнавляем экран
//         speed -= slow;    // замедляем
//     }

//     // финальное изображение
//     PrintMassDMA_Optimized(slot_images[_img], IMG_HEIGHT, IMG_WIDTH, SCREEN_CENTER_Y, SCREEN_CENTER_X, BG_COLOR);
// }
Reel reels[3];

int reelsFlag = 1;
int winFlag=0;

void InitReels(uint16_t X)
{
    if (reelsFlag == 1){
        for (int i = 0; i < 3; i++)
        {
            reels[i].img =  rand() % IMG_COUNT;
            reels[i].down = 0;
            reels[i].speed = 90+(X%10);
            reels[i].active = 1;
            reels[i].spins = (i) + 10 + (X%10); // <-чем меньше число, тембыстрее остановится
        }
        reelsFlag = 0;
    }
    else{
        for (int i = 0; i < 3; i++)
        {
            reels[i].down = 0;
            reels[i].speed = 90+(X%10);
            reels[i].active = 1;
            reels[i].spins = (i) + 10 + (X%10); // <-чем меньше число, тембыстрее остановится
        }
    }
    
}
void DrawReelFrame(Reel* r, int x, uint16_t BG_COLOR)
{
    int next = (r->img + 1) % IMG_COUNT; //какое изображение следующее, % IMG_COUNT чтобы был цикл
    int next2 = (r->img + 2) % IMG_COUNT; //какое изображение следующее, % IMG_COUNT чтобы был цикл
    PrintMassDMA_Optimized(slot_images[r->img], IMG_HEIGHT, IMG_WIDTH, 30 + SCREEN_CENTER_Y + r->down, x, BG_COLOR);               // нижний ряд
    PrintMassDMA_Optimized(slot_images[next], IMG_HEIGHT, IMG_WIDTH, 20 + SCREEN_CENTER_Y + r->down - IMG_HEIGHT, x, BG_COLOR);    // средний ряд
    PrintMassDMA_Optimized(slot_images[next2], IMG_HEIGHT, IMG_WIDTH, 10 + SCREEN_CENTER_Y + r->down - IMG_HEIGHT*2, x, BG_COLOR); // верхний ряд

}
void ClearReelFrame(Reel* r, int x, uint16_t BG_COLOR)
{
    // Нижнее изображение
    ClearMassDMA(IMG_HEIGHT, IMG_WIDTH, 30 + SCREEN_CENTER_Y + r->old_down,x, BG_COLOR);
    // Среднее изображение
    ClearMassDMA(IMG_HEIGHT, IMG_WIDTH, 20 + SCREEN_CENTER_Y + r->old_down - IMG_HEIGHT, x, BG_COLOR);
    // Верхнее изображение
    ClearMassDMA(IMG_HEIGHT, IMG_WIDTH,10 + SCREEN_CENTER_Y + r->old_down - IMG_HEIGHT * 2, x, BG_COLOR);
}
void Clear3Reels(uint16_t BG_COLOR)
{
    ClearReelFrame(&reels[0], 355, BG_COLOR);
    ClearReelFrame(&reels[1], 195, BG_COLOR);
    ClearReelFrame(&reels[2], 35, BG_COLOR);
}
void Draw3Reels(uint16_t BG_COLOR)
{
    DrawReelFrame(&reels[0], 355, BG_COLOR);
    DrawReelFrame(&reels[1], 195, BG_COLOR);
    DrawReelFrame(&reels[2], 35, BG_COLOR);
}

int CheckWin()
{
    int middle0 = reels[0].img;
    int middle1 = reels[1].img;
    int middle2 = reels[2].img;
    HAL_DelayMs(500);
    if(middle0 == middle1 && middle1 == middle2){
        ClearMassDMA(320, 480, 0, 0, 0xFCC0);
        drawText(180, 30, "YOU WON...", 0x0000, 0xFCC0, 7);
        HAL_DelayMs(3000);
        drawText(120, 120, "nothiiiing!!!", 0x0000, 0xFCC0, 3);
        winFlag = 1;
    }
}

void Spin3Slots(uint16_t BG_COLOR)
{
    if(winFlag!=0) ClearMassDMA(320, 480, 0, 0, BG_COLOR);
    int active_count = 3; //для остановки по очереди
    while (active_count > 0)
    {
        for (int i = 0; i < 3; i++)
        {
            if (!reels[i].active) continue;
            // обновляем сдвиг
            reels[i].old_down = reels[i].down;
            reels[i].down += reels[i].speed;
            // переход к следующей картинке
            if (reels[i].down >= IMG_HEIGHT)
            {
                reels[i].down=0;
                reels[i].img = (reels[i].img + 1) % IMG_COUNT;
                reels[i].spins--;    // один оборот завершён
            }
            // замедление при конце движения
            if (reels[i].spins < 20 )
                reels[i].speed--;
            // остановка
            if (reels[i].speed <= 6 || reels[i].spins <= 0)
            {
                reels[i].speed = 0;
                reels[i].active = 0;
                reels[i].down = 0;
                active_count--;
            }
        }
        // обновление экрана — 3 барабана сразу
        Draw3Reels(BG_COLOR);
    }
    Clear3Reels(BG_COLOR);
    Draw3Reels(BG_COLOR);
    draw_rect(110,30,100,420,2,BG_COLOR);
    CheckWin();
}