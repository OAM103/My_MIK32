/**
 * @file    paint_history.h
 * @brief   История штрихов графического редактора Paint
 */

#ifndef PAINT_HISTORY_H
#define PAINT_HISTORY_H

#include "touchpad.h"

/*
 * Максимальное количество точек, которое можно сохранить в истории рисунка
 * Одна точка хранит координаты X и Y: 2 байта + 2 байта = 4 байта
 * 3000 точек = примерно 12 КБ.
 */
#define PAINT_HISTORY_MAX_POINTS 1500
#define PAINT_HISTORY_MAX_STROKES 128

#define PAINT_BRUSH_SQUARE   0
#define PAINT_BRUSH_CIRCLE   1
#define PAINT_BRUSH_ELLIPSE  2
#define PAINT_BRUSH_SPRAY    3

void paint_history_redraw_area(int x_min, int y_min, int x_max, int y_max);

/**
 * @brief  Одна сохранённая точка штриха
 */
typedef struct
{
    int16_t x;
    int16_t y;
} PaintHistoryPoint;

/**
 * @brief  Информация о штрихе
 */
typedef struct
{
    uint16_t first_point;
    uint16_t point_count;
    uint16_t color;
    uint16_t brush_size;
    uint8_t brush_type;
} PaintHistoryStroke;

/**
 * @brief Получить цвет рисунка под указанной координатой.
 *
 * Поиск выполняется от последнего штриха к первому, поэтому верхний (последний нарисованный) штрих имеет приоритет
 *
 * @param x Координата X
 * @param y Координата Y
 *
 * @param color Указатель для найденного цвета
 *
 * @return 1 - рисунок найден
 * @return 0 - в данной точке рисунка нет
 */
int paint_history_pick_color( int x, int y, uint16_t *color);

/**
 * @brief  Инициализация истории рисунка
 */
void paint_history_init(void);

/**
 * @brief  Начало нового штриха
 *
 * Вызывается в момент первого касания холста
 *
 * @param color       Цвет кисти
 * @param brush_size  Размер кисти
 */
void paint_history_start_stroke(uint16_t color, uint8_t brush_size);

/**
 * @brief  Добавление точки в текущий штрих
 *
 * @param x  Координата X на экране
 * @param y  Координата Y на экране
 *
 * @return 1 - точка сохранена
 * @return 0 - история заполнена
 */
int paint_history_add_point(int x, int y);

/**
 * @brief  Завершение текущего штриха
 */
void paint_history_end_stroke(void);

/**
 * @brief  Полная очистка истории рисунка
 */
void paint_history_clear(void);

/**
 * @brief  Повторная отрисовка всего сохранённого рисунка
 */
void paint_history_redraw(void);

/**
 * @brief  Шаг назад (Ctrl+Z)
 */
void paint_history_undo(void);

/**
 * @brief  Шаг вперёд (Ctrl+Y)
 */
void paint_history_redo(void);

#endif 