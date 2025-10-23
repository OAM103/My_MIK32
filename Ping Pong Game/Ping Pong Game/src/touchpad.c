#include "touchpad.h"
#include "lcd.h"
#include "ball.h"
#include "brick.h"
#include "system_config.h"
#include "mik32_hal.h"
#include "mik32_hal_spi.h"
#include "xprintf.h"
#include <stdbool.h>

//extern SPI_HandleTypeDef hspi0;
//extern SPI_HandleTypeDef hspi1;

#define READ_X  0xD0
#define READ_Y  0x90
#define PLATFORM_TOUCH_RADIUS 60

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

    /*Комбинирование 12 бит данных

    Байты 1 и 2 содержат 16 бит, из которых 12 бит - это полезная нагрузка (координата).
    Обычно 4 младших бита первого байта бесполезны, а 4 старших бита второго байта бесполезны.
  
    Собираем 12 бит: (master_input[1] << 4) | (master_input[2] >> 4)
    
    value = ((master_input[1] & 0x0F) << 8) | master_input[2]; // Простая сборка 16 бит

    Старший байт (master_input[1]) содержит 4 старших бита
    Младший байт (master_input[2]) содержит 8 младших бит*/

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
        HAL_DelayMs(3); // небольшая задержка между измерениями
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


void process_touchpad(){
    int x, y;
    int coord_x, coord_y;

    // x = read_coordinate(READ_X);
    // y = read_coordinate(READ_Y);
    x = read_filtered_coordinate(READ_X, 5);
    y = read_filtered_coordinate(READ_Y, 5);
    
    if(x >  0 && y <2032){
        if(x < 150)x = 150;
        if(y < 100)y = 100;

        static int last_x = 0, last_y = 0;

        if (abs(x - last_x) < 20 && abs(y - last_y) < 20) return; // фильтр мелкитх движений
        last_x = x;
        last_y = y;
        // "устойчивая" точка — принимаем
        coord_x = transform(x, 2032, 150, 320, 0);
        coord_y = transform(y, 2032, 100, 480, 0);
        //rect(30, 10 , coord_y, coord_x, 0x0000);
        //drawCircle(coord_x, coord_y, 15, 0x0000);
    }
}


// // начальные параметры платформы
// Platform right_platform = {320 - 15 - 30, 160, 15, 70};
// void process_platform_touch(const uint16_t PLATFORM_COLOR, const uint16_t BG_COLOR) {

//     if(game_over) return;

//     rect(right_platform.height, right_platform.width,right_platform.y - right_platform.height/2,right_platform.x - right_platform.width/2,PLATFORM_COLOR);

//     int x_raw, y_raw;
//     int coord_x, coord_y;

//     x_raw = read_filtered_coordinate(READ_X, 5);
//     y_raw = read_filtered_coordinate(READ_Y, 5);

//     if (x_raw > 0 && y_raw < 2032) {
//         if (x_raw < 150) x_raw = 150;
//         if (y_raw < 100) y_raw = 100;

//         static int last_x = 0, last_y = 0;
//         if (abs(x_raw - last_x) < 10 && abs(y_raw - last_y) < 10) return;
//         last_x = x_raw;
//         last_y = y_raw;

//         coord_x = transform(x_raw, 2032, 150, 320, 0);
//         coord_y = transform(y_raw, 2032, 100, 480, 0);

//         // Стираем старую платформу
//         rect(right_platform.height, right_platform.width,right_platform.y - right_platform.height/2,right_platform.x - right_platform.width/2,BG_COLOR);

//         // Обновляем координаты
//         right_platform.x = coord_x;
//         right_platform.y = coord_y;

//         // Ограничение по экрану
//         if (right_platform.x - right_platform.width/2 < 200)  right_platform.x = 200 + right_platform.width/2;
//         if (right_platform.x + right_platform.width/2 > 320)  right_platform.x = 320 - right_platform.width/2;
//         if (right_platform.y - right_platform.height/2 < 0)   right_platform.y = right_platform.height/2;
//         if (right_platform.y + right_platform.height/2 > 480) right_platform.y = 480 - right_platform.height/2;

//         // Рисуем новую платформу
//         rect(right_platform.height, right_platform.width,right_platform.y - right_platform.height/2,right_platform.x - right_platform.width/2,PLATFORM_COLOR);
//     }
// }

Platform right_platform = {240, 240, 15, 70};

void process_platform_touch(const uint16_t PLATFORM_COLOR, const uint16_t BG_COLOR) {

    if(game_over==1||bricks_live==0){
        right_platform.x = 240;
        right_platform.y = 240;
        return; 
    }

    rect(right_platform.height, right_platform.width,right_platform.y - right_platform.height/2,right_platform.x - right_platform.width/2,PLATFORM_COLOR);

    int x_raw, y_raw;
    int coord_x, coord_y;

    x_raw = read_filtered_coordinate(READ_X, 5);
    y_raw = read_filtered_coordinate(READ_Y, 5);

    if (x_raw > 0 && y_raw < 2032) {
        coord_x = transform(x_raw, 2032, 150, 320, 0);
        coord_y = transform(y_raw, 2032, 100, 480, 0);

        // проверка, находится ли тач рядом с платформой
        if (abs(coord_x - right_platform.x) <= PLATFORM_TOUCH_RADIUS-30 &&
            abs(coord_y - right_platform.y) <= PLATFORM_TOUCH_RADIUS) {

            // фильтр мелких движений
            static int last_x = 0, last_y = 0;
            if (abs(coord_x - last_x) < 5 && abs(coord_y - last_y) < 5) return;
            last_x = coord_x;
            last_y = coord_y;

            // стираем старую платформу
            rect(right_platform.height, right_platform.width,
                 right_platform.y - right_platform.height/2,
                 right_platform.x - right_platform.width/2, BG_COLOR);

            // обновляем координаты
            right_platform.x = coord_x;
            right_platform.y = coord_y;

            // ограничение по экрану
            if (right_platform.x - right_platform.width/2 < 200)  right_platform.x = 200 + right_platform.width/2;
            if (right_platform.x + right_platform.width/2 > 320)  right_platform.x = 320 - right_platform.width/2;
            if (right_platform.y - right_platform.height/2 < 0)   right_platform.y = right_platform.height/2;
            if (right_platform.y + right_platform.height/2 > 480) right_platform.y = 480 - right_platform.height/2;

            // рисуем новую платформу
            rect(right_platform.height, right_platform.width,
                 right_platform.y - right_platform.height/2,
                 right_platform.x - right_platform.width/2, PLATFORM_COLOR);
        }
    }
}