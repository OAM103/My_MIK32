/**
 * @file    bgpio.h
 * @brief   Драйвер GPIO
 */

#ifndef BGPIO_H
#define BGPIO_H

/* Includes ------------------------------------------------------------------*/
#include "mik32_hal_gpio.h"
#include "xprintf.h"

/* Define --------------------------------------------------------------------*/
#define CS            GPIO_PIN_4
#define RS            GPIO_PIN_8
#define RESET         GPIO_PIN_9
#define LED           GPIO_PIN_10

#define GPIO_CS       GPIO_1
#define SLAVE_CS0     GPIO_PIN_10
#define SLAVE_CS1     GPIO_PIN_11
#define SLAVE_CS2     GPIO_PIN_14
#define SLAVE_CS_ALL  (SLAVE_CS2 | SLAVE_CS1 | SLAVE_CS0)

/* Exported functions --------------------------------------------------------*/
/**
 * @brief  Инициализация выводов GPIO дисплея и шины SPI
 */
void GPIO_Init(void);

#endif /* __BGPIO_H__ */
