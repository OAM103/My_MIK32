#include "Mikboy_LCD_ST7735.hpp"
#include <cstdint>
#include <mik32_hal_def.h>
#include <xprintf.h>
#include "Mikboy_LCD_font.h"
#include "png.hpp"

namespace ST7735
{

    int32_t LCD::abs(int32_t a)
    {
        return (a > 0) ? a : -a;
    }
    void LCD::Swap(uint16_t &a, uint16_t &b)
    {
        int32_t t = a;
        a = b;
        b = t;
    }

    LCD::LCD(SPI_TypeDef *spi, DMA_ChannelHandleTypeDef &dma_ch) : Periphery::LCD(spi, dma_ch) {}

    HAL_StatusTypeDef LCD::init(uint16_t width, uint16_t height, int8_t dc, int8_t cs, int8_t rst, int8_t led)
    {

        HAL_StatusTypeDef error = HAL_OK;
        __HAL_PCC_GPIO_0_CLK_ENABLE();
        __HAL_PCC_GPIO_1_CLK_ENABLE();
        __HAL_PCC_GPIO_2_CLK_ENABLE();

        max_width_ = width;
        max_height_ = height;
        min_height_ = 0;
        min_width_ = 0;
        rst_ = rst;
        led_ = led;
        if (rst_ != -1)
            ll_gpio_set_mode(led_, PIN_AS_GPIO);
        ll_gpio_set_output(rst_);
        if (led_ != -1)
        {
            ll_gpio_set_mode(led_, PIN_AS_GPIO);
            ll_gpio_out_write(led_,1);
            
        }
        error = Periphery::LCD::init(dc, cs);

        if (error != HAL_OK)
            return error;
        reset();
        error = sleep_out();
        if (error != HAL_OK)
            return error;
        error = pixel_format(Color_16_bit);
        if (error != HAL_OK)
            return error;
        error = access_control(colomn_order | RGB | row_colomn_exchenge);
        if (error != HAL_OK)
            return error;
        error = normal_mode();
        if (error != HAL_OK)
            return error;
        error = inversion_off();
        if (error != HAL_OK)
            return error;
        error = gamma_set(0x01);
        if (error != HAL_OK)
            return error;
        error = display_on();

        return error;
    }

    HAL_StatusTypeDef LCD::sleep_out() const
    {
        HAL_StatusTypeDef error = HAL_OK;
        error = command(SLEEPOUT);
        if (error != HAL_OK)
            return error;
        HAL_DelayMs(120);
        return error;
    }

    HAL_StatusTypeDef LCD::sleep_in() const
    {
        return command(SLEEPIN);
    }

    HAL_StatusTypeDef LCD::partial_mode() const
    {
        return command(PTLON);
    }
    HAL_StatusTypeDef LCD::normal_mode() const
    {
        return command(NORON);
    }

    HAL_StatusTypeDef LCD::inversion_on()  const
    {
        return command(INVON);
    }

    HAL_StatusTypeDef LCD::inversion_off() const
    {
        return command(INVOFF);
    }

    HAL_StatusTypeDef LCD::gamma_set(uint8_t gamma) const
    {
        HAL_StatusTypeDef error = HAL_OK;
        error = command(GAMSET);
        if (error != HAL_OK)
            return error;
        error = data(gamma);
        return error;
    }

    HAL_StatusTypeDef LCD::display_on() const
    {
        HAL_StatusTypeDef error = HAL_OK;
        error = command(DISPON);
        if (error != HAL_OK)
            return error;
        HAL_DelayMs(100);
        return error;
    }
    HAL_StatusTypeDef LCD::display_off() const
    {
        return command(DISPOFF);
    }

    void LCD::check_border_column(int16_t &x) const
    {
        if (x > max_width_ - 1)
        {
            x = max_width_ - 1;
            return;
        }
        if (x < min_width_)
        {
            x = min_width_;
        }
    }
    void LCD::check_border_row(int16_t &y) const
    {
        if (y > max_height_ - 1)
        {
            y = max_height_ - 1;
            return;
        }
        if (y < min_height_)
        {
            y = min_height_;
        }
    }
    HAL_StatusTypeDef LCD::addres_column(int16_t xstart, int16_t xend) const
    {
        check_border_column(xstart);
        check_border_column(xend);
        HAL_StatusTypeDef error = HAL_OK;
        uint8_t mas[4];
        mas[0] = xstart >> 8;
        mas[1] = xstart;
        mas[2] = xend >> 8;
        mas[3] = xend;
        error = command(CASET);
        if (error != HAL_OK)
            return error;
        error = print_mas(mas, sizeof(mas));
        return error;
    }

