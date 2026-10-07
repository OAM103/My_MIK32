/**
 * @file    lcd.c
 * @brief   Реализация драйвера TFT-дисплея на базе ILI9341 с сенсорной панелью:
 *          инициализация и базовая отрисовка.
 */

/* Includes ------------------------------------------------------------------*/
#include "lcd.h"
#include "bitmaps.h"

/* Private variables ---------------------------------------------------------*/
extern SPI_HandleTypeDef hspi0;

/* Private function prototypes -----------------------------------------------*/
static void Lcd_Reset(void);
static void Lcd_select(void);
static void Lcd_unselect(void);
static void Lcd_Writ_Bus(uint8_t d);
static void Lcd_Write_Command(unsigned char command);
static void Lcd_Write_Data(uint8_t data);
static void Address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
static void lcd_dma_end(void);
static void lcd_dma_send_row(uint16_t width);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Активация дисплея (CS)
 */
static void Lcd_select(void)
{
    HAL_GPIO_WritePin(GPIO_0, CS, GPIO_PIN_LOW);
}

/**
 * @brief  Деактивация дисплея (CS)
 */
static void Lcd_unselect(void)
{
    HAL_GPIO_WritePin(GPIO_0, CS, GPIO_PIN_HIGH);
}

/**
 * @brief  Отправка 1 байта данных или команды
 */
static void Lcd_Writ_Bus(uint8_t d)
{
    uint8_t out[] = {d};
    uint8_t in[sizeof(out)];

    HAL_SPI_Exchange(&hspi0, out, in, sizeof(out), SPI_TIMEOUT_DEFAULT);
}

/**
 * @brief  Отправка команды (RS = 0)
 */
static void Lcd_Write_Command(unsigned char command)
{
    HAL_GPIO_WritePin(GPIO_0, RS, 0);
    Lcd_Writ_Bus(command);
}

/**
 * @brief  Отправка данных (RS = 1)
 */
static void Lcd_Write_Data(uint8_t data)
{
    HAL_GPIO_WritePin(GPIO_0, RS, 1);
    Lcd_Writ_Bus(data);
}

/**
 * @brief  Установка области памяти LCD для записи пикселей
 */
static void Address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    if (x1 > x2) {
        uint16_t tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y1 > y2) {
        uint16_t tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    Lcd_Write_Command(0x2a);
    Lcd_Write_Data(x1 >> 8);
    Lcd_Write_Data(x1 & 0xFF);
    Lcd_Write_Data(x2 >> 8);
    Lcd_Write_Data(x2 & 0xFF);

    Lcd_Write_Command(0x2b);
    Lcd_Write_Data(y1 >> 8);
    Lcd_Write_Data(y1 & 0xFF);
    Lcd_Write_Data(y2 >> 8);
    Lcd_Write_Data(y2 & 0xFF);

    Lcd_Write_Command(0x2c);
}
/**
 * @brief  Аппаратный сброс дисплея
 */
static void Lcd_Reset(void)
{
    HAL_GPIO_WritePin(GPIO_0, RESET, 1);
    HAL_DelayMs(2);
    HAL_GPIO_WritePin(GPIO_0, RESET, 0);
    HAL_DelayMs(5);
    HAL_GPIO_WritePin(GPIO_0, RESET, 1);
    HAL_DelayMs(2);
}

/**
 * @brief       Отправка одной строки пикселей из DMA_BUF в дисплей
 * @param width Число пикселей в строке
 */
static void lcd_dma_send_row(uint16_t width)
{
    HAL_DMA_Start(&hdma_ch0, (void *)DMA_BUF, (void *)&hspi0.Instance->TXDATA, width * 3 - 1);
    HAL_DMA_Wait(&hdma_ch0, DMA_TIMEOUT_DEFAULT);
}

/**
 * @brief  Завершение SPI-сессии после DMA-отрисовки
 */
static void lcd_dma_end(void)
{
    if (!(hspi0.Instance->CONFIG & SPI_CONFIG_MANUAL_CS_M)) {
        __HAL_SPI_DISABLE(&hspi0);
        hspi0.Instance->ENABLE |= SPI_ENABLE_CLEAR_TX_FIFO_M | SPI_ENABLE_CLEAR_RX_FIFO_M;
    }
    volatile uint32_t unused = hspi0.Instance->INT_STATUS;
    (void)unused;
    Lcd_unselect();
}

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Инициализация дисплея
 */
