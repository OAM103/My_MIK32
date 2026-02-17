#include "button.h"
#include "lcd.h"
#include "touchpad.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUTTONS 10

static Button buttons[MAX_BUTTONS];
static uint8_t button_count = 0;

// Добавление кнопки и её отрисовка
// bool Button_Add(uint16_t x, uint16_t y, uint16_t width, uint16_t height,uint16_t color, void (*on_press)(void)) {
//     if (button_count >= MAX_BUTTONS) return false;

//     buttons[button_count].x = x;
//     buttons[button_count].y = y;
//     buttons[button_count].width = width;
//     buttons[button_count].height = height;
//     buttons[button_count].color = color;
//     buttons[button_count].on_press = on_press;

//     // Нарисовать кнопку сразу на экране
//     rect(height, width, y, x, color);

//     button_count++;
//     return true;
// }

bool Button_Add(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                uint16_t color, const char* label, void (*on_press)(void)) {
    if (button_count >= MAX_BUTTONS) return false;

    buttons[button_count].x = x;
    buttons[button_count].y = y;
    buttons[button_count].width = width;
    buttons[button_count].height = height;
    buttons[button_count].color = color;
    buttons[button_count].label = label;
    buttons[button_count].on_press = on_press;

    // Нарисовать прямоугольник кнопки
    ClearMassDMA(height, width, y, x, color);

    // Нарисовать текст 
    drawText(x+27, y+7, label, 0x0000, color,3);         

    button_count++;
    return true;
}

// Проверка нажатия и визуальная реакция
void Buttons_ProcessTouch(int touch_x, int touch_y) {
    for (uint8_t i = 0; i < button_count; i++) {
        Button *btn = &buttons[i];
        if (touch_x >= btn->x && touch_x <= (btn->x + btn->width) &&
            touch_y >= btn->y && touch_y <= (btn->y + btn->height)) {

            // Вызываем обработчик
            if (btn->on_press) btn->on_press();

            break; // Одна кнопка за одно касание
        }
    }
}

void process_button(){
    int x = read_filtered_coordinate(READ_X, 5);
    int y = read_filtered_coordinate(READ_Y, 5);

    if (x > 0 && y > 0) {
        int coord_x = transform(x, 2032, 150, 320, 0);
        int coord_y = transform(y, 2032, 100, 480, 0);

        Buttons_ProcessTouch(coord_x, coord_y);
    }

    HAL_DelayMs(50);
}
