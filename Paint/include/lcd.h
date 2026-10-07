/**
 * @file    lcd.h
 * @brief   Драйвер TFT-дисплея на базе ILI9341 с сенсорной панелью:
 *          инициализация и базовая отрисовка.
*/

#ifndef LCD_H
#define LCD_H

/* Includes ------------------------------------------------------------------*/
#include "mik32_hal.h"
#include "bus_spi.h"
#include "bgpio.h"
#include "bdma.h"

/* Define --------------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Инициализация и настройка дисплея
 */
void Lcd_Init(void);

/**
 * @brief          Заливка прямоугольной области через DMA
 * @param  length  Высота в пикселях
 * @param  width   Ширина в пикселях
 * @param  d_l     Смещение по вертикали
 * @param  d_w     Смещение по горизонтали
 * @param  color   Цвет заливки (RGB565)
 */
void ClearMassDMA(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w, uint16_t color);

/**
 * @brief          Отрисовка залитого прямоугольника
 * @param  length  Высота в пикселях
 * @param  width   Ширина в пикселях
 * @param  y       Координата d_l
 * @param  x       Координата d_w
 * @param  color   Цвет заливки (RGB565)
 */
void draw_rect(uint16_t length, uint16_t width, uint16_t y, uint16_t x, uint16_t color);

void drawChar(uint16_t x, uint16_t y, char ch, uint16_t color, uint16_t bg, uint8_t scale);
// Вывод строки
void drawText(uint16_t x, uint16_t y, const char *text, uint16_t color, uint16_t bg, uint8_t scale);

#endif /* LCD_H */
