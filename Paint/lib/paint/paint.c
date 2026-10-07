/**
 * @file    paint.c
 * @brief   Логика графического редактора Paint
 */

#include "paint.h"
#include "paint_history.h"
#include "lcd.h"

/* 
 * СОСТОЯНИЕ PAINT
 */

static int paint_clear_button_active = 0;
static int paint_brush_menu_active = 0;
static int paint_brush_button_active = 0;
static int paint_touch_started = 0;
static int paint_rgb_menu_active = 0;
static int paint_rgb_selection_active = 0;
static int paint_brush_menu_2_active = 0;
static int paint_touch_action_done = 0;
static int paint_pipette_last_x = -1;
static int paint_pipette_last_y = -1;
static int paint_rgb_last_x = -1;
static int paint_rgb_last_y = -1;

static uint8_t paint_rgb_red = 0;
static uint8_t paint_rgb_green = 0;
static uint8_t paint_rgb_blue = 0;


/* 
 * UNDO / REDO
 */

/**
 * @brief Проверяет нажатие кнопки Undo.
 */
static int paint_is_undo_button_pressed(int x, int y)
{
    return x >= PAINT_UNDO_BUTTON_X + PAINT_UNDO_ICON_X_OFFSET &&
           x <  PAINT_UNDO_BUTTON_X + PAINT_UNDO_ICON_X_OFFSET + PAINT_UNDO_ICON_WIDTH &&
           y >= PAINT_UNDO_BUTTON_Y + PAINT_UNDO_ICON_Y_OFFSET &&
           y <  PAINT_UNDO_BUTTON_Y + PAINT_UNDO_ICON_Y_OFFSET + PAINT_UNDO_ICON_HEIGHT;
}

/**
 * @brief Проверяет нажатие кнопки Redo.
 */
static int paint_is_redo_button_pressed(int x, int y)
{
    return x >= PAINT_REDO_BUTTON_X + PAINT_REDO_ICON_X_OFFSET &&
           x <  PAINT_REDO_BUTTON_X + PAINT_REDO_ICON_X_OFFSET + PAINT_REDO_ICON_WIDTH &&
           y >= PAINT_REDO_BUTTON_Y + PAINT_REDO_ICON_Y_OFFSET &&
           y <  PAINT_REDO_BUTTON_Y + PAINT_REDO_ICON_Y_OFFSET + PAINT_REDO_ICON_HEIGHT;
}

