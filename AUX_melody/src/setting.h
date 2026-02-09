#ifndef SETTING_H
#define SETTING_H
#include "inttypes.h"
#include <stdlib.h>

#include "mik32_hal_pcc.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal.h"
#include "mik32_hal_timer32.h"
#include "mik32_hal_dma.h"
#include "mik32_hal_dac.h"

#define GPIO_CS GPIO_1
#define GPIO_ENC GPIO_1
#define GPIO_POT0 GPIO_2

#define POT0_INC GPIO_PIN_7
#define POT0_UP_DOWN GPIO_PIN_6

#define SLAVE_CS0 GPIO_PIN_10
#define SLAVE_CS1 GPIO_PIN_11
#define SLAVE_CS2 GPIO_PIN_14
#define SLAVE_CS_ALL (SLAVE_CS2 | SLAVE_CS1 | SLAVE_CS0)

#define ENC_A GPIO_PIN_4
#define ENC_B GPIO_PIN_6
#define ENC_A_MASK 0b01
#define ENC_B_MASK 0b10

#define TIMER_CLK 64000000UL  // 64 МГц - тональность
#define SAMPLE_RATE    48000

extern TIMER32_HandleTypeDef htimer32;
extern TIMER32_CHANNEL_HandleTypeDef htimer32_channel;
extern DMA_InitTypeDef hdma;
extern DMA_ChannelHandleTypeDef hdma_ch0;
extern DMA_ChannelHandleTypeDef hdma_ch1;
extern DAC_HandleTypeDef hdac1;
extern DAC_HandleTypeDef hdac2;

void SystemClock_Config();
void GPIO_Init();
void Timer32_Init(void);
void DMA_CH0_Init(DMA_InitTypeDef *hdma);
void DMA_CH1_Init(DMA_InitTypeDef *hdma);
void DMA_Init(void);
void DAC_Init(void);
void DAC2_Init(void);

#endif