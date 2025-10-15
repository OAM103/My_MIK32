#include "touchpad.h"
#include "lcd.h"
#include "system_config.h"
#include "mik32_hal.h"
#include "mik32_hal_spi.h"
#include "xprintf.h"

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


void rect(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w, uint16_t color) {
    Lcd_select();
    HAL_GPIO_WritePin(GPIO_0, RS, 0);
    Address_set(d_w, d_l, d_w + width - 1, d_l + length - 1);
    HAL_GPIO_WritePin(GPIO_0, RS, 1);

    uint8_t r = (color >> 8) & 0xF8;
    uint8_t g = (color >> 3) & 0xFC;
    uint8_t b = (color << 3);

    uint32_t pixels = (uint32_t)length * width;

    __HAL_SPI_ENABLE(&hspi0);

    while (pixels--) {
        while (!(hspi0.Instance->INT_STATUS & SPI_INT_STATUS_TX_FIFO_NOT_FULL_M));
        hspi0.Instance->TXDATA = r;
        hspi0.Instance->TXDATA = g;
        hspi0.Instance->TXDATA = b;
    }

    __HAL_SPI_DISABLE(&hspi0);
    Lcd_unselect();
}

void drawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    uint8_t colout[3];
    colout[2] = (color >> 8) & 0xF8; // R
    colout[0] = (color >> 3) & 0xFC; // G
    colout[1] = (color << 3);        // B

    Lcd_select();
    HAL_GPIO_WritePin(GPIO_0, RS, 0);
    Address_set(x, y, x, y);
    HAL_GPIO_WritePin(GPIO_0, RS, 1);

    // передача пикселя
    while (!(hspi0.Instance->INT_STATUS & SPI_INT_STATUS_TX_FIFO_NOT_FULL_M));
    hspi0.Instance->TXDATA = colout[2];
    hspi0.Instance->TXDATA = colout[0];
    hspi0.Instance->TXDATA = colout[1];
    Lcd_unselect();
}
void drawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t color)//пустой круг
{
    int x = radius;
    int y = 0;
    int radiusError = 1 - x;

    while (x >= y)
    {
        drawPixel(x0 + x, y0 + y, color);
        drawPixel(x0 + y, y0 + x, color);
        drawPixel(x0 - x, y0 + y, color);
        drawPixel(x0 - y, y0 + x, color);
        drawPixel(x0 - x, y0 - y, color);
        drawPixel(x0 - y, y0 - x, color);
        drawPixel(x0 + x, y0 - y, color);
        drawPixel(x0 + y, y0 - x, color);

        y++;
        if (radiusError < 0)
        {
            radiusError += 2 * y + 1;
        }
        else
        {
            x--;
            radiusError += 2 * (y - x + 1);
        }
    }
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

        if (abs(x - last_x) < 20 && abs(y - last_y) < 20) {
            return;
        }
        last_x = x;
        last_y = y;
        // "устойчивая" точка — принимаем
        coord_x = transform(x, 2032, 150, 320, 0);
        coord_y = transform(y, 2032, 100, 480, 0);
        //rect(30, 10 , coord_y, coord_x, 0x0000);
        drawCircle(coord_x, coord_y, 15, 0x0000);
    }
}