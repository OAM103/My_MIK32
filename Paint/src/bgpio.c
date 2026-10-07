/**
 * @file    bgpio.c
 * @brief   Реализация драйвера GPIO 
 */
/* Includes ------------------------------------------------------------------*/
#include "bgpio.h"

/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Инициализация выводов GPIO дисплея и шины SPI
 */
void GPIO_Init(void) {
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = CS;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    GPIO_InitStruct.DS = HAL_GPIO_DS_8MA;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = RS;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = RESET;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = LED;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = SLAVE_CS0 | SLAVE_CS1 | SLAVE_CS2;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS_ALL, 0);
    HAL_GPIO_WritePin(GPIO_0, RESET , 1);
    HAL_GPIO_WritePin(GPIO_0, LED , 1);
    
    if (HAL_GPIO_Init(GPIO_CS, &GPIO_InitStruct) != HAL_OK)
    {
        xprintf("SPI_Init_Error\n");
    }
}
