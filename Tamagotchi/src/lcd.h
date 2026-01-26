#ifndef LCD_H
#define LCD_H

#include "mik32_hal.h"

void Lcd_select();
void Lcd_unselect();
void Lcd_Writ_Bus(uint8_t d); //отправляет 1 байт данных или команды
void Lcd_Write_Com(unsigned char VH);//отправляет команду(RS)
void Lcd_Write_Data(uint8_t VH); //отправляет данные
void Lcd_Write_Com_Data(unsigned char com, unsigned char dat);//отправляет команду com и данные
void Address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);//Устанавливает область памяти LCD, в которую будут записываться данные
void Lcd_Init(void); // настройки экрана
void LCD_Clear(unsigned int j); //очишает весь экран, заполняя цветом j ?
void drawText(uint16_t x, uint16_t y, const char *text, uint16_t color, uint16_t bg, uint8_t scale);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void draw_line(int x_start, int y_start, int x_end, int y_end, uint16_t color);
void draw_rect(int x, int y, int width, int height, int thickness, uint16_t color);// контур прямоугольника
void ClearMass(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w, uint16_t color); //закрасить обрасть
void ClearMassDMA(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w, uint16_t color); //закрасить обрасть с 
void GradientDMA(uint16_t height, uint16_t width, uint16_t d_l, uint16_t d_w, uint16_t colorTop, uint16_t colorBottom);
void PrintMass(const uint16_t *mas, uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w );
void PrintMassDMA(const uint16_t *mas, uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w);
void PrintMassDMA_Optimized(const uint16_t *mas, uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w, uint16_t background_color);
void PrintMassDMA_Overlay( const uint16_t *bg, uint16_t bg_w, uint16_t bg_h, const uint16_t *fg, uint16_t fg_w, uint16_t fg_h, uint16_t fg_x, uint16_t fg_y, uint16_t d_l, uint16_t d_w, uint16_t background_color);
void PrintMassDMA_Part(const uint16_t *mas, uint16_t part, uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w, uint16_t background_color);
void PrintMassDMA_DoubleBuffer(const uint16_t *mas,uint16_t length,uint16_t width,uint16_t d_l,uint16_t d_w);
void scrollImageLoop(const uint16_t *image, uint16_t height, uint16_t width);

uint16_t GetBackgroundColor(uint32_t status);

#endif