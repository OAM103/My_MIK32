#include "button.h"
#include "lcd.h"
#include "touchpad.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

Button buttons[MAX_BUTTONS];
int max_scroll = 0;

//Добавление кнопки и её отрисовка
bool Button_Add_Standart(uint16_t x, uint16_t y, uint16_t width, uint16_t height, void (*on_press)(void)) {
    if (button_count >= MAX_BUTTONS) return false;

    buttons[button_count].x = x;
    buttons[button_count].y = y;
    buttons[button_count].width = width;
    buttons[button_count].height = height;
    buttons[button_count].on_press = on_press;
    buttons[button_count].active = 1;

    button_count++;
    return true;
}
bool Button_Add(const char *label, void (*on_press)(void))
{
    if (button_count >= MAX_BUTTONS) return false;

    Button *btn = &buttons[button_count];
    btn->x = 10;
    btn->y = WINDOW_Y + button_count * (BUTTON_HEIGHT + BUTTON_MARGIN);
    btn->width = BUTTON_WIDTH;
    btn->height = BUTTON_HEIGHT;
    btn->label = label;
    btn->on_press = on_press;
    btn->active = 1;
    button_count++;

    int content_height = button_count * (BUTTON_HEIGHT + BUTTON_MARGIN);
    max_scroll = (content_height > WINDOW_H) ? (content_height - WINDOW_H) : 0;

    return true;
}
int Buttons_ProcessTouch(int touch_x, int touch_y)
{
    for (uint8_t i = 0; i < button_count; i++) {
        Button *btn = &buttons[i];
        if (!btn->active) continue;

        int visible_y = btn->y - scroll_offset + WINDOW_Y;

        // if (touch_x >= btn->x && touch_x <= (btn->x + btn->width) &&
        //     touch_y >= visible_y && touch_y <= (visible_y + btn->height)){
        if (touch_y >= btn->x && touch_y <= (btn->x + btn->width) &&
            touch_x >= btn->y && touch_x <= (btn->y + btn->height)) {

            if (btn->on_press) btn->on_press();
            return 1;
        }
    }
    return 0;
}
int selected_button = -1;

int Buttons_ProcessTouch_Scroll(int touch_x, int touch_y)
{
    // Проверяем, что касание вообще в зоне кнопок (по X)
    if (touch_x >= SCROLL_ZONE_H) return 0;
    // Проверяем, что касание в вертикальном окне
    if (touch_y < WINDOW_Y || touch_y > WINDOW_Y + WINDOW_H)  return 0;
    for (uint16_t i = 0; i < button_count; i++)
    {
        Button *btn = &buttons[i];
        if (!btn->active) continue;
        // Реальное положение кнопки на экране с учетом скролла
        int visible_y = btn->y - scroll_offset + WINDOW_Y - 20;
        // Если кнопка не видна, пропускаем
        if (visible_y + btn->height < WINDOW_Y || visible_y > WINDOW_Y + WINDOW_H) continue;
        // Проверка попадания в кнопку
        if (touch_x >= btn->x && touch_x <= (btn->x + btn->width) && touch_y >= visible_y && touch_y <= (visible_y + btn->height))
        {
            // Запоминаем выбранную кнопку
            selected_button = i;
            // Перерисовываем кнопки (для рамки)
            DrawButtons();
            // Вызываем обработчик
            if (btn->on_press) btn->on_press();  return 1;  // кнопка обработана
        }
    }
    return 0; // ничего не нажато
}
int scroll_offset = 0;
uint16_t button_count = 0;

void DrawButtons(void)
{
    ClearMassDMA(320, WINDOW_H, 0, WINDOW_Y, 0x0000);
    ClearMassDMA(SCROLL_ZONE_W, WINDOW_H, SCROLL_ZONE_H, WINDOW_Y, 0x0003);

    for (uint16_t i = 0; i < button_count; i++)
    {
        Button *btn = &buttons[i];

        int visible_y = btn->y - scroll_offset;

        int btn_top = visible_y;
        int btn_bottom = visible_y + btn->height;

        int window_top = WINDOW_Y;
        int window_bottom = WINDOW_Y + WINDOW_H;

        // если полностью вне окна
        if (btn_bottom <= window_top || btn_top >= window_bottom) continue;

        int draw_y = btn_top;
        int draw_height = btn->height;

        if (btn_top < window_top){
            draw_height -= (window_top - btn_top);
            draw_y = window_top;
        }
        if (btn_bottom > window_bottom) draw_height -= (btn_bottom - window_bottom);

        // рисуем только видимую часть
        ClearMassDMA(btn->width, draw_height,  btn->x, draw_y, 0x0000);

        // рамка
        if (i == selected_button)
        {
            int btn_top = visible_y;
            int btn_bottom = visible_y + btn->height;

            int window_top = WINDOW_Y;
            int window_bottom = WINDOW_Y + WINDOW_H;

            // рамку рисуем только если кнопка полностью внутри окна
            if (btn_top >= window_top && btn_bottom <= window_bottom) draw_rect(btn->x, visible_y, btn->width, btn->height, 1, 0x00FF);
        }
        // текст рисуем только если кнопка полностью видима
        if (draw_height == btn->height) drawText(btn->x + 5, draw_y + 10, btn->label, 0xFFFF, 0x0000, 2);
    }
}
int process_button(void)
{
    static uint8_t was_pressed = 0;

    int x = read_filtered_coordinate(READ_X, 5);
    int y = read_filtered_coordinate(READ_Y, 5);

    // палец НЕ нажат
    if (x <= 0 || y <= 0)
    {
        was_pressed = 0;
        return 0;
    }

    // палец нажат, но он уже был нажат раньше игнор
    if (was_pressed)
        return 0;

    was_pressed = 1;

    int coord_x = transform(x, 2032, 150, 320, 0);
    int coord_y = 480 - transform(y, 2032, 100, 480, 0);
    
    return Buttons_ProcessTouch(coord_x, coord_y);
}

void UpdateScroll(int touch_y, int is_touching)
{
    static int last_touch_y = 0;
    static uint8_t touching = 0;

    if (!is_touching){
        touching = 0;
        return;
    }
    if (!touching){
        touching = 1;
        last_touch_y = touch_y;
        return;
    }
    int dy = touch_y - last_touch_y;
    last_touch_y = touch_y;

    // фильтр шума
    if (abs(dy) < 2) return;
    scroll_offset -= 4 * dy;
    if (scroll_offset < 0) scroll_offset = 0;
    if (scroll_offset > max_scroll)  scroll_offset = max_scroll;
    DrawButtons();
}

void ProcessTouchScroll(void)
{
    int x = read_filtered_coordinate(READ_X, 5);
    int y = read_filtered_coordinate(READ_Y, 5);

    int is_touching = 1;

    if (x <= 0 || y <= 0 || y >= 2032 || x >= 2032)
        is_touching = 0;

    if (!is_touching)
    {
        UpdateScroll(0, 0);
        return;
    }

    int coord_x = transform(x, 2032, 150, 320, 0);
    int coord_y = 480 - transform(y, 2032, 100, 480, 0);

    if (coord_x >= SCROLL_ZONE_H - 20 && (coord_y >= WINDOW_Y && coord_y <= WINDOW_H + WINDOW_Y)) UpdateScroll(coord_y, 1);
    else Buttons_ProcessTouch_Scroll(coord_x, coord_y);
}
