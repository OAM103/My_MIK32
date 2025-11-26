#include "system_config.h"
#include "uart_lib.h"
#include "xprintf.h"
#include "slot.h"
#include "mik32_hal_irq.h"
#include "joystick.h"
#include "system_config.h"

#define GPIO_BT GPIO_0
#define j_but GPIO_PIN_14

uint8_t master_output_ch6[] = {0b00000111, 0b10000000, 0};
uint8_t master_output_ch7[] = {0b00000111, 0b11000000, 0};
uint8_t master_input_x[3];
uint8_t master_input_y[3];

uint8_t button = 0;
uint8_t lastbutton = 0;

uint16_t axisX, axisY = 0;
HAL_StatusTypeDef SPI_Status;

void Joystick()
{

    if(button!=lastbutton){
        if(button != 1) {
            xprintf("ON\n");
        }else{
            xprintf("OFF\n");
        }
        lastbutton = button;
    }

    /* Ось х */
    __HAL_SPI_ENABLE(&hspi1);
    HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS0, __HIGH); // Выбрать внешний АЦП CS

    /* Передача и прием данных */
    SPI_Status = HAL_SPI_Exchange(&hspi1, master_output_ch6, master_input_x, 3, SPI_TIMEOUT_DEFAULT);
    if (SPI_Status != HAL_OK)
    {
        xprintf("SPI_Error %d, OVR %d, MODF %d\n", SPI_Status, hspi1.ErrorCode & HAL_SPI_ERROR_OVR, hspi1.ErrorCode & HAL_SPI_ERROR_MODF);
        HAL_SPI_ClearError(&hspi1);
    }

    HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS0, __LOW); // Выбрать внешний АЦП CS
    __HAL_SPI_DISABLE(&hspi1);

    axisX = ((master_input_x[1] << 8) | master_input_x[0]) & 0b1111111111111;

    for (volatile uint32_t i = 0; i < 1000; i++);
    
    /* Ось y */
    __HAL_SPI_ENABLE(&hspi1);
    HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS0, __HIGH); // Выбрать внешний АЦП CS

    /* Передача и прием данных */
    SPI_Status = HAL_SPI_Exchange(&hspi1, master_output_ch7, master_input_y, 3, SPI_TIMEOUT_DEFAULT);
    if (SPI_Status != HAL_OK)
    {
        xprintf("SPI_Error %d, OVR %d, MODF %d\n", SPI_Status, hspi1.ErrorCode & HAL_SPI_ERROR_OVR, hspi1.ErrorCode & HAL_SPI_ERROR_MODF);
        HAL_SPI_ClearError(&hspi1);
    }

    HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS0, __LOW); // Выбрать внешний АЦП CS
    __HAL_SPI_DISABLE(&hspi1);

    axisY = ((master_input_y[1] << 8) | master_input_y[0]) & 0b1111111111111;

    // xprintf("X = %04d", axisX);
    // xprintf(", Y = %04d\n", axisY);
    // if (axisX >> 12 != 0)
    // {
    //     xprintf("X = error");
    // }
    // else
    // {
    //     xprintf("X = %04d", axisX);
    // }
    
    // if (axisY >> 12 != 0)
    // {
    //     xprintf(", Y = error\n");
    // }
    // else
    // {
    //     xprintf("Y = %04d\n", axisY);
    // }

}

void trap_handler()
{
    // xprintf("1");
    if (EPIC_CHECK_GPIO_IRQ())
    {
        if(HAL_GPIO_LineInterruptState(GPIO_LINE_6)){
            if(button == 0){
                button = 1;
            }else{
                button = 0;
            }
            GPIO_IRQ->CLEAR = 1 << 6;
            return;
        }
    }
}