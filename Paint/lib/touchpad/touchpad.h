/**
 * @file    touchpad.h
 * @brief   Драйвер тачпада и рисование по касанию
 */

#ifndef TOUCHPAD_H
#define TOUCHPAD_H

/* Includes ------------------------------------------------------------------*/

#include "lcd.h"
#include "bgpio.h"
#include "bus_spi.h"


/* Defines -------------------------------------------------------------------*/

/** Абсолютное значение числа. */
#define ABS(x) \
    ((x) > 0 ? (x) : -(x))

/** Команды SPI для чтения координат тачпада. */
#define READ_Y                0x90
#define READ_X                0xD0

/** Значения кисти по умолчанию. */
#define TOUCH_DEFAULT_COLOR   0xF00F
#define TOUCH_DEFAULT_SIZE    4

/** Допустимый диапазон размера кисти. */
#define TOUCH_MIN_BRUSH_SIZE  1
#define TOUCH_MAX_BRUSH_SIZE  32

/** Стандартные размеры кисти. */
#define BRUSH_SIZE_SMALL      4
#define BRUSH_SIZE_MEDIUM     8
#define BRUSH_SIZE_LARGE      16
#define BRUSH_SIZE_XLARGE     32

#define BRUSH_SIZE_DEFAULT    BRUSH_SIZE_MEDIUM

/** Калибровка сырых координат тачпада. */
#define TOUCH_MIN_RAW_X       150
#define TOUCH_MIN_RAW_Y       100
#define TOUCH_MAX_RAW         1960

/** Размер экрана. */
#define TOUCH_SCREEN_WIDTH    480
#define TOUCH_SCREEN_HEIGHT   320

/** Область панели инструментов и холста. */
#define TOUCH_TOOLBAR_HEIGHT  40
#define TOUCH_CANVAS_TOP      TOUCH_TOOLBAR_HEIGHT
#define TOUCH_CANVAS_BOTTOM   (TOUCH_SCREEN_HEIGHT - 1)

/** Параметры фильтрации координат. */
#define TOUCH_FILTER_SAMPLES  4
#define TOUCH_FILTER_DELAY    0

/** Подтверждение касания и отпускания. */
#define TOUCH_PRESS_CONFIRM_SAMPLES    3
#define TOUCH_RELEASE_CONFIRM_SAMPLES  2

/** Параметры фильтрации движения. */
#define TOUCH_NOISE_THRESHOLD 2
#define TOUCH_MAX_JUMP        400
#define TOUCH_MAX_JUMP_SQ     (TOUCH_MAX_JUMP * TOUCH_MAX_JUMP)


/* Brush shape ---------------------------------------------------------------*/

/**
 * @brief Форма кисти.
 */
typedef enum
{
    BRUSH_SHAPE_SQUARE = 0,
    BRUSH_SHAPE_CIRCLE,
    BRUSH_SHAPE_ELLIPSE,
    BRUSH_SHAPE_SPRAY
} BrushShape;


/* Brush functions -----------------------------------------------------------*/

/**
 * @brief Установка формы кисти.
 *
 * @param shape Новая форма кисти.
 */
void touch_set_brush_shape(BrushShape shape);

/**
 * @brief Получение текущей формы кисти.
 *
 * @return Текущая форма кисти.
 */
BrushShape touch_get_brush_shape(void);


/* Touchpad control ----------------------------------------------------------*/

/**
 * @brief Выбор тачпада на шине SPI.
 */
void touch_select(void);

/**
 * @brief Снятие выбора тачпада.
 */
void touch_unselect(void);


/* Touchpad reading ----------------------------------------------------------*/

/**
 * @brief Усреднённое чтение пары координат Y/X.
 *
 * @param raw_x Усреднённое сырое значение оси X.
 * @param raw_y Усреднённое сырое значение оси Y.
 *
 * @return 1 — касание есть.
 * @return 0 — нет валидных значений.
 */
int touch_read_averaged(
    int *raw_x,
    int *raw_y);

