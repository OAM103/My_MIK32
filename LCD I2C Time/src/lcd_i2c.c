//Работа LCD на HD44780 через I2C-расширитель PCF8574

#include "lcd_i2c.h"
#include <string.h>
#include "mik32_hal_gpio.h"
#include <stdio.h>
#include "xprintf.h"

// Биты, принятые в стандартных I2C(PCF8574)
#define LCD_EN   0x04  // говорит LCD принять данные
#define LCD_RW   0x02  // выбор режима чтения или записи
#define LCD_RS   0x01  // выбор регистра(команд(отчистка, курсор, режим, дисплей и тд.) или данных(отображаемые символы))
#define LCD_BACKLIGHT 0x08 //включена подсветка
#define LCD_NOBACKLIGHT 0x00 //выключена подсветка

//Команды HD44780
#define LCD_CLEARDISPLAY 0x01 //очистка дисплея
#define LCD_RETURNHOME   0x02 //возврат курсора на начало
#define LCD_ENTRYMODESET 0x04 //настройка режима ввода символов
#define LCD_DISPLAYCONTROL 0x08 // управление дисплеем курсором и миганием
#define LCD_CURSORSHIFT  0x10 //сдвиг курсора или содержимого дисплея
#define LCD_FUNCTIONSET  0x20 //настройка дисплея, кол-во строк, размер шрифта
#define LCD_SETCGRAMADDR 0x40 //задание адреса для пользовательских символов
#define LCD_SETDDRAMADDR 0x80 //установка позиции курсора

// Флаги для режима ввода(поведение курсора и дисплея после записи символа)
#define LCD_ENTRYLEFT          0x02 // курсор сдвигается вправо
#define LCD_ENTRYSHIFTDECREMENT 0x00 //экран не сдвигается

// Флаги для управления дисплеем
#define LCD_DISPLAYON  0x04 //включаем дисплей
#define LCD_CURSORON   0x02 //включаем курсор
#define LCD_BLINKON    0x01 //включаем мигание

static void lcd_expanderWrite(LCD_I2C *lcd, uint8_t data);//1 байт на расширитель -> подсветка
static void lcd_pulseEnable(LCD_I2C *lcd, uint8_t data);// генерирует импульс EN, иначе LCD не прочитает байт
static void lcd_write4bits(LCD_I2C *lcd, uint8_t value);// 4-битный режим по частям
static void lcd_send(LCD_I2C *lcd, uint8_t value, uint8_t mode); //отправляет полный байт
static void delay_us(volatile uint32_t us); //задержка

static void delay_us(volatile uint32_t us) //задержка для импульса EN в мкс
{
    volatile uint32_t i;
    while (us--) {
        for (i = 0; i < 10; ++i) asm volatile ("nop");
    }
}

//Запись 1 байта на PCF8574 
static void lcd_expanderWrite(LCD_I2C *lcd, uint8_t data)
{
    uint8_t buf[1]; //форм. байт -> сост. подсветки
    buf[0] = data | (lcd->backlight ? LCD_BACKLIGHT : LCD_NOBACKLIGHT);
    HAL_I2C_Master_Transmit(lcd->hi2c, (uint16_t)(lcd->addr), buf, 1, 1000); //адрес I2C 7-бит
    if(HAL_I2C_Master_Transmit(lcd->hi2c, (uint16_t)(lcd->addr), buf, 1, 1000)!=HAL_OK) xprintf("I2C write error\n");
}

// генерируем импульс EN
static void lcd_pulseEnable(LCD_I2C *lcd, uint8_t data)
{
    lcd_expanderWrite(lcd, data | LCD_EN);
    //HAL_DelayMs(50);
    delay_us(1); // > 450 мкс
    lcd_expanderWrite(lcd, data & ~LCD_EN);
    delay_us(50); // >37 мкс

}

//Запись 4-х бит
static void lcd_write4bits(LCD_I2C *lcd, uint8_t value)
{
    lcd_expanderWrite(lcd, value);
    lcd_pulseEnable(lcd, value);
}

// Отправка байта (mode = 0 - команда/ mode = 1 - данные)
static void lcd_send(LCD_I2C *lcd, uint8_t value, uint8_t mode)
{
    uint8_t highnib = value & 0xF0;
    uint8_t lownib  = (value << 4) & 0xF0;
    lcd_write4bits(lcd, highnib | mode);
    lcd_write4bits(lcd, lownib  | mode);
}


