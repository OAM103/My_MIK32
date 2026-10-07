/**
 * @file    bus_spi.h
 * @brief   Драйвер SPI
 */

#ifndef BUS_SPI_H
#define BUS_SPI_H

/* Includes ------------------------------------------------------------------*/
#include "mik32_hal_spi.h"
#include "xprintf.h"

/* Exported variables --------------------------------------------------------*/
extern SPI_HandleTypeDef hspi0;
extern SPI_HandleTypeDef hspi1;

/* Exported functions --------------------------------------------------------*/
/**
 * @brief  Инициализация SPI0 (дисплей) в режиме Master
 */
void SPI0_Init(void);

/**
 * @brief  Инициализация SPI1 (тачпад) в режиме Master
 */
void SPI1_Init(void);

#endif /* __BUS_SPI_H__ */
