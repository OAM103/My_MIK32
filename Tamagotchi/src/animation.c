#include "animation.h"
#include "lcd.h"
#include "sprite.h"
#include "button.h"

#include <stdint.h>

#define BG_COLOR 0x0001

uint16_t anim1_frame = 0;
uint16_t anim2_frame = 0;
uint16_t anim3_frame = 0;
uint16_t anim4_frame = 0;
uint16_t anim5_frame = 0;
uint16_t anim6_frame = 0;
uint16_t anim7_frame = 0;

typedef struct {
    const uint16_t *image;   // указатель на картинку
    uint16_t delay_ms;       // задержка после кадра
} Frame;

const Frame anim1[] = {
    { eye1_1,   10 },
    { eye1_2,   10 },
    { eye1_1,   10 },
    { fox_stay, 500 },{ fox_stay, 500 },
    { tail_1,   50 },
    { tail_2,   50 },
    { tail_3,   50 },
    { fox_stay, 500 }, { fox_stay, 500 }, { fox_stay, 500 }, { fox_stay, 500 }, { fox_stay, 500 }, { fox_stay, 500 },
    { tail_1,   50 },
    { tail_2,   50 },
    { tail_3,   50 },
    { fox_stay, 500 },  { fox_stay, 500 }, { fox_stay, 500 }, { fox_stay, 500 }, 
};

const Frame anim3[] = {
    { eat,    500 }, { eat,    500 }, { eat,    500 }, { eat,    500 },
    { tail_1, 50 },
    { tail_2, 50 },
    { tail_3, 50 }, 
};

const Frame anim4[] = {
    { mouth1,    80},
    { mouth2,    80},
    { mouth3,    80},
    { mouth2,    80}, 
    { mouth1,    80},
    { mouth2,    80},
    { mouth3,    80},
    { mouth2,    80},
    { mouth1,    80},
};

const Frame anim5[] = {
    { boop3,    80},
    { sleep1,    200}, { sleep1,    200}, { sleep1,    200},
    { boop2,    80},
    { sleep2,    150},
    { boop1,    80},
    { sleep3,    200}, { sleep3,    200}, { sleep3,    200},
    { boop2,    80},
    { sleep2,    150}, 
    
};

const Frame anim6[] = {
    { fox_sad, 90},
    { sad_eye2, 90}, { sad_eye1, 90}, { sad_eye2, 90}, { sad_eye1, 90}, { sad_eye2, 90}, { sad_eye1, 90}, { sad_eye2, 90}, { sad_eye1, 90}, { sad_eye2, 90}, { sad_eye1, 90}, { sad_eye2, 90}, { sad_eye1, 90}, { sad_eye2, 90}, { sad_eye1, 90}, { sad_eye2, 90}, { sad_eye1, 90}, { sad_eye2, 90}, { sad_eye1, 90}, { sad_eye2, 90}, { sad_eye1, 90}, { sad_eye2, 90}, { sad_eye1, 90},
    { fox_sad, 90},
    { sad_eye2, 90},
    { tail_1, 20 },
    { sad_eye2, 70},
    { tail_2, 20 },
    { sad_eye1, 70},
    { tail_3, 20 }, 
    { sad_eye2, 70},
    
};

const Frame anim7[] = {
    { tired1, 500 }, { tired1, 500 }, { tired1, 500 }, { tired1, 500 }, { tired1, 500 }, { tired1, 500 }, { tired1, 500 }, { tired1, 500 }, { tired1, 500 }, { tired1, 500 }, { tired1, 500 }, { tired1, 500 },
    { tired2, 90 },
    { tired3, 500 }, { tired3, 500 }, { tired3, 500 }, { tired3, 500 }, { tired3, 500 }, { tired3, 500 }, { tired3, 500 }, { tired3, 500 },
    { tired2, 20 },
};

const uint8_t anim1_len = sizeof(anim1) / sizeof(anim1[0]);
const uint8_t anim3_len = sizeof(anim3) / sizeof(anim3[0]);
const uint8_t anim4_len = sizeof(anim4) / sizeof(anim4[0]);
const uint8_t anim5_len = sizeof(anim5) / sizeof(anim5[0]);
const uint8_t anim6_len = sizeof(anim6) / sizeof(anim6[0]);
const uint8_t anim7_len = sizeof(anim7) / sizeof(anim7[0]);

