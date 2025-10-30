#include "touchpad.h"
#include "lcd.h"
#include "transform.h"
#include "system_config.h"
#include "mik32_hal.h"
#include "mik32_hal_spi.h"
#include "xprintf.h"
#include <stdbool.h>

//extern SPI_HandleTypeDef hspi0;
//extern SPI_HandleTypeDef hspi1;

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
        drawCircle(coord_x, coord_y, 15, 0x0000);
    }
}

typedef enum { NONE, LEFT, RIGHT, TOP, BOTTOM, ROTATE_MODE } Side;

void dynamic_rectangle(Rectangle *rect, uint16_t BG_COLOR, uint16_t RECT_COLOR)
{
    static Side active_side = NONE; //активная сторона:)
    static int last_x = 0, last_y = 0;//текущие координаты, как последнее состояние

    int x = read_filtered_coordinate(READ_X, 5);
    int y = read_filtered_coordinate(READ_Y, 5);

    if (x > 0 && y < 2032)
    {
        int coord_x = transform(x, 2032, 150, 320, 0);
        int coord_y = transform(y, 2032, 100, 480, 0);

        // выбор стороны при первом касании
        if (active_side == NONE)
        {
            if (abs(coord_x - rect->x) < 10 && coord_y > rect->y && coord_y < rect->y + rect->height)
                active_side = LEFT;
            else if (abs(coord_x - (rect->x + rect->width)) < 10 && coord_y > rect->y && coord_y < rect->y + rect->height)
                active_side = RIGHT;
            else if (abs(coord_y - rect->y) < 10 && coord_x > rect->x && coord_x < rect->x + rect->width)
                active_side = TOP;
            else if (abs(coord_y - (rect->y + rect->height)) < 10 && coord_x > rect->x && coord_x < rect->x + rect->width)
                active_side = BOTTOM;

            last_x = coord_x;
            last_y = coord_y;
        }

        // если тянем — изменяем размер
        if (active_side != NONE)
        {
            // стираем старый прямоугольник
            draw_rectangle(rect, BG_COLOR);

            int dx = coord_x - last_x;
            int dy = coord_y - last_y;

            switch (active_side)
            {
                case LEFT:  rect->x += dx; rect->width -= dx; 
                    if (rect->width < 10) rect->width = 10; 
                    break;
                case RIGHT: rect->width += dx; 
                    if (rect->width < 10) rect->width = 10; 
                    break;
                case TOP:   rect->y += dy; rect->height -= dy; 
                    if (rect->height < 10) rect->height = 10; 
                    break;
                case BOTTOM:rect->height += dy; 
                    if (rect->height < 10) rect->height = 10; 
                    break;
                default: break;
            }

            last_x = coord_x;
            last_y = coord_y;

            draw_rectangle(rect, RECT_COLOR);
        }
    }
    else
    {
        active_side = NONE;//если не трогает;)
    }
}


void rotate_rectangle(Rectangle *rect, uint16_t BG_COLOR, uint16_t RECT_COLOR)
{
    static int last_x = 0;
    static float angle_deg = 0;
    static int initialized = 0;

    // Чтение координат тачпада
    int x = read_filtered_coordinate(READ_X, 5);
    int y = read_filtered_coordinate(READ_Y, 5);

    if (x > 0 && y < 2032)
    {
        int coord_x = transform(y, 2032, 100, 480, 0);
        //int coord_y = transform(y, 2032, 100, 480, 0);

        // Инициализация
        if (!initialized)
        {
            last_x = coord_x;
            initialized = 1;
        }
        draw_rotated_rect(rect, BG_COLOR);// Стираем старый прямоугольник
        int dx = coord_x - last_x; // Вычисляем изменение положения по X
        angle_deg += dx * 0.5f; //для плавности
        // Ограничим диапазон угла
        if (angle_deg > 180) angle_deg -= 360;
        if (angle_deg < -180) angle_deg += 360;

        rect->angle = angle_deg; // Обновляем угол и рисуем новый
        draw_rotated_rect(rect, RECT_COLOR);

        last_x = coord_x; // Обновляем последнюю координату
    }
    else
    {
        initialized = 0;
    }
}

// TouchState touch = {0, 0};
// float angle_x = 0.0f;
// float angle_y = 0.0f;
// int active = 0; 
// uint8_t cube_update = 0; // флаг нужно ли перерисовать куб

// void rotate_cube_touch()
// {
//     int x = read_filtered_coordinate(READ_X, 5);
//     int y = read_filtered_coordinate(READ_Y, 5);

//     static int last_coord_x = 0, last_coord_y = 0;

//     if (x > 0 && y < 2032)
//     {
//         int coord_y = transform(x, 2032, 150, 320, 0);
//         int coord_x = transform(y, 2032, 100, 480, 0);

//         if (!active) {
//             touch.last_x = coord_x;
//             touch.last_y = coord_y;
//             last_coord_x = coord_x;
//             last_coord_y = coord_y;
//             active = 1;
//             cube_update = 0;
//             return;
//         } 
//         // если палец стоит на месте — не вращаем
//         if (abs(coord_x - last_coord_x) < 3 && abs(coord_y - last_coord_y) < 3) {
//             cube_update = 0; // не нужно обновление
//             return;
//         }
//         // вычисляем изменение
//         int dx = coord_x - touch.last_x;
//         int dy = coord_y - touch.last_y;

//         // вращаем (чем меньше множители — тем плавнее)
//         angle_y += dx * 0.005f;
//         angle_x += dy * 0.005f;

//         touch.last_x = coord_x;
//         touch.last_y = coord_y;
//         last_coord_x = coord_x;
//         last_coord_y = coord_y;

//         cube_update = 1; // куб нужно обновить
//     }
//     else {
//         active = 0;
//         cube_update = 0;
//     }
// }