void LCD_Init(LCD_I2C *lcd, I2C_HandleTypeDef *hi2c, uint8_t address, uint8_t cols, uint8_t rows)
{
    lcd->hi2c = hi2c;
    lcd->addr = address;
    lcd->cols = cols;
    lcd->rows = rows;
    lcd->backlight = 1;
    lcd->displayfunction = 0x00;
    //lcd->displayfunction = 0x00; 
    if (rows > 1) lcd->displayfunction |= 0x08; 
    lcd->displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT; //режим вывода
    HAL_DelayMs(50);
    // Сбрасываем экспандер и выключаем подсветку временно
    lcd_expanderWrite(lcd, 0x00);
    HAL_DelayMs(1000);
    // Инициализация в 4-bit режиме 
    lcd_write4bits(lcd, 0x03 << 4);
    HAL_DelayMs(5);
    lcd_write4bits(lcd, 0x03 << 4);
    delay_us(150);
    lcd_write4bits(lcd, 0x03 << 4);
    delay_us(150);
    lcd_write4bits(lcd, 0x02 << 4); 

    lcd_send(lcd, LCD_FUNCTIONSET | lcd->displayfunction, 0);//настройка функций дисплея
    // включение дисплея
    lcd->displaycontrol = LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKON;
    lcd->displaycontrol = LCD_DISPLAYON; 
    lcd_send(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol, 0);//экран включен, курсор не мигает

    LCD_Clear(lcd);

    // режим выода
    lcd_send(lcd, LCD_ENTRYMODESET | lcd->displaymode, 0);

    // курсор на место
    lcd_send(lcd, LCD_RETURNHOME, 0);
    HAL_DelayMs(2);
}

void LCD_Clear(LCD_I2C *lcd)
{
    lcd_send(lcd, LCD_CLEARDISPLAY, 0);
    HAL_DelayMs(2);
}

void LCD_Home(LCD_I2C *lcd)
{
    lcd_send(lcd, LCD_RETURNHOME, 0);
    HAL_DelayMs(2);
}

void LCD_SetCursor(LCD_I2C *lcd, uint8_t col, uint8_t row)
{
    const uint8_t row_offsets_2line[] = { 0x00, 0x40 };
    if (row >= lcd->rows) row = lcd->rows - 1;
    lcd_send(lcd, LCD_SETDDRAMADDR | (col + row_offsets_2line[row]), 0);
}

void LCD_Print(LCD_I2C *lcd, const char *str)
{
    while (*str) {
        LCD_Write(lcd, (uint8_t)(*str));
        str++;
    }
}

void LCD_Write(LCD_I2C *lcd, uint8_t value)
{
    lcd_send(lcd, value, LCD_RS);
}

void LCD_CreateChar(LCD_I2C *lcd, uint8_t location, uint8_t charmap[])
{
    location &= 0x7; 
    lcd_send(lcd, LCD_SETCGRAMADDR | (location << 3), 0);
    for (int i = 0; i < 8; i++) {
        LCD_Write(lcd, charmap[i]);
    }
}

void LCD_NoDisplay(LCD_I2C *lcd) {
    lcd->displaycontrol &= ~LCD_DISPLAYON;
    lcd_send(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol, 0);
}
void LCD_Display(LCD_I2C *lcd) {
    lcd->displaycontrol |= LCD_DISPLAYON;
    lcd_send(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol, 0);
}

void LCD_NoCursor(LCD_I2C *lcd) {
    lcd->displaycontrol &= ~LCD_CURSORON;
    lcd_send(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol, 0);
}
void LCD_Cursor(LCD_I2C *lcd) {
    lcd->displaycontrol |= LCD_CURSORON;
    lcd_send(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol, 0);
}

void LCD_NoBlink(LCD_I2C *lcd) {
    lcd->displaycontrol &= ~LCD_BLINKON;
    lcd_send(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol, 0);
}
void LCD_Blink(LCD_I2C *lcd) {
    lcd->displaycontrol |= LCD_BLINKON;
    lcd_send(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol, 0);
}

void LCD_BacklightOn(LCD_I2C *lcd) {
    lcd->backlight = 1;
    lcd_expanderWrite(lcd, 0);
}
void LCD_BacklightOff(LCD_I2C *lcd) {
    lcd->backlight = 0;
    lcd_expanderWrite(lcd, 0);
}