void Lcd_Init(void)
{
    Lcd_Reset();
    Lcd_select();

    Lcd_Write_Command(0xF7);
    Lcd_Write_Data(0xA9);
    Lcd_Write_Data(0x51);
    Lcd_Write_Data(0x2C);
    Lcd_Write_Data(0x82);

    Lcd_Write_Command(0xC0);
    Lcd_Write_Data(0x11);
    Lcd_Write_Data(0x09);

    Lcd_Write_Command(0xC1);
    Lcd_Write_Data(0x41);

    Lcd_Write_Command(0xC5);
    Lcd_Write_Data(0x00);
    Lcd_Write_Data(0x0A);
    Lcd_Write_Data(0x80);

    Lcd_Write_Command(0xB1);
    Lcd_Write_Data(0xB0);
    Lcd_Write_Data(0x11);

    Lcd_Write_Command(0xB4);
    Lcd_Write_Data(0x02);

    Lcd_Write_Command(0xB6);
    Lcd_Write_Data(0x02);
    Lcd_Write_Data(0x22);

    Lcd_Write_Command(0xB7);
    Lcd_Write_Data(0xC6);

    Lcd_Write_Command(0xBE);
    Lcd_Write_Data(0x00);
    Lcd_Write_Data(0x04);

    Lcd_Write_Command(0xE9);
    Lcd_Write_Data(0x00);

    Lcd_Write_Command(0x36);
    Lcd_Write_Data(0x08);

    Lcd_Write_Command(0x3A);
    Lcd_Write_Data(0x66);

    Lcd_Write_Command(0xE0);
    Lcd_Write_Data(0x00);
    Lcd_Write_Data(0x07);
    Lcd_Write_Data(0x10);
    Lcd_Write_Data(0x09);
    Lcd_Write_Data(0x17);
    Lcd_Write_Data(0x0B);
    Lcd_Write_Data(0x41);
    Lcd_Write_Data(0x89);
    Lcd_Write_Data(0x4B);
    Lcd_Write_Data(0x0A);
    Lcd_Write_Data(0x0C);
    Lcd_Write_Data(0x0E);
    Lcd_Write_Data(0x18);
    Lcd_Write_Data(0x1B);
    Lcd_Write_Data(0x0F);

    Lcd_Write_Command(0xE1);
    Lcd_Write_Data(0x00);
    Lcd_Write_Data(0x17);
    Lcd_Write_Data(0x1A);
    Lcd_Write_Data(0x04);
    Lcd_Write_Data(0x0E);
    Lcd_Write_Data(0x06);
    Lcd_Write_Data(0x2F);
    Lcd_Write_Data(0x45);
    Lcd_Write_Data(0x43);
    Lcd_Write_Data(0x02);
    Lcd_Write_Data(0x0A);
    Lcd_Write_Data(0x09);
    Lcd_Write_Data(0x32);
    Lcd_Write_Data(0x36);
    Lcd_Write_Data(0x0F);

    Lcd_Write_Command(0x11);
    HAL_DelayMs(120);

    Lcd_Write_Command(0x29);
    Lcd_unselect();
}


/**
 * @brief          Отрисовка залитого прямоугольника
 * @param  length  Высота в пикселях
 * @param  width   Ширина в пикселях
 * @param  y       Координата d_l
 * @param  x       Координата d_w
 * @param  color   Цвет заливки (RGB565)
 */
