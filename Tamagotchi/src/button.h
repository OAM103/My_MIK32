#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_BUTTONS 10

// Структура кнопки
typedef struct {
    uint16_t x;             // левая граница
    uint16_t y;             // верхняя граница
    uint16_t width;         // ширина кнопки
    uint16_t height;        // высота кнопки
    uint16_t press_color;   // цвет при нажатии
    void (*on_press)(void); // функция-обработчик нажатия (событие после нажатия)
    uint8_t active;         // состояние кнопки
    uint8_t pressed;
} Button;


extern Button buttons[MAX_BUTTONS];
extern uint8_t button_count;

// Добавление кнопки (она сразу отрисуется)
bool Button_Add(uint16_t x, uint16_t y, uint16_t width, uint16_t height, void (*on_press)(void));
// Проверка нажатия тачпада и вызов колбэка с визуальной реакцией
int Buttons_ProcessTouch(int touch_x, int touch_y);
// Отрисовка всех кнопок (при необходимости обновления экрана)
void Button_DrawAll(void);
int process_button();

#endif