/**
 * @brief Преобразование сырых координат в координаты экрана.
 *
 * @param raw_x Сырое значение оси X тачпада.
 * @param raw_y Сырое значение оси Y тачпада.
 * @param sx    Горизонтальная координата на экране.
 * @param sy    Вертикальная координата на экране.
 *
 * @return 1 — координаты получены.
 * @return 0 — касание отсутствует.
 */
int touch_raw_to_screen(
    int raw_x,
    int raw_y,
    int *sx,
    int *sy);

/**
 * @brief Ограничение координат границами экрана.
 *
 * @param x Горизонтальная координата.
 * @param y Вертикальная координата.
 */
void touch_clamp_screen(
    int *x,
    int *y);

/**
 * @brief Ограничение координат областью холста.
 *
 * @param x Горизонтальная координата.
 * @param y Вертикальная координата.
 */
void touch_clamp_canvas(
    int *x,
    int *y);


/* Drawing -------------------------------------------------------------------*/

/**
 * @brief Отрисовка точки касания.
 *
 * @param x Горизонтальная координата.
 * @param y Вертикальная координата.
 */
void touch_draw_stamp(
    int x,
    int y);

/**
 * @brief Отрисовка линии между двумя точками.
 *
 * Используется алгоритм Брезенхэма.
 *
 * @param x0 Начальная координата X.
 * @param y0 Начальная координата Y.
 * @param x1 Конечная координата X.
 * @param y1 Конечная координата Y.
 */
void touch_stamp_line(
    int x0,
    int y0,
    int x1,
    int y1);

/**
 * @brief Рисование линий при движении стилуса по экрану.
 */
void touch_draw_lines(void);

/**
 * @brief Отрисовка точки на экране.
 *
 * @param x Горизонтальная координата.
 * @param y Вертикальная координата.
 */
void touch_draw_point(
    int x,
    int y);

/**
 * @brief Сброс состояния текущего штриха.
 */
void touch_reset_stroke(void);


/* Coordinate processing -----------------------------------------------------*/

/**
 * @brief Чтение одной координаты тачпада по SPI.
 *
 * @param command Команда чтения: READ_X или READ_Y.
 *
 * @return 12-битное значение координаты.
 * @return -1 при ошибке.
 */
int read_coordinate(
    unsigned char command);

/**
 * @brief Усреднённое чтение одной оси.
 *
 * @param command Команда чтения: READ_X или READ_Y.
 * @param samples Количество замеров.
 *
 * @return Среднее значение координаты.
 * @return -1 при ошибке.
 */
int read_filtered_coordinate(
    uint8_t command,
    uint8_t samples);

/**
 * @brief Линейное преобразование координаты в другой диапазон.
 *
 * @param value  Исходное значение.
 * @param r1_max Верхняя граница исходного диапазона.
 * @param r1_min Нижняя граница исходного диапазона.
 * @param r2_max Верхняя граница целевого диапазона.
 * @param r2_min Нижняя граница целевого диапазона.
 *
 * @return Преобразованное значение с ограничением
 *         по границам целевого диапазона.
 */
uint16_t transform(
    uint16_t value,
    uint16_t r1_max,
    uint16_t r1_min,
    uint16_t r2_max,
    uint16_t r2_min);


/* Touch processing ----------------------------------------------------------*/

/**
 * @brief Обработка касания и отрисовка маркера на экране.
 */
void process_touchpad(void);


/* Brush settings ------------------------------------------------------------*/

/**
 * @brief Установка цвета кисти.
 *
 * @param color Цвет в формате RGB565.
 */
void touch_set_color(
    uint16_t color);

/**
 * @brief Получение текущего цвета кисти.
 *
 * @return Цвет в формате RGB565.
 */
uint16_t touch_get_color(void);

/**
 * @brief Установка размера кисти.
 *
 * @param size Размер кисти.
 */
void touch_set_brush_size(
    uint8_t size);

/**
 * @brief Получение текущего размера кисти.
 *
 * @return Размер кисти.
 */
uint8_t touch_get_brush_size(void);

/**
 * @brief Установка размера кисти.
 *
 * @param size Размер кисти.
 */
void brush_set_size(
    uint16_t size);


/* Debug ---------------------------------------------------------------------*/

/**
 * @brief Вывод сырых координат тачпада для отладки.
 */
void touch_debug_raw(void);


#endif 

