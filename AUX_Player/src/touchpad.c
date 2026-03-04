#include "touchpad.h"
#include "lcd.h"
#include "system_config.h"
#include "mik32_hal.h"
#include "mik32_hal_spi.h"
#include "xprintf.h"

#define READ_X  0xD0
#define READ_Y  0x90


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
        //HAL_DelayMs(2); // небольшая задержка между измерениями
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

int process_touchpad(void)
{
    int x_raw = read_filtered_coordinate(READ_X, 5);
    int y_raw = read_filtered_coordinate(READ_Y, 5);

    // Нет касания
    if (x_raw <= 0 || y_raw >= 2032) {
        return -1;
    }

    coord_x = transform(x_raw, 2032, 150, 320, 0);
    coord_y = transform(y_raw, 2032, 100, 480, 0);

    if (coord_y >= 480 - WINDOW_H && coord_y <= 480 - WINDOW_Y) {
        xprintf("coord_y: %d\n", coord_y);
        xprintf("coord_x: %d\n", coord_x);
        return coord_y;
    }
    else return 500;
}