    HAL_StatusTypeDef LCD::addres_row(int16_t ystart, int16_t yend) const
    {
        check_border_row(ystart);
        check_border_row(yend);
        HAL_StatusTypeDef error = HAL_OK;
        uint8_t mas[4];
        mas[0] = ystart >> 8;
        mas[1] = ystart;
        mas[2] = yend >> 8;
        mas[3] = yend;
        error = command(RASET);
        if (error != HAL_OK)
            return error;
        error = print_mas(mas, sizeof(mas));
        return error;
    }

    HAL_StatusTypeDef LCD::pixel_format(PixelFormat mode) const
    {
        HAL_StatusTypeDef error = HAL_OK;
        error = command(COLMOD);
        if (error != HAL_OK)
            return error;
        error = data(mode);
        return error;
    }

    HAL_StatusTypeDef LCD::access_control(uint8_t mode) const
    {
        HAL_StatusTypeDef error = HAL_OK;
        error = command(MADCTL);
        if (error != HAL_OK)
            return error;
        error = data(mode);
        return error;
    }

    // HAL_StatusTypeDef LCD::set_border(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
    // {
    //     HAL_StatusTypeDef error = HAL_OK;
    //     error = addres_column(x0, x1);
    //     if (error != HAL_OK)
    //         return error;
    //     error = addres_row(y0, y1);
    //     if (error != HAL_OK)
    //         return error;
    //     error = command(RAMWR);
    //     return error;
    // }
    HAL_StatusTypeDef LCD::set_border(int16_t x0, int16_t y0, int16_t x1, int16_t y1) const
    {
        check_border_column(x0);
        check_border_column(x1);
        command(CASET);
        uint8_t mas[4];
        mas[0] = 0;
        mas[1] = x0;
        mas[2] = 0;
        mas[3] = x1;
        print_mas(mas, sizeof(mas));
        check_border_row(y0);
        check_border_row(y1);
        uint8_t mas1[4];
        mas1[0] = 0;
        mas1[1] = y0;
        mas1[2] = 0;
        mas1[3] = y1;
        command(RASET);
        print_mas(mas1, sizeof(mas1));
        dma_wait();
        command(RAMWR);
        return HAL_OK;
    }

    HAL_StatusTypeDef LCD::print_mas_bg_gradient(uint16_t x, uint16_t y, const uint16_t* src, uint16_t w, uint16_t h, uint16_t from_color, uint16_t to_color, uint16_t grad_top, uint16_t grad_bottom) const
    {
        dma_wait(); // ждём завершения предыдущей DMA передачи
        set_border(x, y, x + w - 1, y + h - 1); //создаём окно рисования
        // распаковочка RGB565
        const int r1 = (grad_top >> 11) & 0x1F; // берём верхние 5 бит
        const int g1 = (grad_top >> 5)  & 0x3F; // берём верхние 6 бит
        const int b1 = grad_top & 0x1F;         // берём верхние 5 бит
        // то же самое для второго цвета
        const int r2 = (grad_bottom >> 11) & 0x1F;
        const int g2 = (grad_bottom >> 5)  & 0x3F;
        const int b2 = grad_bottom & 0x1F;

        static uint16_t sprite_buf[16 * 12]; // буфер всей картинки

        for (uint16_t py = 0; py < h; py++) // рисуем картинку построчно
        {
            uint16_t screen_y = y + py;     // абсолютный Y строки на экране

            // вычисляем цвет строки (линейный градиент)
            int r = r1 + ((r2 - r1) * screen_y) / 128;
            int g = g1 + ((g2 - g1) * screen_y) / 128;
            int b = b1 + ((b2 - b1) * screen_y) / 128;

            uint16_t bg_color = (r << 11) | (g << 5) | b; // обратно упаковываем в RGB565

            for (uint16_t px = 0; px < w; px++) // проходим по всем пикселям строки
            {
                uint32_t i = py * w + px;
                uint16_t pixel = src[i];

                // замена цветов
                if (pixel == from_color) pixel = to_color;
                else if (pixel == BG) pixel = bg_color;

                sprite_buf[i] = (pixel << 8) | (pixel >> 8); // меняем биты местами для SPI
            }
        }

        write_spi_dma((uint8_t*)sprite_buf, w * h * 2 - 1); // одна DMA передача на строку (w * 2 -> 1 пиксель = 2 байта)
        dma_wait(); // ждём завершения строки, только потом рисуем следующую
        return HAL_OK;
    }
   
