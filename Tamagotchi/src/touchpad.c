#include "touchpad.h"
#include "lcd.h"
#include "system_config.h"
#include "mik32_hal.h"
#include "mik32_hal_spi.h"
#include "sprite.h"
#include "xprintf.h"

#define READ_X  0xD0
#define READ_Y  0x90

//const uint16_t *cute_images[4] = {&cute1[0], &cute2[0], &cute3[0], &cute4[0]};
//const uint16_t *stay_images[4] = {&fox_stay[0], &eye1[0], &eye1[0], &eye1[0], &tail1[0], &tail2[0], &tail3[0]};

int read_coordinate(unsigned char command) {
    int16_t value;

    // Подготовка пакета (Команда + 2 байта для приема)
    uint8_t master_output[] = {command, 0x00, 0x00}; //команда  и 2 байта для приёма данных. Тачпад отправляет12-битные координваты, упакованные в 2 байта
    uint8_t master_input[3] = {0}; // Инициализируем нулями

    // Обмен данными по SPI (Блокирующий режим)
    HAL_StatusTypeDef SPI_Status = HAL_SPI_Exchange(&hspi1, master_output, master_input, sizeof(master_output), SPI_TIMEOUT_DEFAULT); //отправляет команду и одновременно принимает данные от тачпада
    
    if (SPI_Status != HAL_OK) {
        xprintf("SPI_Error %d, OVR %d, MODF %d\n", SPI_Status, hspi0.ErrorCode & HAL_SPI_ERROR_OVR, hspi0.ErrorCode & HAL_SPI_ERROR_MODF);
        HAL_SPI_ClearError(&hspi0);
        return -1; // Возвращаем ошибку
    }

    value = (master_input[1] << 4) | (master_input[2] >> 4); 

    return value;
}

int read_filtered_coordinate(uint8_t command, uint8_t samples) {
    int32_t sum = 0;
    uint8_t valid = 0;

    for (uint8_t i = 0; i < samples; i++) {
        int val = read_coordinate(command);
        if (val > 0) { // только корректные значения
            sum += val;
            valid++;
        }
        HAL_DelayMs(2); // небольшая задержка между измерениями
    }

    if (valid == 0)
        return -1;

    return sum / valid; // усреднённое значение
}

uint16_t transform(uint16_t value, uint16_t r1_max, uint16_t r1_min, uint16_t r2_max, uint16_t r2_min){//value - коорд. тачпада, которую нужно преобраз., мин. и макс. знач координат с тачпада, мин. макс. знач с экрана 
  uint16_t scale = (r2_max - r2_min) * 1000 / (r1_max - r1_min) ;
  uint16_t res = (value - r1_min) * scale / 1000; //преобраз. коорд с тачпада в коорд. с экрана
  return res;
}

volatile int coord_x = -1;
volatile int coord_y = -1;

// int process_touchpad(void)
// {
//     int x_raw = read_filtered_coordinate(READ_X, 5);
//     int y_raw = read_filtered_coordinate(READ_Y, 5);

//     if (x_raw <= 0 || y_raw >= 2032)
//         return 0;

//     coord_x = transform(x_raw, 2032, 150, 320, 0);
//     coord_y = transform(y_raw, 2032, 100, 480, 0);

//     // проверка
//     if (coord_x >= 50 && coord_x <= 50 + 200 && coord_y >= 175 && coord_y <= 200 + 165) return 1;   // тач по картинке
//     if (coord_x >= 65 && coord_x <= 100 && coord_y >= 20 && coord_y <= 70)  return 3;   // тач по яйцу
//     if (coord_x >= 125 && coord_x <= 170 && coord_y >= 10 && coord_y <= 70) return 4;   // тач по курице
//     if (coord_x >= 195 && coord_x <= 285 && coord_y >= 10 && coord_y <= 70) return 5;   // тач по рыбе
//     return 2;       // тач мимо
// }
#define SWIPE_Y_THRESHOLD 270 //отклонение от траектории
#define SWIPE_X_TOLERANCE 50   //отклонение от траектории

static int swipe_active = 0;
static int swipe_start_x = 0;
static int swipe_start_y = 0;

int process_touchpad(void)
{
    int x_raw = read_filtered_coordinate(READ_X, 5);
    int y_raw = read_filtered_coordinate(READ_Y, 5);

    // Нет касания
    if (x_raw <= 0 || y_raw >= 2032) {
        swipe_active = 0; // сброс жеста
        return 0;
    }

    coord_x = transform(x_raw, 2032, 150, 320, 0);
    coord_y = transform(y_raw, 2032, 100, 480, 0);

    //Обработка свайпа
    if (!swipe_active) {
        // начало касания
        swipe_active = 1;
        swipe_start_x = coord_x;
        swipe_start_y = coord_y;
    } else {
        int dy = swipe_start_y - coord_y;
        int dx = coord_x - swipe_start_x;

        if (dy > SWIPE_Y_THRESHOLD && abs(dx) < SWIPE_X_TOLERANCE) {
            swipe_active = 0;
            return 6;                                                                 // свайп вниз
        }
    }

    // Обычные тачи
    if (coord_x >= 50 && coord_x <= 250 && coord_y >= 175 && coord_y <= 340) return 1; // тач по картинке
    if (coord_x >= 65 && coord_x <= 100 && coord_y >= 20 && coord_y <= 70)   return 3; // тач по яйцу
    if (coord_x >= 125 && coord_x <= 170 && coord_y >= 10 && coord_y <= 70)  return 4; // тач по курице
    if (coord_x >= 195 && coord_x <= 285 && coord_y >= 10 && coord_y <= 70)  return 5; // тач по рыбе

    return 2;                                                                          // тач мимо
}