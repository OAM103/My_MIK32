#include "button.h"
#include "lcd.h"
#include "touchpad.h"
#include "animation.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_BUTTONS 10

Button buttons[MAX_BUTTONS];
uint8_t button_count = 0;

// Добавление кнопки и её отрисовка
bool Button_Add(uint16_t x, uint16_t y, uint16_t width, uint16_t height, void (*on_press)(void)) {
    if (button_count >= MAX_BUTTONS) return false;

    buttons[button_count].x = x;
    buttons[button_count].y = y;
    buttons[button_count].width = width;
    buttons[button_count].height = height;
    buttons[button_count].on_press = on_press;
    buttons[button_count].active = 0;

    button_count++;
    return true;
}


// // Проверка нажатия и визуальная реакция
// void Buttons_ProcessTouch(int touch_x, int touch_y) {
//     for (uint8_t i = 0; i < button_count; i++) {
//         Button *btn = &buttons[i];
//         if (!btn->active) continue;
//         if (touch_y >= btn->x && touch_y <= (btn->x + btn->width) &&
//             touch_x >= btn->y && touch_x <= (btn->y + btn->height)) {
//             // Вызываем обработчик
//             if (btn->on_press) btn->on_press();
//             break; // Одна кнопка за одно касание
//         }
//     }
// }

// int process_button(){
//     int x = read_filtered_coordinate(READ_X, 5);
//     int y = read_filtered_coordinate(READ_Y, 5);

//     if (x > 0 && y > 0) {
//         int coord_x = transform(x, 2032, 150, 320, 0);
//         int coord_y = 480 - transform(y, 2032, 100, 480, 0);

//         Buttons_ProcessTouch(coord_x, coord_y);
//         return 1;
//     }
//     return 0;
// }

int Buttons_ProcessTouch(int touch_x, int touch_y)
{
    for (uint8_t i = 0; i < button_count; i++) {
        Button *btn = &buttons[i];
        if (!btn->active) continue;

        if (touch_y >= btn->x && touch_y <= (btn->x + btn->width) &&
            touch_x >= btn->y && touch_x <= (btn->y + btn->height)) {

            if (btn->on_press)
                btn->on_press();

            return 1;
        }
    }
    return 0;
}

int process_button(void)
{
    int x = read_filtered_coordinate(READ_X, 5);
    int y = read_filtered_coordinate(READ_Y, 5);

    if (x <= 0 || y <= 0)
        return 0;

    int coord_x = transform(x, 2032, 150, 320, 0);
    int coord_y = 480 - transform(y, 2032, 100, 480, 0);

    return Buttons_ProcessTouch(coord_x, coord_y);
}