    HAL_StatusTypeDef LCD::print_mas_gradient(uint16_t x, uint16_t y, const uint16_t* src, uint16_t w, uint16_t h, uint16_t from_color, uint16_t to_color, uint16_t grad_top, uint16_t grad_bottom) const
    {
        dma_wait();

        set_border(x, y, x + w - 1, y + h - 1);

        const int r1 = (grad_top >> 11) & 0x1F;
        const int g1 = (grad_top >> 5)  & 0x3F;
        const int b1 = grad_top & 0x1F;

        const int r2 = (grad_bottom >> 11) & 0x1F;
        const int g2 = (grad_bottom >> 5)  & 0x3F;
        const int b2 = grad_bottom & 0x1F;

        uint16_t line_buf[120];

        for (uint16_t py = 0; py < h; py++)
        {
            uint16_t screen_y = y + py;

            int r = r1 + ((r2 - r1) * screen_y) / 128;
            int g = g1 + ((g2 - g1) * screen_y) / 128;
            int b = b1 + ((b2 - b1) * screen_y) / 128;

            uint16_t bg_color =
                (r << 11) |
                (g << 5)  |
                b;

            for (uint16_t px = 0; px < w; px++)
            {
                uint16_t pixel = src[py * w + px];

                if (pixel == from_color)
                    pixel = to_color;
                else if (pixel == BG)
                    pixel = bg_color;

                line_buf[px] =
                    (pixel << 8) |
                    (pixel >> 8);
            }

            write_spi_dma((uint8_t*)line_buf, w * 2 - 1);
            dma_wait();
        }

        return HAL_OK;
    }
    HAL_StatusTypeDef LCD::gradient(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color_top, uint16_t color_bottom) const //вертикальный градиент
    {
        dma_wait();

        set_border(x, y, x + w - 1, y + h - 1);

        static uint16_t line_buf[160];

        int r1 = (color_top >> 11) & 0x1F;
        int g1 = (color_top >> 5)  & 0x3F;
        int b1 = color_top & 0x1F;

        int r2 = (color_bottom >> 11) & 0x1F;
        int g2 = (color_bottom >> 5)  & 0x3F;
        int b2 = color_bottom & 0x1F;

        for (uint16_t iy = 0; iy < h; iy++)
        {
            int r = r1 + ((r2 - r1) * iy) / (h - 1);
            int g = g1 + ((g2 - g1) * iy) / (h - 1);
            int b = b1 + ((b2 - b1) * iy) / (h - 1);

            uint16_t color = (r << 11) | (g << 5) | b;

            for (uint16_t ix = 0; ix < w; ix++)
            {
                line_buf[ix] = __builtin_bswap16(color);
            }

            write_spi_dma((uint8_t*)line_buf, w * 2 - 1);
        }

        return HAL_OK;
    }

    void LCD::reset() const
    {
        if (rst_ == -1)
        {
            command(SWRESET);
        }
        else
        {
            ll_gpio_write(rst_, 0);
            HAL_DelayMs(10);
            ll_gpio_write(rst_, 1);
        }
        HAL_DelayMs(120);
    }
    HAL_StatusTypeDef LCD::draw_line(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint32_t color) const
    {
        if (x_start == x_end && y_start == y_end)
        {
            return draw_pixel(x_start, y_start, color);
        }

        uint8_t steep = (abs((int16_t)y_end - (int16_t)y_start) > abs((int16_t)x_end - (int16_t)x_start));

        if (steep)
        {
            Swap(y_start, x_start);
            Swap(y_end, x_end);
        }

        if (x_end < x_start)
        {
            Swap(x_end, x_start);
            Swap(y_end, y_start);
        }

        int16_t dx = x_end - x_start;
        int16_t dy = abs((int16_t)y_end - (int16_t)y_start);
        int16_t error = dx / 2;
        int16_t ystep = (y_start < y_end) ? 1 : -1;
        int16_t y = y_start;

        for (int16_t x = x_start; x <= x_end; x++)
        {
            HAL_StatusTypeDef status = draw_pixel(steep ? y : x, steep ? x : y, color);
            if (status != HAL_OK)
                return status;

            error -= dy;
            if (error < 0)
            {
                y += ystep;
                error += dx;
            }
        }
        return HAL_OK;
    }

