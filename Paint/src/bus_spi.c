/**
 * @file   bus_spi.c
 * @brief  Реализация драйвера SPI
 */

/* Includes ------------------------------------------------------------------*/
#include "bus_spi.h"

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi0;
SPI_HandleTypeDef hspi1;

/* Private functions ---------------------------------------------------------*/
/**
 * @brief        Инициализация SPI0 в режиме Master
 */
void SPI0_Init(void) {
    hspi0.Instance = SPI_0;

    /* Режим SPI */
    hspi0.Init.SPI_Mode = HAL_SPI_MODE_MASTER;

    /* Настройки */
    hspi0.Init.CLKPhase = SPI_PHASE_ON;
    hspi0.Init.CLKPolarity = SPI_POLARITY_HIGH;
    hspi0.Init.ThresholdTX = 4;

    /* Настройки для ведущего */
    hspi0.Init.BaudRateDiv = 0;
    hspi0.Init.Decoder = SPI_DECODER_NONE;
    hspi0.Init.ManualCS = SPI_MANUALCS_OFF;
    hspi0.Init.ChipSelect = SPI_CS_0;

    if (HAL_SPI_Init(&hspi0) != HAL_OK) {
        xprintf("SPI_Init_Error\n");
    }
}

/**
 * @brief        Инициализация SPI1 в режиме Master
 */
void SPI1_Init(void)
{
    hspi1.Instance = SPI_1;

    /* Режим SPI */
    hspi1.Init.SPI_Mode = HAL_SPI_MODE_MASTER;

    /* Настройки */
    hspi1.Init.CLKPhase = SPI_PHASE_ON;
    hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
    hspi1.Init.ThresholdTX = 4;

    /* Настройки для ведущего */
    hspi1.Init.BaudRateDiv = SPI_BAUDRATE_DIV8;
    hspi1.Init.Decoder = SPI_DECODER_NONE;
    hspi1.Init.ManualCS = SPI_MANUALCS_ON;
    hspi1.Init.ChipSelect = SPI_CS_0;      

    if (HAL_SPI_Init(&hspi1) != HAL_OK)
    {
        xprintf("SPI_Init_Error\n");
    }
}