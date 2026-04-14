#include "touchpad.h"
#include "lcd.h"
#include "system_config.h"
#include "mik32_hal.h"
#include "mik32_hal_spi.h"
#include "xprintf.h"
#include "tracks.h"

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

uint8_t touch_active = 0;
uint8_t play_sound = 0;

void process_touchpad()
{
    int x = read_filtered_coordinate(READ_X, 5);
    int y = read_filtered_coordinate(READ_Y, 5);

    //  нет касания -> выключаем звук
    if (x <= 0 || y >= 2032)
    {
        touch_active = 0;
        return;
    }
    // удержание
    if (touch_active) return;
    //  новое касание
    touch_active = 1;
    play_sound = 1;
    music_pos = 0;

    if (x < 150) x = 150;
    if (y < 100) y = 100;

    int coord_x = transform(x, 2032, 150, 320, 0);
    int coord_y = transform(y, 2032, 100, 480, 0);

    // rect(10, 10, -coord_y, coord_x, 0xFFFF);
    gradientCircle(coord_x, 480-coord_y, 1, 20, 0x00F, 0x0000);
    drawCircle(coord_x, 480-coord_y, 5, 0x001F);
}

MovingCircle circles[MAX_CIRCLES];

void process_touchpad_active()
{
    int x = read_filtered_coordinate(READ_X, 5);
    int y = read_filtered_coordinate(READ_Y, 5);

    if (x <= 0 || y >= 2032) //  нет касания -> выключаем звук
    {
        touch_active = 0;
        return;
    }

    if (touch_active) return;

    touch_active = 1;
    play_sound = 1;
    music_pos = 0;

    if (x < 150) x = 150;
    if (y < 100) y = 100;

    int coord_x = transform(x, 2032, 150, 320, 0);
    int coord_y = transform(y, 2032, 100, 480, 0);

    // ищем свободный слот
    for (int i = 0; i < MAX_CIRCLES; i++)
    {
        if (!circles[i].active)
        {
            circles[i].x = coord_x;
            circles[i].y = 480 - coord_y;
            circles[i].active = 1;
            break;
        }
    }
}

void update_circles() // анимация двидения кругов
{
    for (int i = 0; i < MAX_CIRCLES; i++)
    {
        if (!circles[i].active) continue;

        // рисуем
        gradientCircle(circles[i].x, circles[i].y, 1, 20, 0x00F, 0x0000);
        drawCircle(circles[i].x, circles[i].y, 5, 0x001F);

        // движение вниз
        circles[i].y += 2;

        // если ушёл за экран - удаляем
        if (circles[i].y > 480+20)
        {
            circles[i].active = 0;
        }
    }
}