    HAL_StatusTypeDef LCD::draw_circle(uint16_t x0, uint16_t y0, uint16_t radius, uint32_t color) const
    {
        int x = 0;
        int y = radius;
        int d = 3 - 2 * radius; // Правильная формула параметра решения

        while (x <= y)
        {
            // Рисуем все 8 симметричных точек
            draw_pixel(x + x0, y + y0, color);
            draw_pixel(y + x0, x + y0, color);
            draw_pixel(-x + x0, y + y0, color);
            draw_pixel(-y + x0, x + y0, color);
            draw_pixel(-x + x0, -y + y0, color);
            draw_pixel(-y + x0, -x + y0, color);
            draw_pixel(x + x0, -y + y0, color);
            draw_pixel(y + x0, -x + y0, color);
            x++;
            if (d < 0)
            {
                d = d + 4 * x + 6;
            }
            else
            {
                y--;
                d = d + 4 * (x - y) + 10;
            }
        }
        return HAL_OK;
    }

    HAL_StatusTypeDef LCD::draw_pixel(uint16_t x, uint16_t y, uint32_t color) const
    {
        if (set_border(x, y, x, y) != HAL_OK)
            return HAL_ERROR;
        uint8_t output[2];
        output[0] = (color >> 8) & 0xFF;
        output[1] = color & 0xFF;
        print_mas(output, 2);
        if (dma_wait() != HAL_OK)
            return HAL_BUSY; // TODO в теории надо от этого избавиться
        return HAL_OK;
    }

    HAL_StatusTypeDef LCD::draw_rect(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint32_t color) const
    {
        // Проверяем все линии на успешное выполнение
        if (draw_line(x_start, y_start, x_end, y_start, color) != HAL_OK)
            return HAL_ERROR; // Верхняя
        if (draw_line(x_start, y_end, x_end, y_end, color) != HAL_OK)
            return HAL_ERROR; // Нижняя
        if (draw_line(x_start, y_start, x_start, y_end, color) != HAL_OK)
            return HAL_ERROR; // Левая
        if (draw_line(x_end, y_start, x_end, y_end, color) != HAL_OK)
            return HAL_ERROR; // Правая

        return HAL_OK;
    }

    HAL_StatusTypeDef LCD::draw_fill_rect(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint32_t color) const
    {
        if (set_border(x_start, y_start, x_end, y_end) != HAL_OK)
            return HAL_ERROR;
        uint8_t output[2];
        output[0] = (color >> 8) & 0xFF;
        output[1] = color & 0xFF;
        for (uint16_t x = x_start; x <= x_end; x++)
        {
            for (uint16_t y = y_start; y <= y_end; y++)
            {
                print_mas(output, 2);
            }
        }
        return HAL_OK;
    }


    HAL_StatusTypeDef LCD::draw_fill_rect_gradient(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color_top, uint16_t color_bottom) const
    {
        dma_wait();

        if (set_border(x_start, y_start, x_end, y_end) != HAL_OK) return HAL_ERROR; //устонавливаем область вывода
        // считываем размеры
        const uint16_t w = x_end - x_start + 1;
        const uint16_t h = y_end - y_start + 1;

        // RGB565 распаковочка
        const int r1 = (color_top >> 11) & 0x1F; //верхний цвет
        const int g1 = (color_top >> 5)  & 0x3F;
        const int b1 = color_top & 0x1F;

        const int r2 = (color_bottom >> 11) & 0x1F;//нижнний цвет
        const int g2 = (color_bottom >> 5)  & 0x3F;
        const int b2 = color_bottom & 0x1F;

        static uint16_t line_buf[160]; // буфер одной строки

        for (uint16_t py = 0; py < h; py++)
        {
            uint16_t screen_y = y_start + py;

            // градиент строки
            int r = r1 + ((r2 - r1) * screen_y) / 128; // интерполяция цвета (формула: color = A + (B - A) * t)
            int g = g1 + ((g2 - g1) * screen_y) / 128;
            int b = b1 + ((b2 - b1) * screen_y) / 128;

            uint16_t color = (r << 11) | (g << 5) | b; // собираем обратно 16-битный цвет
            uint16_t spi_color = (color << 8) | (color >> 8); //меняем порядок байтов

            for (uint16_t x = 0; x < w; x++) line_buf[x] = spi_color; // заполняем строку одним цветом
            write_spi_dma((uint8_t*)line_buf, w * 2 - 1); // передаём строку по дма
            dma_wait();
        }

        return HAL_OK;
    }

