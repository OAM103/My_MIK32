#include "joystick.h"
#include "system_config.h"
#include "xprintf.h"
#include "mik32_hal.h"
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

uint8_t master_output_ch6[] = {0b00000111, 0b10000000, 0};
uint8_t master_output_ch7[] = {0b00000111, 0b11000000, 0};
uint8_t master_input_x[3];
uint8_t master_input_y[3];

volatile  uint8_t button = 0;
volatile  uint8_t lastbutton = 0;

uint8_t cube_update = 0; // флаг нужно ли перерисовать куб
float angle_x = 0.0f;
float angle_y = 0.0f;

#define FILTER_SIZE 6
#define DEAD_ZONE   80

void Joystick_Init(void)
{
    static uint16_t xBuffer[FILTER_SIZE] = {2048};
    static uint16_t yBuffer[FILTER_SIZE] = {2048};
    static uint8_t index = 0;
    static uint16_t lastX = 2048;
    static uint16_t lastY = 2048;

    uint16_t rawX = 0, rawY = 0;
    HAL_StatusTypeDef SPI_Status;

    //  Чтение X  
    __HAL_SPI_ENABLE(&hspi1);
    HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS0, __HIGH);

    SPI_Status = HAL_SPI_Exchange(&hspi1, master_output_ch6, master_input_x, 3, SPI_TIMEOUT_DEFAULT);
    if (SPI_Status == HAL_OK)
        rawX = ((master_input_x[1] << 8) | master_input_x[0]) & 0x1FFF;

    HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS0, __LOW);
    __HAL_SPI_DISABLE(&hspi1);

    //  Короткая пауза  
    HAL_DelayMs(5);

    //  Чтение Y  
    __HAL_SPI_ENABLE(&hspi1);
    HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS0, __HIGH);

    SPI_Status = HAL_SPI_Exchange(&hspi1, master_output_ch7, master_input_y, 3, SPI_TIMEOUT_DEFAULT);
    if (SPI_Status == HAL_OK)
        rawY = ((master_input_y[1] << 8) | master_input_y[0]) & 0x1FFF;

    HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS0, __LOW);
    __HAL_SPI_DISABLE(&hspi1);

    //  Фильтр скользящего среднего  
    xBuffer[index] = rawX;
    yBuffer[index] = rawY;
    index = (index + 1) % FILTER_SIZE;

    uint32_t sumX = 0, sumY = 0;
    for (uint8_t i = 0; i < FILTER_SIZE; i++) {
        sumX += xBuffer[i];
        sumY += yBuffer[i];
    }

    int16_t avgX = sumX / FILTER_SIZE;
    int16_t avgY = sumY / FILTER_SIZE;

    //  Изменение координат  
    int16_t dX = (int16_t)avgX - (int16_t)lastX;
    int16_t dY = (int16_t)avgY - (int16_t)lastY;

    lastX = avgX;
    lastY = avgY;

    if (abs(dX) < DEAD_ZONE) dX = 0;
    if (abs(dY) < DEAD_ZONE) dY = 0;

    //  Обновление куба  
    if (dX != 0 || dY != 0) {
        angle_x += dY * 0.001f;
        angle_y += dX * 0.001f;
        cube_update = 1;
        xprintf("Joystick: X=%04d, Y=%04d, dX=%d, dY=%d\n", lastX, lastY, dX, dY);
    } else {
        cube_update = 0;
    }

    //  Кнопка  
    if (button != lastbutton) {
        xprintf("Button: %s\n", button ? "OFF" : "ON");
        lastbutton = button;
    }
}


// void Joystick_Init(void)
// {
//     static uint16_t lastX = 0, lastY = 0;
//     static uint8_t firstRead = 1;
//     HAL_StatusTypeDef SPI_Status;

//     uint16_t axisX = 0, axisY = 0;

//     //   Чтение оси X  
//     __HAL_SPI_ENABLE(&hspi1);
//     HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS0, __HIGH);

//     SPI_Status = HAL_SPI_Exchange(&hspi1, master_output_ch6, master_input_x, 3, SPI_TIMEOUT_DEFAULT);
//     if (SPI_Status != HAL_OK)
//     {
//         xprintf("SPI_X_Error %d\n", SPI_Status);
//         HAL_SPI_ClearError(&hspi1);
//     }

//     HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS0, __LOW);
//     __HAL_SPI_DISABLE(&hspi1);

//     axisX = ((master_input_x[1] << 8) | master_input_x[0]) & 0x0FFF; // 12 бит данных

//     // Задержка для стабильности  
//     for (volatile uint32_t i = 0; i < 500; i++);

//     //   Чтение оси Y  
//     __HAL_SPI_ENABLE(&hspi1);
//     HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS0, __HIGH);

//     SPI_Status = HAL_SPI_Exchange(&hspi1, master_output_ch7, master_input_y, 3, SPI_TIMEOUT_DEFAULT);
//     if (SPI_Status != HAL_OK)
//     {
//         xprintf("SPI_Y_Error %d\n", SPI_Status);
//         HAL_SPI_ClearError(&hspi1);
//     }

//     HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS0, __LOW);
//     __HAL_SPI_DISABLE(&hspi1);

//     axisY = ((master_input_y[1] << 8) | master_input_y[0]) & 0x0FFF;

//     //   Проверка корректности  
//     if (axisX >> 12 != 0 || axisY >> 12 != 0)
//     {
//         xprintf("ADC_Error\n");
//         cube_update = 0;
//         return;
//     }

//     //   Инициализация первого чтения  
//     if (firstRead)
//     {
//         lastX = axisX;
//         lastY = axisY;
//         firstRead = 0;
//         cube_update = 0;
//         return;
//     }

//     //   Вычисляем изменения  
//     int16_t dx = (int16_t)axisX - (int16_t)lastX;
//     int16_t dy = (int16_t)axisY - (int16_t)lastY;

//     //   Зона "мертвого" центра (чтобы куб не дрожал)  
//     if (abs(dx) < 10 && abs(dy) < 10)
//     {
//         cube_update = 0;
//         return;
//     }

//     //   Масштабируем вращение куба  
//     angle_y += dx * 0.0005f;  // Чем меньше коэффициент — тем плавнее
//     angle_x += dy * 0.0005f;

//     //   Ограничиваем углы 
//     if (angle_x > 6.28f) angle_x -= 6.28f;
//     if (angle_x < 0.0f)  angle_x += 6.28f;
//     if (angle_y > 6.28f) angle_y -= 6.28f;
//     if (angle_y < 0.0f)  angle_y += 6.28f;

//     //   Обновляем  
//     lastX = axisX;
//     lastY = axisY;
//     cube_update = 1;

//     xprintf("Joystick: X=%04d, Y=%04d, dX=%d, dY=%d\n", axisX, axisY, dx, dy);
// }