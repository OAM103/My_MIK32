// заголовок с типами

#ifndef LCD_I2C_H
#define LCD_I2C_H
#include <stdint.h>
#include "mik32_hal_i2c.h"

//настройки нашегор дисплея
typedef struct {
    I2C_HandleTypeDef *hi2c; // указатель на I2C в HAL
    uint8_t addr; // адрес дисплея (0x27)
    uint8_t cols; // кол-во колонок 16
    uint8_t rows; // кол-во строк 2 (передает пользователь)
    uint8_t backlight; // состояние подсветки
    uint8_t displayfunction; // "паспорт дисплея"
    uint8_t displaycontrol; // биты включения дисплея, курсора, мигания
    uint8_t displaymode; //режим ввода текста
    uint8_t numlines; //фактическое кол-во строк
} LCD_I2C; 

//Инициализация (запускает дисплей, принимает указатель на структуру lcd, сохраняет в неё hi2c, адрес, размеры) (далее инициализация HD44780 в 4-битный режим)
void LCD_Init(LCD_I2C *lcd, I2C_HandleTypeDef *hi2c, uint8_t address, uint8_t cols, uint8_t rows);

//Основные команды для дисплея
void LCD_Clear(LCD_I2C *lcd); //очистить экран
void LCD_Home(LCD_I2C *lcd); //вернуть курор в начало
void LCD_SetCursor(LCD_I2C *lcd, uint8_t col, uint8_t row); //переместить курсор
void LCD_Print(LCD_I2C *lcd, const char *str);//вывести строку
void LCD_Write(LCD_I2C *lcd, uint8_t value); // вывести символ
void LCD_CreateChar(LCD_I2C *lcd, uint8_t location, uint8_t charmap[]); // создать свой символ

//Дисплейные опции
void LCD_NoDisplay(LCD_I2C *lcd); //выключить экран
void LCD_Display(LCD_I2C *lcd); //включить экран
void LCD_NoCursor(LCD_I2C *lcd); //скрыть курсор
void LCD_Cursor(LCD_I2C *lcd); //показать курсор
void LCD_NoBlink(LCD_I2C *lcd); //выключить мигающий курсор
void LCD_Blink(LCD_I2C *lcd); //включить мигающий курсор

// Подсветка
void LCD_BacklightOn(LCD_I2C *lcd);//вкл
void LCD_BacklightOff(LCD_I2C *lcd);//выкл

#endif // LCD_I2C_H