void draw_rect(uint16_t length, uint16_t width, uint16_t y, uint16_t x, uint16_t color)
{
    uint8_t r = (color >> 8) & 0xF8;
    uint8_t g = (color >> 3) & 0xFC;
    uint8_t b = (color << 3);
    uint32_t pixels = (uint32_t)length * width;

    Lcd_select();
    HAL_GPIO_WritePin(GPIO_0, RS, 0);
    Address_set(x, y, x + width - 1, y + length - 1);
    HAL_GPIO_WritePin(GPIO_0, RS, 1);

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

/**
 * @brief  Заполняет прямоугольную область дисплея одним цветом
 *
 * Цвет RGB565 разбивается на три компонента и записывается в DMA-буфер в том же порядке, в котором цвет передаётся функцией draw_rect(): red -> green -> blue
 *
 * @param length  Высота области в пикселях
 * @param width   Ширина области в пикселях
 * @param d_l     Начальная координата Y
 * @param d_w     Начальная координата X
 * @param color   Цвет области в формате RGB565
 */
void ClearMassDMA(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w, uint16_t color)
{
    uint8_t red   = (color >> 8) & 0xF8;
    uint8_t green = (color >> 3) & 0xFC;
    uint8_t blue  = (color << 3);

    Lcd_select();

    length++;
    width++;

    Address_set(d_w, d_l, width - 2 + d_w, length - 2 + d_l );

    HAL_GPIO_WritePin(GPIO_0, RS, 1);

    hspi0.ErrorCode = HAL_SPI_ERROR_NONE;
    hspi0.Instance->TX_THR = 1;

    if (!(hspi0.Instance->ENABLE & SPI_ENABLE_M)) __HAL_SPI_ENABLE(&hspi0);

    for (uint16_t m = 0; m < length; m++) {
        for (uint16_t i = 0; i < width; i++) {

            DMA_BUF[i * 3 + 0] = red;
            DMA_BUF[i * 3 + 1] = green;
            DMA_BUF[i * 3 + 2] = blue;
        }
        lcd_dma_send_row(width);
    }
    lcd_dma_end();
}

/**
 * @brief  Инверсия порядка битов в одном байте
 * Выполняет полное зеркальное отражение 8 бит: старший бит становится младшим, а младший - старшим
 * @param b  Исходный байт
 * @return  Байт с инвертированным порядком битов.
 */
uint8_t reverse_bits(uint8_t b)
{
    b = ((b & 0xF0) >> 4) | ((b & 0x0F) << 4);
    b = ((b & 0xCC) >> 2) | ((b & 0x33) << 2);
    b = ((b & 0xAA) >> 1) | ((b & 0x55) << 1);
    return b;
}

/**
 * @brief  Отрисовка одного символа зеркально по горизонтали
 *
 * Символ зеркально отражается внутри своего изображения:
 * левая часть символа становится правой, а правая - левой
 *
 * Порядок координат при отрисовке также изменён так,чтобы результат соответствовал горизонтальному зеркальному отражению
 *
 * @param x      Координата X символа
 * @param y      Координата Y символа
 * @param ch     Выводимый символ
 * @param color  Цвет символа
 * @param bg     Цвет фона
 * @param scale  Масштаб символа
 */
void drawChar(uint16_t x, uint16_t y, char ch, uint16_t color, uint16_t bg, uint8_t scale)
{
    const uint8_t *bitmap;
    if (ch < 32 || ch > 126) return;
    bitmap = Char_font1[ch - 32];

    /*
     * Символ содержит 5 столбцов и 7 строк.
     * Столбцы выводятся в обратном порядке, поэтому сам символ получается зеркальным по горизонтали
     */
    for (uint8_t col = 0; col < 5; col++) {

        uint8_t line;
        uint8_t source_col;

        source_col = 4 - col;
        line = reverse_bits(bitmap[source_col]);

        // Биты строки также разворачиваются, чтобы содержимое символа было зеркальным
        for (uint8_t row = 0; row < 7; row++) {

            uint8_t pixel_on;
            uint16_t draw_color;

            pixel_on = (line & (1 << row)) ? 1 : 0;

            draw_color = pixel_on ? color : bg;

            ClearMassDMA( scale + 1, scale + 1, y + col * scale, x - row * scale, draw_color);
        }
    }
}

/**
 * @brief  Отрисовка строки в полностью зеркальном виде
 *
 * Строка отражается по горизонтали целиком: порядок символов разворачивается; каждый отдельный символ также зеркально отражается по горизонтали
 *
 * @param x      Координата X первого выводимого символа
 * @param y      Координата Y строки
 * @param text   Указатель на строку
 * @param color  Цвет текста
 * @param bg     Цвет фона
 * @param scale  Масштаб символов
 */
void drawText(uint16_t x, uint16_t y, const char *text, uint16_t color, uint16_t bg, uint8_t scale)
{
    const char *end;

    end = text;

    // Находим конец строки
    while (*end) end++;

    /*
     * Выводим символы с конца строки к началу
     * Координата Y увеличивается для каждого следующего символа, поэтому визуальный порядок строки также становится обратным
     */
    while (end != text) {
        end--;
        drawChar( x, y, *end, color, bg, scale);

        y += 6 * scale;
    }
}

