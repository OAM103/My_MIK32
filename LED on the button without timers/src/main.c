#include "mik32_hal_gpio.h"
#include <gpio.h>

void GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0}; // структура для конфигурации пина
    __HAL_PCC_GPIO_0_CLK_ENABLE(); //тактирование блока GPIO_0 для пинов
    GPIO_InitStruct.Pin = GPIO_PIN_3;//выбираем 3 пин
    
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);//GPIO_0 на выход

}

void ledBlink()
{
    GPIO_0->OUTPUT ^= 0 << 3; //если выключен, то зажигается, для обратного нужна вместо 0 единица
    HAL_DelayMs(100);//светодиод мигает, и чтобы была видна разница, то делаем задержку
}

void ledButton()
{
    //проверяем состояние кнопки на 8 пине
    if (GPIO_0->STATE & (1 << 8)) GPIO_0->OUTPUT |= 1 << 3; //нажата,т.е.высокий пин 8, включаем светодиод,т.е. пин 3 низкий
    else GPIO_0->OUTPUT &= ~(1 << 3); 

}

int main()
{
    
    GPIO_Init();
    while (1)
    {
        HAL_GPIO_TogglePin(GPIO_0, GPIO_PIN_3);
        ledBlink(); //даёт задержку, инвертирует пин
        ledButton(); //проверяет кнопку и зажигает светодиод
    }
}