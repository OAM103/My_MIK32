/**
 * @file    bdma.c
 * @brief   Реализация драйвера DMA
 */

/* Includes ------------------------------------------------------------------*/
#include "bdma.h"

/* Private variables ---------------------------------------------------------*/
DMA_InitTypeDef hdma;
DMA_ChannelHandleTypeDef hdma_ch0;

static uint8_t dma_buffer[DMA_BUFFER_SIZE];
uint8_t *DMA_BUF;

/**
 * @brief       Привязка внешнего буфера для DMA-передачи строк
 * @param  buf  Указатель на буфер размером DMA_BUFFER_SIZE
 */
void set_dma_buf(uint8_t *buf)
{
    DMA_BUF = buf;
}

/**
 * @brief  Инициализация контроллера DMA
 */
void DMA_Init(void)
{
    hdma.Instance = DMA_CONFIG;
    hdma.CurrentValue = DMA_CURRENT_VALUE_ENABLE;
    if (HAL_DMA_Init(&hdma) != HAL_OK)
    {
        xprintf("DMA_Init Error\n");
    }

    DMA_CH0_Init(&hdma);

    set_dma_buf(dma_buffer);
}

/**
 * @brief       Инициализация канала CH0 
 * @param  hdma Обработчик DMA
 */
void DMA_CH0_Init(DMA_InitTypeDef *hdma)
{
    hdma_ch0.dma = hdma;
    hdma_ch0.ChannelInit.Channel = DMA_CHANNEL_0;
    hdma_ch0.ChannelInit.Priority = DMA_CHANNEL_PRIORITY_VERY_HIGH;
    hdma_ch0.ChannelInit.ReadMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_ch0.ChannelInit.ReadInc = DMA_CHANNEL_INC_ENABLE;
    hdma_ch0.ChannelInit.ReadSize = DMA_CHANNEL_SIZE_BYTE;
    hdma_ch0.ChannelInit.ReadBurstSize = 0;
    hdma_ch0.ChannelInit.ReadRequest = DMA_CHANNEL_SPI_0_REQUEST;
    hdma_ch0.ChannelInit.ReadAck = DMA_CHANNEL_ACK_DISABLE;
    hdma_ch0.ChannelInit.WriteMode = DMA_CHANNEL_MODE_PERIPHERY;
    hdma_ch0.ChannelInit.WriteInc = DMA_CHANNEL_INC_DISABLE;
    hdma_ch0.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_BYTE;
    hdma_ch0.ChannelInit.WriteRequest = DMA_CHANNEL_SPI_0_REQUEST;
    hdma_ch0.ChannelInit.WriteAck = DMA_CHANNEL_ACK_DISABLE;
}
