#ifndef SYSTEM_COFFIG_H
#define SYSTEM_CONFIG_H

#include "inttypes.h"
#include <stdlib.h>

#include "mik32_hal.h"
#include "mik32_hal_spi.h"
#include "mik32_hal_dma.h"
#include "mik32_hal_pcc.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_timer32.h"
#include "mik32_hal_dac.h"

#define CS          GPIO_PIN_4 //p0_4
#define RS          GPIO_PIN_8 //p0_8
#define RESET       GPIO_PIN_9
#define LED         GPIO_PIN_10
#define TFT_WIDTH   480
#define TFT_HEIGHT  320
#define DMA_BUFFER_SIZE (TFT_WIDTH * 3) // Размер буфера для одной строки

#define GPIO_BT GPIO_0
#define j_but GPIO_PIN_14
#define GPIO_CS GPIO_1
#define SLAVE_CS0 GPIO_PIN_10
#define SLAVE_CS1 GPIO_PIN_11
#define SLAVE_CS2 GPIO_PIN_14
#define SLAVE_CS_ALL (SLAVE_CS2 | SLAVE_CS1 | SLAVE_CS0)

#define GPIO_ENC GPIO_1
#define GPIO_POT0 GPIO_2
#define POT0_INC GPIO_PIN_7
#define POT0_UP_DOWN GPIO_PIN_6
#define GPIO_KEYBOARD GPIO_1

#define ENC_A GPIO_PIN_0
#define ENC_B GPIO_PIN_1
#define ENC_A_MASK 0b01
#define ENC_B_MASK 0b10

#define TIMER_CLK 64000000UL  // 64 МГц 
#define SAMPLE_RATE 32000  // частота воспроизведения

// Два буфера для построчной двойной буферизации
extern uint16_t lineBuffer1[TFT_WIDTH];
extern uint16_t lineBuffer2[TFT_WIDTH];

extern uint8_t* DMA_BUF; // Статический буфер

extern SPI_HandleTypeDef hspi0;
extern DMA_InitTypeDef hdma;
extern DMA_ChannelHandleTypeDef hdma_ch0;
extern TIMER32_HandleTypeDef htimer32;
extern TIMER32_CHANNEL_HandleTypeDef htimer32_channel;
extern DAC_HandleTypeDef hdac1;
extern DAC_HandleTypeDef hdac2;

void set_dma_buf(uint8_t* MASS);
void GPIO_Init();
void SystemClock_Config(void);
void SPI0_Init(void);
void SPI1_Init(void);
void DMA_Init(void);
void Timer16_1_Init(void);
void DMA_CH0_Init(DMA_InitTypeDef *hdma);
void DMA_CH1_Init(DMA_InitTypeDef *hdma);
void Timer32_Init(void);
void Timer32_Init_For_Audio(void);
void DAC_Init(void);
void DAC2_Init(void);

#endif