static const uint8_t undo_bitmap[21][21] =
{
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0},
    {0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0},
    {1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0},
    {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0},
    {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0},
    {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
    {0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};
static const uint8_t redo_bitmap[21][21] =
{

    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
    {0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0},
    {0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
    {0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
    {0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0},
    {0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
    {0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};
/**
 * @brief Рисует пиксельную стрелку Undo на кнопке
 */
static void paint_draw_undo_icon(void)
{
    uint8_t row;
    uint8_t col;

    for (col = 0; col < 21; col++) {
        for (row = 0; row < 21; row++) {
            if (undo_bitmap[col][row]) ClearMassDMA(1, 1, PAINT_UNDO_BUTTON_Y + 10 + row, PAINT_UNDO_BUTTON_X + 8 + col, 0x2240);
        }
    }
}
/**
 * @brief Рисует пиксельную стрелку Redo на кнопке
 */
static void paint_draw_redo_icon(void)
{
    uint8_t row;
    uint8_t col;

    for (col = 0; col < 21; col++) {
        for (row = 0; row < 21; row++) {
            if (redo_bitmap[col][row]) ClearMassDMA(1, 1, PAINT_UNDO_BUTTON_Y + 10 + row - 40, PAINT_UNDO_BUTTON_X + 8 + col, 0x2240);
        }
    }
}
/**
 * @brief Рисует кнопки Undo и Redo со стрелками.
 */
static void paint_draw_undo_redo_buttons(void)
{
    uint16_t current_color;
    uint8_t current_brush_size;

    current_color = touch_get_color();
    current_brush_size = touch_get_brush_size();

    draw_rect(
        PAINT_UNDO_BUTTON_HEIGHT,
        PAINT_UNDO_BUTTON_WIDTH,
        PAINT_UNDO_BUTTON_Y,
        PAINT_UNDO_BUTTON_X,
        PAINT_UNDO_BUTTON_COLOR
    );

    draw_rect(
        PAINT_REDO_BUTTON_HEIGHT,
        PAINT_REDO_BUTTON_WIDTH,
        PAINT_REDO_BUTTON_Y,
        PAINT_REDO_BUTTON_X,
        PAINT_REDO_BUTTON_COLOR
    );

    paint_draw_undo_icon();
    paint_draw_redo_icon();

    touch_set_color(0x2240);
    touch_set_brush_size(2);


    // Восстанавливаем настройки кисти пользователя
    touch_set_color(current_color);
    touch_set_brush_size(current_brush_size);
}

/**
 * @brief Полностью очищает область одного RGB-ползунка.
 */
static void paint_clear_rgb_slider(int x)
{
    draw_rect(
        PAINT_RGB_BAR_HEIGHT,
        PAINT_RGB_BAR_WIDTH + 8,
        PAINT_RGB_BAR_Y,
        x - 4,
        PAINT_RGB_DARK
    );
}

/**
 * @brief Рисует один RGB-ползунок
 */
static void paint_draw_rgb_slider(int x, uint8_t value, uint16_t color)
{
    int filled_height;
    int knob_y;
    filled_height = (value * (PAINT_RGB_BAR_HEIGHT - 2)) / 255;
    if (filled_height < 2) filled_height = 2;
    if (filled_height > PAINT_RGB_BAR_HEIGHT - 2) filled_height = PAINT_RGB_BAR_HEIGHT - 2;
    draw_rect(
        PAINT_RGB_BAR_HEIGHT,
        PAINT_RGB_BAR_WIDTH,
        PAINT_RGB_BAR_Y,
        x,
        PAINT_RGB_DARK
    );
    draw_rect(
        filled_height,
        PAINT_RGB_BAR_WIDTH,
        PAINT_RGB_BAR_Y + PAINT_RGB_BAR_HEIGHT -  filled_height,
        x,
        color
    );
    knob_y = PAINT_RGB_BAR_Y + PAINT_RGB_BAR_HEIGHT - filled_height - 5;
    if (knob_y < PAINT_RGB_BAR_Y) knob_y = PAINT_RGB_BAR_Y;
    if (knob_y > PAINT_RGB_BAR_Y + PAINT_RGB_BAR_HEIGHT - 10) knob_y =PAINT_RGB_BAR_Y + PAINT_RGB_BAR_HEIGHT - 10;
    draw_rect(10, PAINT_RGB_BAR_WIDTH + 8, knob_y, x - 4, PAINT_RGB_LIGHT );
}

/**
 * @brief Перерисовывает только один RGB-ползунок.
 */
static void paint_draw_rgb_slider_by_column(int column)
{
    int x;
    uint8_t value;
    uint16_t color;

    if (column == 0) {
        x = PAINT_RGB_RED_X;
        value = paint_rgb_red;
        color = PAINT_RGB_RED_COLOR;
    }
    else if (column == 1) {
        x = PAINT_RGB_GREEN_X;
        value = paint_rgb_green;
        color = PAINT_RGB_GREEN_COLOR;
    }
    else if (column == 2) {
        x = PAINT_RGB_BLUE_X;
        value = paint_rgb_blue;
        color = PAINT_RGB_BLUE_COLOR;
    }
    else {
        return;
    }

    /* Сначала стираем старый ползунок вместе с наконечником */
    paint_clear_rgb_slider(x);

    /* Затем рисуем его в новой позиции */
    paint_draw_rgb_slider(x, value, color);
}

/**
 * @brief Пиксельная маска пипетки размером 24x24 пикселя
 * 0 — прозрачный пиксель
 * 1 — пиксель контура пипетки
 */
static const uint8_t pipette_bitmap[24][24] =
{
    {0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,0,0,0,1,1,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,1,1,1,1,1,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,1,1,1,1,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,1,0,1,1,1,0,1,1,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,1,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,1,1,1,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,0,1,1,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,1,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};
/**
 * @brief Рисует пиксельную пипетку на кнопке.
 */
static void paint_draw_pipette_icon(void)
{
    uint8_t row;
    uint8_t col;

    for (row = 0; row < 24; row++) {
        for (col = 0; col < 24; col++) {
            if (pipette_bitmap[row][col]) ClearMassDMA(1, 1, PAINT_PIPETTE_BUTTON_Y + 10 + row + 18, PAINT_PIPETTE_BUTTON_X + 8 + col, 0x7152);
        }
    }
}

/**
 * @brief Рисует кнопку пипетки и пиктограмму пипетки
 */
static void paint_draw_pipette_button(void)
{
    draw_rect(
        PAINT_PIPETTE_BUTTON_HEIGHT,
        PAINT_PIPETTE_BUTTON_WIDTH,
        PAINT_PIPETTE_BUTTON_Y,
        PAINT_PIPETTE_BUTTON_X,
        0xDDDF
    );

    paint_draw_pipette_icon();
}

/**
 * @brief Проверяет нажатие кнопки пипетки.
 */
static int paint_is_pipette_button_pressed(int x, int y)
{
    if (x < PAINT_PIPETTE_BUTTON_X) return 0;
    if (x >= PAINT_PIPETTE_BUTTON_X + PAINT_PIPETTE_BUTTON_WIDTH) return 0;
    if (y < PAINT_PIPETTE_BUTTON_Y) return 0;
    if (y >= PAINT_PIPETTE_BUTTON_Y + PAINT_PIPETTE_BUTTON_HEIGHT) return 0;
    return 1;
}

/* 
 * RGB
 */

/**
 * @brief Преобразует RGB888 в RGB565
 */
static uint16_t paint_rgb888_to_rgb565(
    uint8_t red,
    uint8_t green,
    uint8_t blue)
{
    return (uint16_t)(((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3));
}

/**
 * @brief Преобразует RGB565 в RGB888
 */
static void paint_rgb565_to_rgb888( uint16_t color, uint8_t *red, uint8_t *green, uint8_t *blue)
{
    *red = (uint8_t)(((color >> 11) & 0x1F) * 255 / 31);
    *green = (uint8_t)(((color >> 5) & 0x3F) * 255 / 63);
    *blue = (uint8_t)((color & 0x1F) * 255 / 31);
}

/**
 * @brief Устанавливает текущий RGB-цвет кисти.
 */
static void paint_apply_rgb_color(void)
{
    uint16_t color;
    color = paint_rgb888_to_rgb565(
        paint_rgb_red,
        paint_rgb_green,
        paint_rgb_blue
    );
    touch_set_color(color);
}

/**
 * @brief Рисует кнопку выбора RGB-цвета.
 *
 * На кнопке отображается пиктограмма палитры
 * из разноцветных пиксельных кружков.
 *
 * В нижней части кнопки отображается
 * текущий выбранный RGB-цвет.
 */
static void paint_draw_rgb_button(void)
{
    uint16_t color;
    color = paint_rgb888_to_rgb565( paint_rgb_red, paint_rgb_green, paint_rgb_blue);
    // Фон кнопки
    draw_rect(PAINT_RGB_BUTTON_HEIGHT, PAINT_RGB_BUTTON_WIDTH, PAINT_RGB_BUTTON_Y, PAINT_RGB_BUTTON_X, 0x7BEF);

    // Красный кружок
    draw_rect(4, 4, PAINT_RGB_BUTTON_Y + 8, PAINT_RGB_BUTTON_X + 8, 0xF800);
    draw_rect(6, 6, PAINT_RGB_BUTTON_Y + 12, PAINT_RGB_BUTTON_X + 7, 0xF800);
    draw_rect(4, 4, PAINT_RGB_BUTTON_Y + 18, PAINT_RGB_BUTTON_X + 8, 0xF800);
    // Оранжевый кружок
    draw_rect(4, 4, PAINT_RGB_BUTTON_Y + 8, PAINT_RGB_BUTTON_X + 18, 0xFD20);
    draw_rect(6, 6, PAINT_RGB_BUTTON_Y + 12, PAINT_RGB_BUTTON_X + 17, 0xFD20);
    draw_rect(4, 4, PAINT_RGB_BUTTON_Y + 18, PAINT_RGB_BUTTON_X + 18, 0xFD20);
    // Жёлтый кружок
    draw_rect(4, 4, PAINT_RGB_BUTTON_Y + 8, PAINT_RGB_BUTTON_X + 28, 0xFFE0);
    draw_rect(6, 6, PAINT_RGB_BUTTON_Y + 12, PAINT_RGB_BUTTON_X + 27, 0xFFE0);
    draw_rect(4, 4, PAINT_RGB_BUTTON_Y + 18, PAINT_RGB_BUTTON_X + 28, 0xFFE0);
    // Зелёный кружок
    draw_rect(4, 4, PAINT_RGB_BUTTON_Y + 26, PAINT_RGB_BUTTON_X + 8, 0x07E0);
    draw_rect(6, 6, PAINT_RGB_BUTTON_Y + 30, PAINT_RGB_BUTTON_X + 7, 0x07E0);
    draw_rect(4, 4, PAINT_RGB_BUTTON_Y + 36, PAINT_RGB_BUTTON_X + 8, 0x07E0);
    // Голубой кружок
    draw_rect(4, 4, PAINT_RGB_BUTTON_Y + 26, PAINT_RGB_BUTTON_X + 18, 0x07FF);
    draw_rect(6, 6, PAINT_RGB_BUTTON_Y + 30, PAINT_RGB_BUTTON_X + 17, 0x07FF);
    draw_rect(4, 4, PAINT_RGB_BUTTON_Y + 36, PAINT_RGB_BUTTON_X + 18, 0x07FF);
    // Синий кружок
    draw_rect(4, 4, PAINT_RGB_BUTTON_Y + 26, PAINT_RGB_BUTTON_X + 28, 0x001F);
    draw_rect(6, 6, PAINT_RGB_BUTTON_Y + 30, PAINT_RGB_BUTTON_X + 27, 0x001F);
    draw_rect(4, 4, PAINT_RGB_BUTTON_Y + 36, PAINT_RGB_BUTTON_X + 28, 0x001F);

    // Текущий выбранный цвет
    draw_rect(10 + 15, 24, PAINT_RGB_BUTTON_Y + 64 - 15, PAINT_RGB_BUTTON_X + 8, color);
}

/**
 * @brief Рисует кнопку очистки
 */
static void paint_draw_clear_button(void)
{
    draw_rect (80, 40, 400, 0, 0x8FF0);
    drawText  (22, 410, "CLEAR", 0x2240, 0x8FF0, 2);
}

/**
 * @brief Проверяет нажатие кнопки очистки.
 */
static int paint_is_clear_button_pressed(int x, int y)
{
    if (y < 400 || y >= 480) return 0;
    if (x < 0 || x >= 40) return 0;
    return 1;
}

/**
 * @brief Рисует кнопку смены размера кисти
 */
/**
 * @brief Рисует кнопку выбора размера кисти
 *
 * На кнопке отображаются четыре вертикальные линии
 * различной толщины — от самой тонкой до самой толстой.
 */
static void paint_draw_brush_button(void)
{
    // Фон кнопки
    draw_rect(80, 40, 0, 0, 0xEDEE);
    // Тонкая линия
    draw_rect(32 + 40, 1, 24 - 20, 5, 0x9899);
    // Линия средней толщины
    draw_rect(32 + 40, 3, 24 - 20, 12, 0x9899);
    // Толстая линия
    draw_rect(32 + 40, 6, 24 - 20, 20, 0x9899);
    // Самая толстая линия
    draw_rect(32 + 40, 8, 24 - 20, 29, 0x9899);
}

/**
 * @brief Проверяет нажатие кнопки выбора размера кисти
 */
static int paint_is_brush_button_pressed(int x, int y)
{
    if (x < 0 || x >= 40) return 0;
    if (y < 0 || y >= 80) return 0;
    return 1;
}

static const uint8_t brush_bitmap[24][24] =
{
    
    {0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,1,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,1,1,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,1,0,0,1,1,1,1,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,1,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,1,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0},
};
    
/**
 * @brief Рисует пиксельный силуэт кисти на кнопке выбора типа кисти.
 *
 * Каждый элемент bitmap со значением 1 выводится
 * одним чёрным пикселем.
 */
static void paint_draw_brush_icon_2(void)
{
    uint8_t row;
    uint8_t col;

    for (row = 0; row < 24; row++) {

        for (col = 0; col < 24; col++) {

            if (brush_bitmap[row][col]) {

                ClearMassDMA(1, 1, PAINT_BRUSH_BUTTON_2_Y + 10 + row + 18, PAINT_BRUSH_BUTTON_2_X + 8 + col, 0x6152);
            }
        }
    }
}
/**
 * @brief Рисует кнопку смены типа кисти и пиктограмму кисти
 */
static void paint_draw_brush_button_2(void)
{
    draw_rect(
        PAINT_BRUSH_BUTTON_2_HEIGHT,
        PAINT_BRUSH_BUTTON_2_WIDTH,
        PAINT_BRUSH_BUTTON_2_Y,
        PAINT_BRUSH_BUTTON_2_X,
        0xDCCF
    );

    paint_draw_brush_icon_2();
}
/**
 * @brief Проверяет нажатие кнопки смена типа кисти
 */
static int paint_is_brush_button_2_pressed(int x, int y)
{
    if (x < PAINT_BRUSH_BUTTON_2_X) return 0;
    if (x >= PAINT_BRUSH_BUTTON_2_X + PAINT_BRUSH_BUTTON_2_WIDTH) return 0;
    if (y < PAINT_BRUSH_BUTTON_2_Y) return 0;
    if (y >= PAINT_BRUSH_BUTTON_2_Y + PAINT_BRUSH_BUTTON_2_HEIGHT)return 0;
    return 1;
}

/* 
 * RGB-КНОПКА
 */

/**
 * @brief Проверяет нажатие RGB-кнопки.
 */
static int paint_is_rgb_button_pressed(int x, int y)
{
    if (x < PAINT_RGB_BUTTON_X) return 0;
    if (x >= PAINT_RGB_BUTTON_X + PAINT_RGB_BUTTON_WIDTH) return 0;
    if (y < PAINT_RGB_BUTTON_Y) return 0;
    if (y >= PAINT_RGB_BUTTON_Y + PAINT_RGB_BUTTON_HEIGHT) return 0;
    return 1;
}

/**
 * @brief Рисует меню выбора размера кисти.
 *
 * В каждом пункте меню отображается вертикальная линия,
 * соответствующая толщине кисти:
 * - первый пункт  — самая тонкая
 * - второй пункт  — тонкая
 * - третий пункт  — толстая
 * - четвёртый пункт — самая толстая
 */
static void paint_draw_brush_menu(void)
{
    uint16_t line_y;
    uint16_t button_x;

    /* Рисуем четыре кнопки меню. */
    draw_rect(
        PAINT_BRUSH_MENU_WIDTH,
        PAINT_BRUSH_MENU_ITEM_HEIGHT,
        PAINT_BRUSH_MENU_Y,
        PAINT_BRUSH_MENU_X,
        0xBAAB
    );

    draw_rect(
        PAINT_BRUSH_MENU_WIDTH,
        PAINT_BRUSH_MENU_ITEM_HEIGHT,
        PAINT_BRUSH_MENU_Y,
        PAINT_BRUSH_MENU_X + PAINT_BRUSH_MENU_ITEM_HEIGHT,
        0xBBBB
    );

    draw_rect(
        PAINT_BRUSH_MENU_WIDTH,
        PAINT_BRUSH_MENU_ITEM_HEIGHT,
        PAINT_BRUSH_MENU_Y,
        PAINT_BRUSH_MENU_X + 2 * PAINT_BRUSH_MENU_ITEM_HEIGHT,
        0xCACA
    );

    draw_rect(
        PAINT_BRUSH_MENU_WIDTH,
        PAINT_BRUSH_MENU_ITEM_HEIGHT,
        PAINT_BRUSH_MENU_Y,
        PAINT_BRUSH_MENU_X + 3 * PAINT_BRUSH_MENU_ITEM_HEIGHT,
        0xAD46
    );

    line_y = PAINT_BRUSH_MENU_Y + 8;
    button_x = PAINT_BRUSH_MENU_X;

    draw_rect(PAINT_BRUSH_MENU_WIDTH - 16, 2, line_y, button_x + (PAINT_BRUSH_MENU_ITEM_HEIGHT - 2) / 2, 0x8888);
    button_x = PAINT_BRUSH_MENU_X + PAINT_BRUSH_MENU_ITEM_HEIGHT;
    draw_rect(PAINT_BRUSH_MENU_WIDTH - 16, 4, line_y, button_x + (PAINT_BRUSH_MENU_ITEM_HEIGHT - 4) / 2, 0x8888);
    button_x = PAINT_BRUSH_MENU_X + 2 * PAINT_BRUSH_MENU_ITEM_HEIGHT;
    draw_rect(PAINT_BRUSH_MENU_WIDTH - 16, 8, line_y,button_x + (PAINT_BRUSH_MENU_ITEM_HEIGHT - 8) / 2, 0x8888);
    button_x = PAINT_BRUSH_MENU_X + 3 * PAINT_BRUSH_MENU_ITEM_HEIGHT;
    draw_rect(PAINT_BRUSH_MENU_WIDTH - 16, 12, line_y, button_x + (PAINT_BRUSH_MENU_ITEM_HEIGHT - 12) / 2, 0x8888);
}

/**
 * @brief Рисует заполненный квадрат
 *
 * @param x      Левая координата квадрата
 * @param y      Верхняя координата квадрата
 * @param size   Размер стороны квадрата
 * @param color  Цвет квадрата
 */
static void paint_draw_brush_shape_square(uint16_t x, uint16_t y, uint16_t size, uint16_t color)
{
    draw_rect(size, size, y, x, color);
}

/**
 * @brief Рисует заполненный круг
 *
 * Круг формируется из отдельных горизонтальных строк пикселей
 *
 * @param center_x  Координата центра по X
 * @param center_y  Координата центра по Y
 * @param radius    Радиус круга
 * @param color     Цвет круга
 */
static void paint_draw_brush_shape_circle(uint16_t center_x, uint16_t center_y, uint16_t radius, uint16_t color)
{
    int16_t dx;
    int16_t dy;

    for (dy = -(int16_t)radius; dy <= (int16_t)radius; dy++) {
        for (dx = -(int16_t)radius; dx <= (int16_t)radius; dx++) {

            if ((dx * dx + dy * dy) <= (radius * radius)) {
                ClearMassDMA(1, 1, center_y + dy, center_x + dx, color);
            }
        }
    }
}

/**
 * @brief Рисует заполненный эллипс
 *
 * @param center_x  Координата центра по X
 * @param center_y  Координата центра по Y
 * @param radius_x  Полуось эллипса по X
 * @param radius_y  Полуось эллипса по Y
 * @param color     Цвет эллипса
 */
static void paint_draw_brush_shape_ellipse(uint16_t center_x, uint16_t center_y, uint16_t radius_x, uint16_t radius_y, uint16_t color)
{
    int16_t dx;
    int16_t dy;
    int32_t value;

    for (dy = -(int16_t)radius_y; dy <= (int16_t)radius_y; dy++) {
        for (dx = -(int16_t)radius_x; dx <= (int16_t)radius_x; dx++) {

            value = (int32_t)dx * dx * radius_y * radius_y + (int32_t)dy * dy * radius_x * radius_x;

            if (value <= (int32_t)radius_x * radius_x * radius_y * radius_y) {
                ClearMassDMA( 1, 1, center_y + dy, center_x + dx, color);
            }
        }
    }
}

/**
 * @brief Рисует пиктограмму кисти типа "спрей"
 *
 * Точки располагаются вокруг центра с различной плотностью, чтобы визуально напоминать распыление
 *
 * @param center_x  Координата центра по X
 * @param center_y  Координата центра по Y
 * @param color     Цвет точек
 */
static void paint_draw_brush_shape_spray(uint16_t center_x, uint16_t center_y, uint16_t color)
{
    static const int8_t spray_points[][2] =
    {
        {-9, -6}, {-6, -8}, {-2, -9}, { 3, -8}, { 7, -6}, {-11, -3}, {-7, -3}, {-3, -4}, { 2, -3}, { 6, -2}, {10, -3}, {-9,  1}, {-5,  0}, { 0,  1}, { 5,  0}, { 9,  2}, {-7,  5}, {-3,  4}, { 2,  5}, { 7,  5}, {-4,  8}, { 1,  8}, { 5,  7}
    };

    uint8_t i;
    uint8_t count;

    count = sizeof(spray_points) / sizeof(spray_points[0]);

    for (i = 0; i < count; i++) {
        ClearMassDMA(2, 2, center_y + spray_points[i][1], center_x + spray_points[i][0], color);
    }
}

/**
 * @brief Рисует меню выбора формы кисти
 * В меню представлены четыре типа кисти: квадрат, круг, эллипс, спрей
 */
static void paint_draw_brush_menu_2(void)
{
    uint16_t color;
    uint16_t center_y;
    uint16_t center_x;

    color = 0x8888;

    // Рисуем четыре кнопки меню
    draw_rect(
        PAINT_BRUSH_MENU_WIDTH,
        PAINT_BRUSH_MENU_ITEM_HEIGHT,
        PAINT_BRUSH_MENU_Y,
        PAINT_BRUSH_MENU_X,
        0xBAAB
    );

    draw_rect(
        PAINT_BRUSH_MENU_WIDTH,
        PAINT_BRUSH_MENU_ITEM_HEIGHT,
        PAINT_BRUSH_MENU_Y,
        PAINT_BRUSH_MENU_X + PAINT_BRUSH_MENU_ITEM_HEIGHT,
        0xBBBB
    );

    draw_rect(
        PAINT_BRUSH_MENU_WIDTH,
        PAINT_BRUSH_MENU_ITEM_HEIGHT,
        PAINT_BRUSH_MENU_Y,
        PAINT_BRUSH_MENU_X + 2 * PAINT_BRUSH_MENU_ITEM_HEIGHT,
        0xCACA
    );

    draw_rect(
        PAINT_BRUSH_MENU_WIDTH,
        PAINT_BRUSH_MENU_ITEM_HEIGHT,
        PAINT_BRUSH_MENU_Y,
        PAINT_BRUSH_MENU_X + 3 * PAINT_BRUSH_MENU_ITEM_HEIGHT,
        0xAD46
    );

    // Центр кнопок
    center_y = PAINT_BRUSH_MENU_Y + PAINT_BRUSH_MENU_WIDTH / 2;
    // Квадрат
    center_x = PAINT_BRUSH_MENU_X + PAINT_BRUSH_MENU_ITEM_HEIGHT / 2;
    paint_draw_brush_shape_square(center_x - 8, center_y - 8, 16, color);
    // Круг
    center_x = PAINT_BRUSH_MENU_X + PAINT_BRUSH_MENU_ITEM_HEIGHT + PAINT_BRUSH_MENU_ITEM_HEIGHT / 2;
    paint_draw_brush_shape_circle(center_x, center_y, 8, color);
    // Эллипс
    center_x = PAINT_BRUSH_MENU_X + 2 * PAINT_BRUSH_MENU_ITEM_HEIGHT + PAINT_BRUSH_MENU_ITEM_HEIGHT / 2;
    paint_draw_brush_shape_ellipse( center_x, center_y, 10, 6, color);
    // Спрей
    center_x = PAINT_BRUSH_MENU_X + 3 * PAINT_BRUSH_MENU_ITEM_HEIGHT + PAINT_BRUSH_MENU_ITEM_HEIGHT / 2;
    paint_draw_brush_shape_spray(center_x, center_y, color);
}

/**
 * @brief Рисует рамку выбранного пункта меню
 * @param y Координата пункта меню
 */
static void paint_draw_brush_menu_border(int y)
{
    draw_rect(
        PAINT_BRUSH_MENU_WIDTH,
        PAINT_BRUSH_MENU_BORDER,
        PAINT_BRUSH_MENU_Y,
        y,
        PAINT_COLOR_WHITE
    );

    draw_rect(
        PAINT_BRUSH_MENU_WIDTH,
        PAINT_BRUSH_MENU_BORDER,
        PAINT_BRUSH_MENU_Y,
        y + PAINT_BRUSH_MENU_ITEM_HEIGHT - PAINT_BRUSH_MENU_BORDER,
        PAINT_COLOR_WHITE
    );

    draw_rect(
        PAINT_BRUSH_MENU_BORDER,
        PAINT_BRUSH_MENU_ITEM_HEIGHT,
        PAINT_BRUSH_MENU_Y,
        y,
        PAINT_COLOR_WHITE
    );

    draw_rect(
        PAINT_BRUSH_MENU_BORDER,
        PAINT_BRUSH_MENU_ITEM_HEIGHT,
        PAINT_BRUSH_MENU_WIDTH - PAINT_BRUSH_MENU_BORDER,
        y,
        PAINT_COLOR_WHITE
    );
}

/**
 * @brief Возвращает размер кисти по координатам меню
 */
static int paint_get_brush_menu_size(int y, int x)
{
    int item;
    if (!paint_brush_menu_active) return 0;
    if (x < 0 || x >= 80) return 0;
    if (y < 40 || y >= 40 + 40 * 4) return 0;
    item = (y - 40) / 40;
    if (item == 0) return BRUSH_SIZE_SMALL;
    if (item == 1) return BRUSH_SIZE_MEDIUM;
    if (item == 2) return BRUSH_SIZE_LARGE;
    if (item == 3) return BRUSH_SIZE_XLARGE;
    return 0;
}

/**
 * @brief Возвращает форму кисти по координатам меню
 */
static int paint_get_brush_menu_2_shape(int y, int x)
{
    int item;
    if (!paint_brush_menu_2_active) return -1;
    if (x < 0 || x >= 80) return -1;
    if (y < 40 || y >= 40 + 40 * 4)  return -1;
    item = (y - 40) / 40;
    if (item == 0) return BRUSH_SHAPE_SQUARE;
    if (item == 1) return BRUSH_SHAPE_CIRCLE;
    if (item == 2) return BRUSH_SHAPE_ELLIPSE;
    if (item == 3) return BRUSH_SHAPE_SPRAY;
    return -1;
}

/**
 * @brief Возвращает координату выбранного размера кисти
 */
static int paint_get_brush_menu_item_y(uint8_t brush_size)
{
    if (brush_size == BRUSH_SIZE_SMALL)  return 40;
    if (brush_size == BRUSH_SIZE_MEDIUM) return 80;
    if (brush_size == BRUSH_SIZE_LARGE)  return 120;
    if (brush_size == BRUSH_SIZE_XLARGE) return 160;
    return -1;
}

/**
 * @brief Возвращает координату выбранной формы кисти.
 */
static int paint_get_brush_menu_2_shape_y(BrushShape shape)
{
    if (shape == BRUSH_SHAPE_SQUARE)  return 40;
    if (shape == BRUSH_SHAPE_CIRCLE)  return 80;
    if (shape == BRUSH_SHAPE_ELLIPSE) return 120;
    if (shape == BRUSH_SHAPE_SPRAY)   return 160;
    return -1;
}

/**
 * @brief Рисует рамку выбранного размера кисти
 */
static void paint_draw_selected_brush_size(void)
{
    int item_y;
    uint8_t brush_size;
    brush_size = touch_get_brush_size();
    item_y = paint_get_brush_menu_item_y(brush_size);
    if (item_y >= 0) paint_draw_brush_menu_border(item_y);
}

/**
 * @brief Рисует рамку выбранной формы кисти
 */
static void paint_draw_selected_brush_shape(void)
{
    int item_y;
    BrushShape shape;
    shape = touch_get_brush_shape();
    item_y = paint_get_brush_menu_2_shape_y(shape);
    if (item_y >= 0) paint_draw_brush_menu_border(item_y);
}

/*
 * RGB-МЕНЮ
*/

/**
 * @brief Рисует RGB-меню
 */
static void paint_draw_rgb_menu(void)
{
    uint16_t current_color;

    current_color = paint_rgb888_to_rgb565(
        paint_rgb_red,
        paint_rgb_green,
        paint_rgb_blue
    );

    draw_rect(
        PAINT_RGB_MENU_HEIGHT,
        PAINT_RGB_MENU_WIDTH,
        PAINT_RGB_MENU_Y,
        PAINT_RGB_MENU_X,
        PAINT_RGB_DARK
    );

    paint_draw_rgb_slider(
        PAINT_RGB_RED_X,
        paint_rgb_red,
        PAINT_RGB_RED_COLOR
    );

    paint_draw_rgb_slider(
        PAINT_RGB_GREEN_X,
        paint_rgb_green,
        PAINT_RGB_GREEN_COLOR
    );

    paint_draw_rgb_slider(
        PAINT_RGB_BLUE_X,
        paint_rgb_blue,
        PAINT_RGB_BLUE_COLOR
    );

    draw_rect(
        PAINT_RGB_PREVIEW_HEIGHT,
        PAINT_RGB_PREVIEW_WIDTH,
        PAINT_RGB_PREVIEW_Y,
        PAINT_RGB_PREVIEW_X,
        current_color
    );
}

/**
 * @brief Определяет RGB-канал по координате X.
 */
static int paint_get_rgb_column(int x)
{
    if (x >= PAINT_RGB_RED_X &&  x < PAINT_RGB_RED_X + PAINT_RGB_COLUMN_WIDTH)    return 0;
    if (x >= PAINT_RGB_GREEN_X && x < PAINT_RGB_GREEN_X + PAINT_RGB_COLUMN_WIDTH) return 1;
    if (x >= PAINT_RGB_BLUE_X && x < PAINT_RGB_BLUE_X + PAINT_RGB_COLUMN_WIDTH) {

        return 2;
    }

    return -1;
}


/**
 * @brief Преобразует координату Y в значение RGB.
 */
static uint8_t paint_get_rgb_value_from_y(int y)
{
    int value;
    if (y < PAINT_RGB_BAR_Y) y = PAINT_RGB_BAR_Y;
    if (y >= PAINT_RGB_BAR_Y + PAINT_RGB_BAR_HEIGHT) y = PAINT_RGB_BAR_Y + PAINT_RGB_BAR_HEIGHT - 1;

    value = (PAINT_RGB_BAR_Y + PAINT_RGB_BAR_HEIGHT - 1 - y) * 255 / (PAINT_RGB_BAR_HEIGHT - 1);

    if (value < 0) value = 0;
    if (value > 255) value = 255;

    return (uint8_t)value;
}


/**
 * @brief Проверяет касание внутри RGB-меню
 */
static int paint_is_rgb_menu_pressed(int x, int y)
{
    if (!paint_rgb_menu_active) return 0;
    if (x < PAINT_RGB_MENU_X) return 0;
    if (x >= PAINT_RGB_MENU_X + PAINT_RGB_MENU_WIDTH) return 0;
    if (y < PAINT_RGB_MENU_Y) return 0;
    if (y >= PAINT_RGB_MENU_Y + PAINT_RGB_MENU_HEIGHT) return 0;
    return 1;
}

/**
 * @brief Рисует выбранный RGB-цвет в окне предпросмотра.
 */
static void paint_draw_selected_rgb_color(void)
{
    uint16_t color;

    color = paint_rgb888_to_rgb565(
        paint_rgb_red,
        paint_rgb_green,
        paint_rgb_blue
    );

    draw_rect(
        PAINT_RGB_PREVIEW_HEIGHT,
        PAINT_RGB_PREVIEW_WIDTH,
        PAINT_RGB_PREVIEW_Y,
        PAINT_RGB_PREVIEW_X,
        color
    );
}

/**
 * @brief Изменяет значение выбранного RGB-канала
 */
static void paint_update_rgb_slider(int x, int y)
{
    int column;
    uint8_t value;
    column = paint_get_rgb_column(x);
    if (column < 0) return;
    value = paint_get_rgb_value_from_y(y);
    if (column == 0) paint_rgb_red = value;
    else if (column == 1) paint_rgb_green = value;
    else paint_rgb_blue = value;
    paint_apply_rgb_color();
    // Рисуем только изменившийся ползунок
    paint_draw_rgb_slider_by_column(column);
    // И только предпросмотр цвета
    paint_draw_selected_rgb_color();
}

/*
 * ЗАКРЫТИЕ МЕНЮ
 */

/**
 * @brief Закрывает меню размера кисти
 */
static void paint_close_brush_menu(void)
{
    paint_brush_menu_active = 0;

    ClearMassDMA(80, PAINT_BRUSH_MENU_HEIGHT, 0, 40, PAINT_COLOR_WHITE);
    paint_history_redraw_area(0, 0, 160, 80);
    paint_draw_rgb_button();
}

/**
 * @brief Закрывает меню формы кисти
 */
static void paint_close_brush_menu_2(void)
{
    paint_brush_menu_2_active = 0;
    ClearMassDMA(80, PAINT_BRUSH_MENU_HEIGHT, 0, 40, PAINT_COLOR_WHITE);
    paint_history_redraw_area(0, 0, 160, 80);
    paint_draw_rgb_button();
}

/**
 * @brief Закрывает RGB-меню
 */
static void paint_close_rgb_menu(void)
{
    paint_rgb_menu_active = 0;
    ClearMassDMA(
        PAINT_RGB_MENU_HEIGHT,
        PAINT_RGB_MENU_WIDTH,
        PAINT_RGB_MENU_Y,
        PAINT_RGB_MENU_X,
        PAINT_COLOR_WHITE
    );
    paint_history_redraw_area(
        PAINT_RGB_MENU_X,
        PAINT_RGB_MENU_Y,
        PAINT_RGB_MENU_WIDTH,
        PAINT_RGB_MENU_HEIGHT
    );
    paint_draw_rgb_button();
}

/* 
 * TOUCH
 */

/**
 * @brief Читает и преобразует координаты касания.
 */
static int paint_read_touch(int *x, int *y)
{
    int raw_x;
    int raw_y;

    if (!touch_read_averaged(&raw_x, &raw_y)) return 0;
    if (!touch_raw_to_screen(raw_x, raw_y, x, y)) return 0;
    return 1;
}

/*
 * ИНИЦИАЛИЗАЦИЯ
 */

/**
 * @brief Инициализирует графический редактор Paint
 */
void paint_init(void)
{
    uint16_t color;

    ClearMassDMA(480, 320, 0, 0, PAINT_COLOR_WHITE);

    paint_draw_brush_button();
    paint_draw_brush_button_2();
    color = touch_get_color();
    paint_rgb565_to_rgb888(
        color,
        &paint_rgb_red,
        &paint_rgb_green,
        &paint_rgb_blue
    );

    paint_draw_rgb_button();
    paint_draw_pipette_button();
    paint_draw_clear_button();

    touch_set_brush_size(BRUSH_SIZE_DEFAULT);

    touch_set_color(
        paint_rgb888_to_rgb565(
            paint_rgb_red,
            paint_rgb_green,
            paint_rgb_blue
        )
    );

    paint_history_init();

    touch_set_brush_shape(BRUSH_SHAPE_SQUARE);

    paint_draw_undo_redo_buttons();

    paint_brush_menu_active = 0;
    paint_brush_menu_2_active = 0;
    paint_brush_button_active = 0;
    paint_clear_button_active = 0;
    paint_touch_started = PAINT_TOUCH_NONE;
    paint_rgb_menu_active = 0;
    paint_rgb_selection_active = 0;
    paint_touch_action_done = 0;
}

/*
 * ОСНОВНОЙ ЦИКЛ
 */

/**
 * @brief Обрабатывает один цикл работы Paint
 */
void paint_process(void)
{
    int x;
    int y;
    int brush_size;

    if (!paint_read_touch(&x, &y)) {

        paint_touch_action_done = 0;

        paint_clear_button_active = 0;
        paint_brush_button_active = 0;
        paint_touch_started = PAINT_TOUCH_NONE;
        paint_rgb_selection_active = 0;

        paint_pipette_last_x = -1;
        paint_pipette_last_y = -1;

        paint_rgb_last_x = -1;
        paint_rgb_last_y = -1;

        touch_reset_stroke();

        return;
    }

    /*
     * Определяем действие только в момент
     * начала нового касания.
     */
    if (paint_touch_started == PAINT_TOUCH_NONE) {

        if (paint_is_undo_button_pressed(x, y)) {
            paint_touch_started = PAINT_TOUCH_UNDO;
        }
        else if (paint_is_redo_button_pressed(x, y)) {
            paint_touch_started = PAINT_TOUCH_REDO;
        }
        else if (paint_brush_menu_2_active) {
            if (paint_get_brush_menu_2_shape(x, y) >= 0) {
                paint_touch_started = PAINT_TOUCH_MENU_2;
            }
        }
        else if (paint_brush_menu_active) {
            if (paint_get_brush_menu_size(x, y) > 0) {
                paint_touch_started = PAINT_TOUCH_MENU;
            }
        }
        else if (paint_rgb_menu_active) {
            if (paint_is_rgb_menu_pressed(x, y)) {
                paint_touch_started = PAINT_TOUCH_RGB;
            }
        }
        else if (paint_is_clear_button_pressed(x, y)) {
            paint_touch_started = PAINT_TOUCH_CLEAR;
        }
        else if (paint_is_brush_button_pressed(x, y)) {
            paint_touch_started = PAINT_TOUCH_BRUSH;
        }
        else if (paint_is_brush_button_2_pressed(x, y)) {
            paint_touch_started = PAINT_TOUCH_BRUSH_2;
        }
        else if (paint_is_rgb_button_pressed(x, y)) {
            paint_touch_started = PAINT_TOUCH_RGB;
        }
        else if (paint_is_pipette_button_pressed(x, y)) {
            paint_touch_started = PAINT_TOUCH_PIPETTE;
        }
        else {
            paint_touch_started = PAINT_TOUCH_CANVAS;
        }
    }

    // Undo - только один раз за одно касание
    if (paint_touch_started == PAINT_TOUCH_UNDO) {

        if (!paint_touch_action_done) {
            touch_reset_stroke();
            paint_history_undo();
            paint_touch_action_done = 1;
        }

        return;
    }

    // Redo - только один раз за одно касание
    if (paint_touch_started == PAINT_TOUCH_REDO) {

        if (!paint_touch_action_done) {
            touch_reset_stroke();
            paint_history_redo();
            paint_touch_action_done = 1;
        }

        return;
    }

    // RGB-меню
    if (paint_rgb_menu_active) {
        if (paint_touch_started == PAINT_TOUCH_RGB) {
            if (paint_is_rgb_menu_pressed(x, y)) {
                // Перерисовываем RGB-меню только если координата тача изменилась.
                if (x != paint_rgb_last_x || y != paint_rgb_last_y) {
                    paint_rgb_last_x = x;
                    paint_rgb_last_y = y;
                    paint_update_rgb_slider(x, y);
                }
                paint_rgb_selection_active = 1;
                touch_reset_stroke();
                return;
            }
        }
        if (paint_touch_started == PAINT_TOUCH_NONE) {
            paint_close_rgb_menu();
            paint_rgb_selection_active = 0;
            paint_rgb_last_x = -1;
            paint_rgb_last_y = -1;
            touch_reset_stroke();
            return;
        }
    }

    // Пипетка
    if (paint_touch_started == PAINT_TOUCH_PIPETTE) {

        // Обновляем цвет только если координата изменилась. Пока палец стоит на месте, экран повторно не перерисовываем
        if (x != paint_pipette_last_x || y != paint_pipette_last_y) {
            uint16_t picked_color;
            paint_pipette_last_x = x;
            paint_pipette_last_y = y;
            if (paint_history_pick_color(x, y, &picked_color)) {
                touch_set_color(picked_color);
                paint_rgb565_to_rgb888(
                    picked_color,
                    &paint_rgb_red,
                    &paint_rgb_green,
                    &paint_rgb_blue
                );
                paint_draw_rgb_button();
            }
        }
        touch_reset_stroke();
        return;
    }

    // Меню смены типа кисти
    if (paint_brush_menu_2_active) {
        if (paint_touch_started == PAINT_TOUCH_MENU_2) {
            int brush_shape;
            brush_shape = paint_get_brush_menu_2_shape(x, y);
            if (brush_shape >= 0) {
                touch_set_brush_shape((BrushShape)brush_shape);
                paint_close_brush_menu_2();
                paint_brush_button_active = 0;
                touch_reset_stroke();
                return;
            }
        }
        if (paint_touch_started == PAINT_TOUCH_NONE) {
            paint_close_brush_menu_2();
            paint_brush_button_active = 0;
            touch_reset_stroke();
            return;
        }
    }

    // Меню смены размера кисти
    if (paint_brush_menu_active) {
        if (paint_touch_started == PAINT_TOUCH_MENU) {
            brush_size = paint_get_brush_menu_size(x, y);
            if (brush_size > 0) {
                touch_set_brush_size((uint8_t)brush_size);
                paint_close_brush_menu();
                paint_brush_button_active = 0;
                touch_reset_stroke();
                return;
            }
        }
        if (paint_touch_started == PAINT_TOUCH_NONE) {
            paint_close_brush_menu();
            paint_brush_button_active = 0;
            touch_reset_stroke();
            return;
        }
    }

    //Очистка холста
    if (paint_touch_started == PAINT_TOUCH_CLEAR) {
        if (!paint_clear_button_active) {
            paint_brush_menu_active = 0;
            paint_brush_menu_2_active = 0;
            paint_rgb_menu_active = 0;
            paint_clear_canvas();
            paint_clear_button_active = 1;
        }
        touch_reset_stroke();
        return;
    }

    // Кнопка меню смены типа кисти
    if (paint_touch_started == PAINT_TOUCH_BRUSH_2) {
        if (!paint_brush_button_active) {
            paint_brush_menu_active = 0;
            paint_rgb_menu_active = 0;
            paint_draw_brush_menu_2();
            paint_draw_selected_brush_shape();
            paint_brush_menu_2_active = 1;
            paint_brush_button_active = 1;
        }
        touch_reset_stroke();
        return;
    }

    // Кнопка меню смены размера кисти
    if (paint_touch_started == PAINT_TOUCH_BRUSH) {
        if (!paint_brush_button_active) {

            paint_brush_menu_2_active = 0;
            paint_rgb_menu_active = 0;

            paint_draw_brush_menu();
            paint_draw_selected_brush_size();

            paint_brush_menu_active = 1;
            paint_brush_button_active = 1;
        }

        touch_reset_stroke();
        return;
    }

    // RGB-кнопка
    if (paint_touch_started == PAINT_TOUCH_RGB) {
        if (!paint_rgb_menu_active) {

            paint_brush_menu_active = 0;
            paint_brush_menu_2_active = 0;
            paint_brush_button_active = 0;

            paint_draw_rgb_menu();
            paint_rgb_menu_active = 1;
        }
        touch_reset_stroke();
        return;
    }

    // Рисование по холсту
    if (paint_touch_started == PAINT_TOUCH_CANVAS) {

        paint_clear_button_active = 0;
        paint_brush_button_active = 0;

        touch_draw_point(x, y);
        return;
    }
}

/*
 * ОЧИСТКА ХОЛСТА
 */

/**
 * @brief Очищает холст и восстанавливает интерфейс
 */
void paint_clear_canvas(void)
{
    paint_history_clear();
    ClearMassDMA(480, 280, 0, 40, PAINT_COLOR_WHITE);

    paint_draw_brush_button();
    paint_draw_brush_button_2();
    paint_draw_rgb_button();
    paint_draw_pipette_button();
    paint_draw_clear_button();
    paint_draw_undo_redo_buttons();

    paint_brush_menu_active = 0;
    paint_brush_menu_2_active = 0;
    paint_rgb_menu_active = 0;
    paint_brush_button_active = 0;
}
