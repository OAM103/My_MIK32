#include "mik32_hal_gpio.h"
#include <mik32_memory_map.h>
#include <pad_config.h>
#include <gpio.h>
#include <power_manager.h>
#include <wakeup.h>

void GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);

}

#define BOARD_LITE
// #define BOARD_DIP
#ifdef BOARD_LITE
#define PIN_LED 3     // Светодиод управляется выводом PORT_2_7
#define PIN_BUTTON 8 // Кнопка управляет сигналом на выводе PORT_2_6
#endif
void InitClock()
{
    PM->CLK_APB_P_SET |= PM_CLOCK_APB_P_UART_0_M | PM_CLOCK_APB_P_GPIO_0_M | PM_CLOCK_APB_P_GPIO_1_M | PM_CLOCK_APB_P_GPIO_2_M; // включение тактирования GPIO
    PM->CLK_APB_M_SET |= PM_CLOCK_APB_M_PAD_CONFIG_M | PM_CLOCK_APB_M_WU_M | PM_CLOCK_APB_M_PM_M;                                // включение тактирования блока для смены режима выводов
}


void ledBlink()
{
#ifdef BOARD_LITE
    GPIO_0->OUTPUT ^= 0 << PIN_LED; // Установка сигнала вывода 7 порта 2 в противоположный уровень
    for (volatile int i = 0; i < 100000; i++)
        ;
#endif
}
#ifdef BOARD_LITE


void ledButton()
{
    if (GPIO_0->STATE & (1 << PIN_BUTTON))
    {
        GPIO_0->OUTPUT |= 1 << PIN_LED; // Установка сигнала вывода 7 порта 2 в высокий уровень
    }
    else
    {
        GPIO_0->OUTPUT &= ~(1 << PIN_LED); // Установка сигнала вывода 7 порта в низкий уровень
    }
}
#endif



int main()
{
    InitClock(); // Включение тактирования GPIO
#ifdef BOARD_LITE
#ifdef MIK32V2
    PAD_CONFIG->PORT_0_CFG &= ~(0b11 << (2 * PIN_LED));    // Установка вывода 7 порта 2 в режим GPIO
    PAD_CONFIG->PORT_0_CFG &= ~(0b11 << (2 * PIN_BUTTON)); // Установка вывода 6 порта 2 в режим GPIO
    GPIO_0->DIRECTION_OUT = 1 << PIN_LED;    // Установка направления вывода 7 порта 2 на выход
    GPIO_0->DIRECTION_IN = 1 << PIN_BUTTON; // Установка направления вывода 6 порта 2 на вход
#endif
#endif
    GPIO_Init();
   
    while (1)
    {
        HAL_GPIO_TogglePin(GPIO_0, GPIO_PIN_3);
        ledBlink(); /* Светодиод мигает */
#ifdef BOARD_LITE
        ledButton(); /* Светодиод зажигается при нажатой кнопке */
#endif
    }
}