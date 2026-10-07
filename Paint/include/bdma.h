/**
 * @file    bdma.h
 * @brief   Драйвер DMA 
 */

#ifndef BDMA_H
#define BDMA_H

/* Includes ------------------------------------------------------------------*/
#include "mik32_hal_dma.h"
#include "xprintf.h"

/* Define --------------------------------------------------------------------*/
#define TFT_WIDTH   480                 /* Ширина экрана */
#define TFT_HEIGHT  320                 /* Высота экрана */
#define DMA_BUFFER_SIZE (TFT_WIDTH * 3) /* Размер буфера для одной строки */

/* Exported variables --------------------------------------------------------*/
extern uint8_t *DMA_BUF;
extern DMA_InitTypeDef hdma;
extern DMA_ChannelHandleTypeDef hdma_ch0;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief       Привязка внешнего буфера для DMA-передачи строк
 * @param  buf  Указатель на буфер размером DMA_BUFFER_SIZE
 */
void set_dma_buf(uint8_t *buf);

/**
 * @brief       Инициализация контроллера DMA
 */
void DMA_Init(void);

/**
 * @brief       Инициализация канала CH0 
 * @param  hdma Обработчик DMA
 */
void DMA_CH0_Init(DMA_InitTypeDef *hdma);

#endif /* __BDMA_H__ */
