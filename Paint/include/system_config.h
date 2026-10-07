
/**
 * @file   system_config.h
 * @brief  Базовая конфигурация MIK32 Амур
 */ 

#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "mik32_hal.h"
#include "xprintf.h"

/* Exported function prototypes -----------------------------------------------*/
/**
 * @brief Конфигурация источника тактового сигнала
 */
void SystemClock_Config(void);

#endif /* SYSTEM_CONFIG_H */