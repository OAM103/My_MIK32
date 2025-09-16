#include "mik32_hal_gpio.h"

void GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
    HAL_GPIO_Init(GPIO_1, &GPIO_InitStruct);
}
int main()
{

    GPIO_Init();
    while (1)
    {
        HAL_GPIO_TogglePin(GPIO_0, GPIO_PIN_3);
        HAL_GPIO_TogglePin(GPIO_1, GPIO_PIN_3);
        HAL_DelayMs(500);
    }
}