#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_BUTTONS 100
#define BUTTON_HEIGHT 50
#define BUTTON_MARGIN 10
#define BUTTON_WIDTH 250

// Структура кнопки
// typedef struct {
//     uint16_t x;             // левая граница
//     uint16_t y;             // верхняя граница
//     uint16_t width;         // ширина кнопки
//     uint16_t height;        // высота кнопки
//     uint16_t press_color;   // цвет при нажатии
//     void (*on_press)(void); // функция-обработчик нажатия (событие после нажатия)
//     uint8_t active;         // состояние кнопки
//     uint8_t pressed;
// } Button;

typedef struct {
    uint16_t x;
    uint16_t y;          // логическая Y
    uint16_t width;
    uint16_t height;
    const char *label;
    void (*on_press)(void);
    uint8_t active;
} Button;

extern Button buttons[MAX_BUTTONS];
extern uint16_t button_count;
extern int scroll_offset;
extern int max_scroll;

// Добавление кнопки (она сразу отрисуется)
bool Button_Add(const char *label, void (*on_press)(void));
bool Button_Add_Standart(uint16_t x, uint16_t y, uint16_t width, uint16_t height, void (*on_press)(void));
// Проверка нажатия тачпада и вызов колбэка с визуальной реакцией
int Buttons_ProcessTouch(int touch_x, int touch_y);
int Buttons_ProcessTouch_Scroll(int touch_x, int touch_y);
void ProcessTouchScroll(void);
void UpdateScroll(int touch_y, int is_touching);
// Отрисовка всех кнопок (при необходимости обновления экрана)
void Button_DrawAll(void);
void DrawButtons(void);
int process_button();

#endif
