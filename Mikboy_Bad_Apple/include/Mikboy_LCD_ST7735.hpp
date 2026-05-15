#ifndef MIKBOY_LCD_ST7735
#define MIKBOY_LCD_ST7735

#include "Mikboy_LCD_periphery.hpp"
#include <cstdint>

namespace ST7735
{

    constexpr uint8_t CHAR_OFFSET = 0x20;
    constexpr uint16_t SIZE_BUF = 260;

    enum Commands
    {
        NOP = 0x00,       // No Operation
        SWRESET = 0x01,   // Software Reset
        RDDID = 0x04,     // Read Display ID
        RDDST = 0x09,     // Read Display Status
        RDDPM = 0x0A,     // Read Display Power Mode
        RDDMADCTL = 0x0B, // Read Display MADCTL
        RDDCOLMOD = 0x0C, // Read Display Pixel Format
        RDDIM = 0x0D,     // Read Display Image Mode
        RDDSM = 0x0E,     // Read Display Signal Mode
        RDDSDR = 0x0F,    // Read Display Self-diagnostic
        SLEEPIN = 0x10,   // Sleep In & Booster Off
        SLEEPOUT = 0x11,  // Sleep Out & Booster On
        PTLON = 0x12,     // Partial Mode On
        NORON = 0x13,     // Partial Off (Normal)
        INVOFF = 0x20,    // Display Inversion Off
        INVON = 0x21,     // Display Inversion On
        GAMSET = 0x26,    // Gamma Curve Select
        DISPOFF = 0x28,   // Display Off
        DISPON = 0x29,    // Display On
        CASET = 0x2A,     // Column Address Set
        RASET = 0x2B,     // Row Address Set
        RAMWR = 0x2C,     // Memory Write
        RGBSET = 0x2D,    // LUT for 4k,65k,262k Color
        RAMRD = 0x2E,     // Memory Read
        PTLAR = 0x30,     // Partial Start/End Address
        SCRLAR = 0x33,    // Scroll area set
        TEOFF = 0x34,     // Tearing effect line off
        TEON = 0x35,      // Tearing Effect Mode Set
        MADCTL = 0x36,    // Memory Data Access Control
        VSCSAD = 0x37,    // Scroll Start Address
        IDMOFF = 0x38,    // Idle Mode Off
        IDMON = 0x39,     // Idle Mode On
        COLMOD = 0x3A,    // Interface Pixel Format
        RDID1 = 0xDA,     // Read ID1
        RDID2 = 0xDB,     // Read ID2
        RDID3 = 0xDC,     // Read ID3
        PWCTR1 = 0xC0,    // Power Control Setting
        PWCTR2 = 0xC1,
        PWCTR3 = 0xC2,
        PWCTR4 = 0xC3,
        PWCTR5 = 0xC4,
        VMCTR1 = 0xC5,  // VCOM Control 1
        VMOFCTR = 0xC7, // Set VCOM Offset control
        WRID2 = 0xD1,   // Set LCM Version Code
        WRID3 = 0xD2,   // Customer Project Code
        NVCTR1 = 0xD9,
        WRID4 = 0xDE,
        WRID5 = 0xDF,
        GMCTRP1 = 0xE0, // Positive Gamma Correction
        GMCTRN1 = 0xE1  // Negative Gamma Correction
    };

    typedef enum
    {
        Color_12_bit = 0x03,
        Color_16_bit = 0x05,
        Color_18_bit = 0x06
    } PixelFormat;

    typedef enum
    {
        row_order = (1 << 7),           // Отражает координаты по строкам.
        colomn_order = (1 << 6),        // Отражает координаты по столбцам.
        row_colomn_exchenge = (1 << 5), // Меняет местами строки/столбцы.
        top_bottom_refresh = (0 << 4),  // Обновляет дисплей сверху вниз.
        bottom_top_refrash = (1 << 4),  // Обновляет дисплей снизу вверх.
        RGB = (0 << 3),                 // Устанавливает цвет RGB.
        BGR = (1 << 3),                 // Устанавливает цвет BGR.
        left_right_refresh = (0 << 2),  // Обновляет дисплей слева направо.
        right_left_refresh = (1 << 2)   // Обновляет дисплей справа налево.
    } AccessControl;

    class LCD : public Periphery::LCD
    {
    private:
        uint8_t min_width_ = 0;  // ширина дисплея
        uint8_t min_height_ = 0; // высота дисплея
        uint8_t max_width_;
        uint8_t max_height_;

        int8_t rst_ = -1; // опциональный
        int8_t led_ = -1; // опциональный

        // шрифт
        uint8_t font_height = 1; // множитель высоты
        uint8_t font_width = 1;  // множитель ширины
        uint8_t font_kerning = 1;