void PlayAnimation1(void) //анимация покоя
{
    const Frame *f = &anim1[anim1_frame];

    if(f->image == eye1_1 || f->image == eye1_2) PrintMassDMA_Optimized(f->image, 90, 70, 60+70, 120+70, BG_COLOR);
    else if(f->image == tail_1 || f->image == tail_2 || f->image == tail_3) PrintMassDMA_Optimized(f->image, 80, 60, 60, 120+130, BG_COLOR);
    else PrintMassDMA_Optimized(f->image, 200, 200, 60, 120, BG_COLOR);//60,170

    HAL_DelayMs(f->delay_ms);

    anim1_frame++;
    if (anim1_frame >= anim1_len) anim1_frame = 0;
}

void PlayAnimation2(void) //анимация поглаживания
{
    // Анимация 2 (лиса)
    switch (anim2_frame)
    {
        case 0: PrintMassDMA_Optimized(cute1, 200, 200, 60, 120, BG_COLOR); break;
        case 1: PrintMassDMA_Optimized(cute2, 200, 200, 60, 120, BG_COLOR); break;
        case 2: PrintMassDMA_Optimized(cute3, 200, 200, 60, 120, BG_COLOR); break;
        case 3: PrintMassDMA_Optimized(cute4, 200, 200, 60, 120, BG_COLOR); break;
    }

    anim2_frame = (anim2_frame + 1) % 4;

    // Анимация 5 (сердце)
    switch (anim5_frame)
    {
        case 0: PrintMassDMA_Optimized(heart1, 15, 15, 170, 100, BG_COLOR); break;
        case 1: PrintMassDMA_Optimized(heart2, 15, 15, 170, 100, BG_COLOR); break;
        case 2: PrintMassDMA_Optimized(heart3, 15, 15, 170, 100, BG_COLOR); break;
        case 3: PrintMassDMA_Optimized(heart2, 15, 15, 170, 100, BG_COLOR); break;
    }

    anim5_frame = (anim5_frame + 1) % 4;
}

void PlayAnimation3(void) //анимация для еды
{
    const Frame *f = &anim3[anim3_frame];

    if(f->image == tail_1 || f->image == tail_2 || f->image == tail_3) PrintMassDMA_Optimized(f->image, 80, 60, 60, 120+130, BG_COLOR);
    else PrintMassDMA_Optimized(f->image, 200, 200, 60, 120, BG_COLOR);//60,170

    HAL_DelayMs(f->delay_ms);

    anim3_frame++;
    if (anim3_frame >= anim3_len) anim3_frame = 0;
}

void PlayAnimation4(void) //анимация пережёвывания
{
    while (anim4_frame < anim4_len) {
        const Frame *f = &anim4[anim4_frame];

        PrintMassDMA_Optimized(f->image,45, 25,60 + 93,120 + 133,BG_COLOR);

        HAL_DelayMs(f->delay_ms);
        anim4_frame++;
    }
    anim4_frame = 0;
}

void PlayAnimation5(void) //анимация сна
{
    //PrintMassDMA_Optimized(&fox_stay[0], 200, 200, 60, 170, BG_COLOR);//60,170
    const Frame *f = &anim5[anim5_frame];
    if(f->image == sleep1 || f->image == sleep2 || f->image == sleep3) PrintMassDMA_Optimized(f->image, 180, 165, 80, 147, BG_COLOR);
    if(f->image == boop1 || f->image == boop2 || f->image == boop3) PrintMassDMA_Optimized(f->image, 45, 45, 53, 97, BG_COLOR);
    HAL_DelayMs(f->delay_ms);
    anim5_frame++;
    if (anim5_frame >= anim5_len) anim5_frame = 0;

}

void PlayAnimation6(void) //анимация грусти
{
    const Frame *f = &anim6[anim6_frame];

    if(f->image == sad_eye1 || f->image == sad_eye2) PrintMassDMA_Optimized(f->image, 90, 65, 60+67, 120+75, BG_COLOR);
    else if(f->image == tail_1 || f->image == tail_2 || f->image == tail_3) PrintMassDMA_Optimized(f->image, 80, 60, 60, 120+130, BG_COLOR);
    else PrintMassDMA_Optimized(f->image, 200, 200, 60, 120, BG_COLOR);//60,170

    HAL_DelayMs(f->delay_ms);

    anim6_frame++;
    if (anim6_frame >= anim6_len) anim6_frame = 0;
}

void PlayAnimation7(void) //анимация усталости
{
    const Frame *f = &anim7[anim7_frame];

    if(f->image == tired2 || f->image == tired3) PrintMassDMA_Optimized(f->image, 90, 70, 60+68, 120+68, BG_COLOR);
    else PrintMassDMA_Optimized(f->image, 200, 200, 60, 120, BG_COLOR);//60,170

    HAL_DelayMs(f->delay_ms);

    anim7_frame++;
    if (anim7_frame >= anim7_len) anim7_frame = 0;
}