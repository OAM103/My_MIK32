#ifndef LCD_H
#define LCD_H

#include "mik32_hal.h"

#define WIDTH 240
#define LENGHT 320

typedef struct {
    const uint8_t *data;
    int len;
}FrameRLE_t;

void Lcd_select();
void Lcd_unselect();
void Lcd_Writ_Bus(uint8_t d); //отправляет 1 байт данных или команды
void Lcd_Write_Com(unsigned char VH);//отправляет команду(RS)
void Lcd_Write_Data(uint8_t VH); //отправляет данные
void Lcd_Write_Com_Data(unsigned char com, unsigned char dat);//отправляет команду com и данные
void Address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);//Устанавливает область памяти LCD, в которую будут записываться данные
void Lcd_Init(void);
void LCD_Clear(unsigned int j); //очишает весь экран, заполняя цветом j ?
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void ClearMass(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w, uint16_t color);
void ClearMassDMA(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w, uint16_t color);
void ClearMassDMA_Fast(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w, uint16_t color);
void PrintMass(const uint16_t *mas, uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w );
void PrintMassDMA(const uint16_t *mas, uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w);
void PrintMassDMA_Fast(const uint16_t *mas,uint16_t length,uint16_t width,uint16_t d_l,uint16_t d_w);
void PrintMassDMA_Optimized(const uint16_t *mas, uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w, uint16_t color);
void drawChar(uint16_t x, uint16_t y, char ch, uint16_t color, uint16_t bg, uint8_t scale);
void drawText(uint16_t x, uint16_t y, const char *text, uint16_t color, uint16_t bg, uint8_t scale);
void drawCircle(int x0, int y0, int r, uint16_t color);
void GradientDMA(uint16_t height, uint16_t width, uint16_t d_l, uint16_t d_w, uint16_t colorTop, uint16_t colorBottom);
void gradientCircle(int cx, int cy, int radius, int grad_radius, uint16_t colorCenter, uint16_t colorBg);
void rect(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w, uint16_t color);
void PrintFramesBW_RLE_DMA(const FrameRLE_t *frames, int frame_count);

#endif