        static int32_t abs(int32_t a);
        static void Swap(uint16_t &a, uint16_t &b);
        void check_border_column(int16_t &x) const;
        void check_border_row(int16_t &y) const;

        
        HAL_StatusTypeDef scroll_area() const;
        HAL_StatusTypeDef scroll_start(uint8_t step) const;
        
        protected:
        public:
        LCD() = default;
        LCD(SPI_TypeDef *spi, DMA_ChannelHandleTypeDef &dma_ch);
        HAL_StatusTypeDef init(uint16_t width, uint16_t height, int8_t dc, int8_t cs = -1, int8_t rst = -1, int8_t led = -1);
        HAL_StatusTypeDef write_char(uint16_t x_start, uint16_t y_start, const uint8_t word, uint16_t *mas) const;
        
        /// @brief Перезагружает дисплей
        void reset() const;

        /// @brief Выводит дисплей в состояние сна
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef sleep_out() const;

        /// @brief Вводит дисплей из состояние сна
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef sleep_in() const;

        /// @brief Вводит дисплей в режим частичного отображения
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef partial_mode() const;

        /// @brief Выводит дисплей из режима частичного отображения
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef normal_mode() const;

        /// @brief Инвертирует изображение на дисплее
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef inversion_on() const;

        /// @brief Отключает инверсию
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef inversion_off() const;

        /// @brief Устанавливает гамму дисплея
        /// @param gamma значение гаммы
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef gamma_set(uint8_t gamma) const;

        /// @brief Включает дисплей
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef display_on() const;

        /// @brief Выключает дисплей
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef display_off() const;

        /// @brief Устанавливает начало и конец вывода по колонкам
        /// @param start колонка начала записи
        /// @param end колонка конца записи
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef addres_column(int16_t start, int16_t end) const;

        /// @brief Устанавливает начало и конец вывода по строкам
        /// @param start строка начала записи
        /// @param end строка конца записи
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef addres_row(int16_t start, int16_t end) const;

        /// @brief Устанавливает количество бит на пиксель
        /// @param mode Color_12_bit, Color_16_bit, Color_18_bit
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef pixel_format(PixelFormat mode) const;

        /// @brief Настройки доступа дисплея к памяти
        /// @param сontrol перечисление из AccessControl
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef access_control(uint8_t control) const;

        /// @brief Установка границ записи данных на дисплей
        /// @param x0 начало по столбцам
        /// @param y0 начало по строкам
        /// @param x1 конец по столбцам
        /// @param y1 конец по строкам
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef set_border(int16_t x0, int16_t y0, int16_t x1, int16_t y1) const;

        /// @brief Включает подсветку дисплея
        void led_on() const;

        /// @brief Выключает подсветку дисплея
        void led_off() const;

        /// @brief Функция настроек шрифта
        /// @param height множитель высоты шрифта от 1 до 4
        /// @param width множитель ширины шрифта от 1 до 4
        /// @param kerning межбуквенный интервал
        void set_font_multiplier(int8_t height, int8_t width, int8_t kerning = -1);

        /// @brief Закрашивает пиксель по координатам
        /// @param x колонка
        /// @param y строка
        /// @param color цвет
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef draw_pixel(uint16_t x, uint16_t y, uint32_t color) const;

        /// @brief Рисует круг на дисплее
        /// @param x центр круга по x
        /// @param y центр круга по y
        /// @param radius радиус
        /// @param color цвет
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef draw_circle(uint16_t x, uint16_t y, uint16_t radius, uint32_t color) const;

        /// @brief Рисует линию на дисплее
        /// @param x_start начало линии по x
        /// @param y_start начало линии по y
        /// @param x_end конец линии по x
        /// @param y_end конец линии по y
        /// @param color цвет
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef draw_line(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint32_t color) const;

        /// @brief Рисует квадрат
        /// @param x_start верхний левый угол по x
        /// @param y_start верхний левый угол по y
        /// @param x_end нижний правый угол по x
        /// @param y_end нижний правый угол по y
        /// @param color цвет
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef draw_rect(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint32_t color) const;

        /// @brief Рисует закрашеный квадрат
        /// @param x_start верхний левый угол по x
        /// @param y_start верхний левый угол по y
        /// @param x_end нижний правый угол по x
        /// @param y_end нижний правый угол по y
        /// @param color цвет
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef draw_fill_rect(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint32_t color) const;

        /// @brief Рисует закрашеный круг на дисплее
        /// @param x центр круга по x
        /// @param y центр круга по y
        /// @param radius радиус
        /// @param color цвет
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef draw_fill_circle(uint16_t x, uint16_t y, uint16_t radius, uint32_t color) const;

        /// @brief Выводит строку на дисплей
        /// @param x_start верхний левый угол по x
        /// @param y_start верхний левый угол по y
        /// @param word указатель на строку
        /// @param color цвет шрифта
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef print_word(uint16_t x_start, uint16_t y_start, const char *word, uint32_t color) const;
    };
}
#endif