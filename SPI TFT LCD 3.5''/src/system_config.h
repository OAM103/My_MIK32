#ifndef SYSTEM_COFFIG_H
#define SYSTEM_CONFIG_H

#include "mik32_hal.h"
#include "mik32_hal_spi.h"
#include "mik32_hal_dma.h"

#define CS          GPIO_PIN_4 //p0_4
#define RS          GPIO_PIN_8 //p0_8
#define RESET       GPIO_PIN_9
#define LED         GPIO_PIN_10
#define TFT_WIDTH   480
#define TFT_HEIGHT  320
#define DMA_BUFFER_SIZE (TFT_WIDTH * 3) // Размер буфера для одной строки

// Два буфера для построчной двойной буферизации
static uint16_t lineBuffer1[TFT_WIDTH];
static uint16_t lineBuffer2[TFT_WIDTH];

extern uint8_t dma_buffer[DMA_BUFFER_SIZE]; // Статический буфер

extern SPI_HandleTypeDef hspi0;
extern SPI_HandleTypeDef hspi1;
extern DMA_InitTypeDef hdma;
extern DMA_ChannelHandleTypeDef hdma_ch0;

void GPIO_Init();
void SystemClock_Config(void);
void SPI0_Init(void);
void SPI1_Init(void);
void DMA_Init(void);
static void DMA_CH0_Init(DMA_InitTypeDef *hdma);

#endif