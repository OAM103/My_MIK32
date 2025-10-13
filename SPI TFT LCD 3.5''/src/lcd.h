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
void Lcd_Init(void);
void LCD_Clear(unsigned int j); //очишает весь экран, заполняя цветом j (нет красного?может и зеленого?)
void writeChar(char t, uint8_t dlta);
void clearChar(char t, uint8_t dlta);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
//void ClearMass(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w);
void ClearMass(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w, uint16_t color);
void PrintMass(const uint16_t *mas, uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w );
void PrintMassDMA(const uint16_t *mas, uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w);
//void PrintMassDMA_Optimized(const uint16_t *mas, uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w, uint16_t char_color, uint16_t background_color);
void PrintMassDMA_Optimized(const uint16_t *mas, uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w);
void PrintMassDMA_DoubleBuffer(const uint16_t *mas,uint16_t length,uint16_t width,uint16_t d_l,uint16_t d_w);
#endif