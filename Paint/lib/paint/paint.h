#ifndef PAINT_H
#define PAINT_H

/*
 * РАЗМЕРЫ И ПОЛОЖЕНИЕ ЭЛЕМЕНТОВ ИНТЕРФЕЙСА
 */

/** Кнопка первой кисти. */
#define PAINT_BRUSH_BUTTON_X          0
#define PAINT_BRUSH_BUTTON_Y          0
#define PAINT_BRUSH_BUTTON_WIDTH      40
#define PAINT_BRUSH_BUTTON_HEIGHT     80

/** Кнопка второй кисти. */
#define PAINT_BRUSH_BUTTON_2_X        0
#define PAINT_BRUSH_BUTTON_2_Y        80
#define PAINT_BRUSH_BUTTON_2_WIDTH    40
#define PAINT_BRUSH_BUTTON_2_HEIGHT   80

/** Кнопка пипетки. */
#define PAINT_PIPETTE_BUTTON_X        0
#define PAINT_PIPETTE_BUTTON_Y        160
#define PAINT_PIPETTE_BUTTON_WIDTH    40
#define PAINT_PIPETTE_BUTTON_HEIGHT   80

/** Кнопка RGB. */
#define PAINT_RGB_BUTTON_X            0
#define PAINT_RGB_BUTTON_Y            240
#define PAINT_RGB_BUTTON_WIDTH        40
#define PAINT_RGB_BUTTON_HEIGHT       80

/** Кнопка Redo. */
#define PAINT_REDO_BUTTON_X           0
#define PAINT_REDO_BUTTON_Y           320
#define PAINT_REDO_BUTTON_WIDTH       40
#define PAINT_REDO_BUTTON_HEIGHT      40

/** Кнопка Undo. */
#define PAINT_UNDO_BUTTON_X           0
#define PAINT_UNDO_BUTTON_Y           360
#define PAINT_UNDO_BUTTON_WIDTH       40
#define PAINT_UNDO_BUTTON_HEIGHT      40

/* Размер области нажатия Undo/Redo по фактической пиктограмме */

#define PAINT_UNDO_ICON_X_OFFSET      8
#define PAINT_UNDO_ICON_Y_OFFSET      10
#define PAINT_UNDO_ICON_WIDTH         21
#define PAINT_UNDO_ICON_HEIGHT        18

#define PAINT_REDO_ICON_X_OFFSET      8
#define PAINT_REDO_ICON_Y_OFFSET      10
#define PAINT_REDO_ICON_WIDTH         21
#define PAINT_REDO_ICON_HEIGHT        18

/** Кнопка очистки. */
#define PAINT_CLEAR_BUTTON_X          0
#define PAINT_CLEAR_BUTTON_Y          400
#define PAINT_CLEAR_BUTTON_WIDTH      40
#define PAINT_CLEAR_BUTTON_HEIGHT     80

/*
 * МЕНЮ РАЗМЕРА И ФОРМЫ КИСТИ
 */

#define PAINT_BRUSH_MENU_X            40
#define PAINT_BRUSH_MENU_Y            0
#define PAINT_BRUSH_MENU_WIDTH        80
#define PAINT_BRUSH_MENU_ITEM_HEIGHT  40
#define PAINT_BRUSH_MENU_ITEMS        4

#define PAINT_BRUSH_MENU_HEIGHT \
    (PAINT_BRUSH_MENU_ITEM_HEIGHT * PAINT_BRUSH_MENU_ITEMS)

#define PAINT_BRUSH_MENU_BORDER       2

/*
 * RGB-МЕНЮ
 */

#define PAINT_RGB_MENU_X              40
#define PAINT_RGB_MENU_Y              0
#define PAINT_RGB_MENU_WIDTH          240
#define PAINT_RGB_MENU_HEIGHT         150

#define PAINT_RGB_RED_X              55
#define PAINT_RGB_GREEN_X           125
#define PAINT_RGB_BLUE_X             195

#define PAINT_RGB_COLUMN_WIDTH        22

#define PAINT_RGB_BAR_Y                8
#define PAINT_RGB_BAR_HEIGHT         134
#define PAINT_RGB_BAR_WIDTH           22

#define PAINT_RGB_PREVIEW_X          245
#define PAINT_RGB_PREVIEW_Y            8
#define PAINT_RGB_PREVIEW_WIDTH       28
#define PAINT_RGB_PREVIEW_HEIGHT     134

/*
 * ЦВЕТА
 */

#define PAINT_COLOR_WHITE           0xFFFF
#define PAINT_COLOR_RED             0xF800
#define PAINT_COLOR_GREEN           0x07E0
#define PAINT_COLOR_BLUE            0x001F
#define PAINT_COLOR_YELLOW          0xFFE0
#define PAINT_COLOR_CYAN            0x07FF
#define PAINT_COLOR_MAGENTA         0xF81F

#define PAINT_RGB_RED_COLOR         0xF800
#define PAINT_RGB_GREEN_COLOR       0x07E0
#define PAINT_RGB_BLUE_COLOR        0x001F

#define PAINT_RGB_DARK              0x4208
#define PAINT_RGB_LIGHT             0xFFFF

#define PAINT_UNDO_BUTTON_COLOR     0x9F0F
#define PAINT_REDO_BUTTON_COLOR     0x9F0F

/*
 * СОСТОЯНИЯ TOUCH
 */

#define PAINT_TOUCH_NONE            0
#define PAINT_TOUCH_CANVAS          1
#define PAINT_TOUCH_CLEAR           2
#define PAINT_TOUCH_BRUSH           3
#define PAINT_TOUCH_RGB             4
#define PAINT_TOUCH_MENU            5
#define PAINT_TOUCH_PIPETTE         6
#define PAINT_TOUCH_BRUSH_2         7
#define PAINT_TOUCH_MENU_2          8
#define PAINT_TOUCH_UNDO            9
#define PAINT_TOUCH_REDO            10

/*
 * ПУБЛИЧНЫЙ ИНТЕРФЕЙС
 */

/**
 * @brief Инициализация графического редактора Paint.
 */
void paint_init(void);

/**
 * @brief Обработка одного цикла работы Paint.
 */
void paint_process(void);

/**
 * @brief Очистка области рисования и восстановление интерфейса.
 */
void paint_clear_canvas(void);

#endif /* PAINT_H */