    HAL_StatusTypeDef LCD::draw_fill_circle(uint16_t x0, uint16_t y0, uint16_t radius, uint32_t color) const
    {
        uint8_t output[2];
        output[0] = (color >> 8) & 0xFF;
        output[1] = color & 0xFF;
        int x = radius;
        int y = 0;
        int radiusError = 1 - radius;
        int x0_, y0_, x1_, y1_;
        while (x >= y)
        {
            x0_ = -x + x0;
            y0_ = y + y0;
            x1_ = x + x0;
            y1_ = y + y0;
            if (set_border(x0_, y0_, x1_, y1_) == HAL_OK)
            {
                for (int16_t i = -x + x0; i <= x + x0; i++)
                {
                    print_mas(output, 2);
                }
            }
            if (set_border(-x + x0, -y + y0, x + x0, -y + y0) == HAL_OK)
            {
                for (int16_t i = -x + x0; i <= x + x0; i++)
                {
                    print_mas(output, 2);
                }
            }
            if (set_border(-y + x0, x + y0, y + x0, x + y0) == HAL_OK)
            {
                for (int16_t i = -y + x0; i <= y + x0; i++)
                {
                    print_mas(output, 2);
                }
            }

            if (set_border(-y + x0, -x + y0, y + x0, -x + y0) == HAL_OK)
            {
                for (int16_t i = -y + x0; i <= y + x0; i++)
                {
                    print_mas(output, 2);
                }
            }
            y++;
            if (radiusError < 0)
            {
                radiusError += 2 * y + 1;
            }
            else
            {
                x--;
                radiusError += 2 * (y - x) + 1;
            }
        }
        return HAL_OK;
    }

    HAL_StatusTypeDef LCD::scroll_area() const
    {
        HAL_StatusTypeDef error = HAL_OK;
        uint8_t mas[6];
        mas[0] = 0;
        mas[1] = 0;
        mas[2] = 0;
        mas[3] = 0;
        mas[4] = 0;
        mas[5] = 0;
        error = command(CASET);
        if (error != HAL_OK)
            return error;
        error = print_mas(mas, sizeof(mas));
        return error;
    }
    HAL_StatusTypeDef LCD::scroll_start(uint8_t step) const
    {
        HAL_StatusTypeDef error = HAL_OK;
        uint8_t mas[2];
        mas[0] = 0;
        mas[1] = step;
        // error = scroll_area();
        if (error != HAL_OK)
            return error;
        error = command(VSCSAD);
        if (error != HAL_OK)
            return error;
        error = print_mas(mas, sizeof(mas));
        return error;
    }
    HAL_StatusTypeDef LCD::write_char(uint16_t x_start, uint16_t y_start, uint8_t word, uint16_t *mas, uint8_t scale) const
    {
        const uint16_t pixel_w = font_width  * scale;
        const uint16_t pixel_h = font_height * scale;

        const uint16_t size_mas = pixel_w * pixel_h * 2;

        for (uint8_t col = 0; col < 5; col++)
        {
            for (uint8_t row = 0; row < 8; row++)
            {
                if (Char_font1[word - CHAR_OFFSET][col] & (1 << row))
                {
                    uint16_t x0 = x_start + col * pixel_w;
                    uint16_t y0 = y_start + row * pixel_h;

                    set_border( x0, y0, x0 + pixel_w - 1, y0 + pixel_h - 1);
                    print_mas((uint8_t*)mas, size_mas);
                }
            }
        }

        return HAL_OK;
    }

    HAL_StatusTypeDef LCD::print_word(uint16_t x_start, uint16_t y_start, const char *word, uint32_t color, uint8_t scale) const
    {
        uint16_t mas[SIZE_BUF];

        for (uint16_t i = 0; i < SIZE_BUF; i++)
            mas[i] = ((color & 0xff) << 8) | (color >> 8);

        uint16_t x = x_start;

        for (size_t i = 0; word[i] != '\0'; i++)
        {
            uint8_t cha = word[i];

            if (cha > 191)
                cha = cha - 65;

            write_char(x, y_start, cha, mas, scale);

            // шаг с учётом масштаба
            x += (5 * font_width + font_kerning) * scale;
        }

        dma_wait();
        return HAL_OK;
    }

    void LCD::set_font_multiplier(int8_t height, int8_t width, int8_t kerning)
    {
        if (height > 16)
            font_height = 16;
        else if (height < 1)
            font_height = 1;
        else
            font_height = height;
        if (width > 16)
            font_width = 16;
        else if (width < 1)
            font_width = 1;
        else
            font_width = width;
        if (kerning < 0)
            font_kerning = font_width;
        else
            font_kerning = kerning;
    }

    void LCD::led_on() const
    {
        if (led_ != -1)
            ll_gpio_write(led_, 0);
    }
    void LCD::led_off() const
    {
        if (led_ != -1)
            ll_gpio_write(led_, 1);
    }

    
}

