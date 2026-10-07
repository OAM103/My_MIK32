/**
 * @file    paint_history.c
 * @brief   История штрихов графического редактора Paint
 */

#include "paint_history.h"
#include "lcd.h"

static PaintHistoryPoint paint_history_points[PAINT_HISTORY_MAX_POINTS];
static PaintHistoryStroke paint_history_strokes[PAINT_HISTORY_MAX_STROKES];

static uint16_t paint_history_point_count = 0;
static uint16_t paint_history_stroke_count = 0;

/**
 * @brief Количество штрихов, которые сейчас видны на холсте
 */
static uint16_t paint_history_visible_stroke_count = 0;
static int paint_history_stroke_active = 0;

/**
 * @brief Получение точки истории
 * @param stroke  Сохранённый штрих
 * @param index   Номер точки внутри штриха
 * @return Указатель на точку истории
 */
static PaintHistoryPoint *paint_history_get_point(const PaintHistoryStroke *stroke, uint16_t index);

/**
 * @brief Проверка попадания точки в указанную область.
 */
static int paint_history_point_inside_area(int x, int y, int x_min, int y_min, int x_max, int y_max);

/**
 * @brief Проверка пересечения отрезка с указанной областью.
 */
static int paint_history_line_intersects_area( int x0, int y0, int x1, int y1, int x_min, int y_min, int x_max, int y_max);

/**
 * @brief Проверка попадания точки пипетки в штрих
 * Учитывается размер кисти и смещение рисунка по X, используемое touch_draw_stamp()
 */
static int paint_history_point_hits( int x, int y, const PaintHistoryPoint *point, uint8_t brush_size);

/**
 * @brief Проверка попадания точки пипетки в сегмент штриха. Расстояние до отрезка вычисляется без sqrt().
 */
static int paint_history_segment_hits( int x, int y, const PaintHistoryPoint *point0, const PaintHistoryPoint *point1, uint8_t brush_size);

/**
 * @brief Инициализация истории рисунка
 */
void paint_history_init(void)
{
    paint_history_point_count = 0;
    paint_history_stroke_count = 0;
    paint_history_visible_stroke_count = 0;
    paint_history_stroke_active = 0;
}

/**
 * @brief Начало нового штриха
 *
 * Если перед началом нового штриха была выполнена операция Undo, новая линия начинает новую ветку истории и старая ветка Redo становится недоступной.
 *
 * @param color       Цвет кисти
 * @param brush_size  Размер кисти
 */
void paint_history_start_stroke(uint16_t color, uint8_t brush_size)
{
    PaintHistoryStroke *stroke;

    //Если предыдущий штрих ещё не был завершён, сначала завершаем его
    if (paint_history_stroke_active) paint_history_end_stroke();

    // Если до этого был выполнен Undo, новые штрихи начинают новую ветку истории. Старые штрихи после текущей позиции больше недоступны для Redo
    if (paint_history_visible_stroke_count < paint_history_stroke_count) {

        paint_history_stroke_count = paint_history_visible_stroke_count;

        // Точки старых штрихов остаются в массиве, но больше не считаются частью активной истории.
        if (paint_history_stroke_count > 0) {

            PaintHistoryStroke *last_stroke = &paint_history_strokes[paint_history_stroke_count - 1];
            paint_history_point_count = last_stroke->first_point + last_stroke->point_count;

        } else paint_history_point_count = 0;
    }

    // Не создаём новый штрих, если закончились записи для точек или штрихов.
    if (paint_history_stroke_count >= PAINT_HISTORY_MAX_STROKES) {
        return;
    }

    if (paint_history_point_count >= PAINT_HISTORY_MAX_POINTS) {
        return;
    }

    stroke = &paint_history_strokes[paint_history_stroke_count];

    stroke->color = color;
    stroke->brush_size = brush_size;
    stroke->brush_type = (uint8_t)touch_get_brush_shape();
    stroke->first_point = paint_history_point_count;
    stroke->point_count = 0;

    paint_history_stroke_count++;
    paint_history_stroke_active = 1;
    paint_history_visible_stroke_count = paint_history_stroke_count;
}

/**
 * @brief Добавление точки в текущий штрих
 *
 * @param x Координата X
 * @param y Координата Y
 *
 * @return 1 — точка сохранена
 * @return 0 — точка не сохранена
 */
int paint_history_add_point(int x, int y)
{
    PaintHistoryStroke *stroke;
    if (!paint_history_stroke_active) return 0;
    if (paint_history_stroke_count == 0) return 0;

    if (paint_history_point_count >= PAINT_HISTORY_MAX_POINTS) return 0;
    stroke = &paint_history_strokes[paint_history_stroke_count - 1];

    paint_history_points[paint_history_point_count].x = (int16_t)x;
    paint_history_points[paint_history_point_count].y = (int16_t)y;

    paint_history_point_count++;
    stroke->point_count++;

    return 1;
}

/**
 * @brief Завершение текущего штриха
 */
void paint_history_end_stroke(void)
{
    paint_history_stroke_active = 0;
}

/**
 * @brief Полная очистка истории рисунка
 */
void paint_history_clear(void)
{
    paint_history_point_count = 0;
    paint_history_stroke_count = 0;
    paint_history_visible_stroke_count = 0;
    paint_history_stroke_active = 0;
}

/**
 * @brief Повторная отрисовка всего сохранённого рисунка
 *
 * Для каждого штриха временно восстанавливаются
 * сохранённые цвет, размер и форма кисти.
 */
void paint_history_redraw(void)
{
    uint16_t stroke_index;

    uint16_t current_color;
    uint8_t current_brush_size;
    BrushShape current_brush_shape;

    // Сохраняем текущие настройки кисти. Перерисовка истории временно меняет параметры кисти под каждый сохранённый штрих.
    current_color = touch_get_color();
    current_brush_size = touch_get_brush_size();
    current_brush_shape = touch_get_brush_shape();

    for (stroke_index = 0; stroke_index < paint_history_stroke_count; stroke_index++) {
        PaintHistoryStroke *stroke = &paint_history_strokes[stroke_index];
        uint16_t point_index;
        // Пустой штрих ничего не рисует
        if (stroke->point_count == 0) continue;

        // Временно устанавливаем параметры кисти, сохранённые для этого штриха
        touch_set_color(stroke->color);
        touch_set_brush_size(stroke->brush_size);
        touch_set_brush_shape((BrushShape)stroke->brush_type);

        // Первую точку рисуем отдельно
        PaintHistoryPoint *first_point = paint_history_get_point(stroke, 0);
        touch_draw_stamp(first_point->x, first_point->y);

        // Остальные точки соединяем тем же алгоритмом, который используется обычным рисованием
        for (point_index = 1; point_index < stroke->point_count; point_index++) {
            PaintHistoryPoint *previous_point = paint_history_get_point( stroke, point_index - 1);

            PaintHistoryPoint *current_point =
                paint_history_get_point(stroke, point_index);

            touch_stamp_line(
                previous_point->x,
                previous_point->y,
                current_point->x,
                current_point->y);
        }
    }

    // Восстанавливаем текущие настройки кисти пользователя. Перерисовка истории не должна менять активные настройки инструмента.
    touch_set_color(current_color);
    touch_set_brush_size(current_brush_size);
    touch_set_brush_shape(current_brush_shape);
}

/**
 * @brief Перерисовка части сохранённого рисунка.
 *
 * Перерисовываются только штрихи и сегменты,
 * пересекающие указанную область.
 *
 * @param x_min Левая граница области.
 * @param y_min Верхняя граница области.
 * @param x_max Правая граница области.
 * @param y_max Нижняя граница области.
 */
void paint_history_redraw_area(int x_min, int y_min, int x_max, int y_max)
{
    uint16_t stroke_index;

    uint16_t current_color;
    uint8_t current_brush_size;
    BrushShape current_brush_shape;

    current_color = touch_get_color();
    current_brush_size = touch_get_brush_size();
    current_brush_shape = touch_get_brush_shape();

    for (stroke_index = 0; stroke_index < paint_history_stroke_count; stroke_index++) {

        PaintHistoryStroke *stroke = &paint_history_strokes[stroke_index];
        uint16_t point_index;

        if (stroke->point_count == 0) continue;

        touch_set_color(stroke->color);
        touch_set_brush_size(stroke->brush_size);
        touch_set_brush_shape((BrushShape)stroke->brush_type);

        //Проверяем первую точку штриха
        PaintHistoryPoint *first_point = paint_history_get_point(stroke, 0);
        if (paint_history_point_inside_area(first_point->x, first_point->y, x_min, y_min, x_max, y_max)) {
            touch_draw_stamp(first_point->x,first_point->y);
        }

        //Проверяем все сегменты штриха
        for (point_index = 1; point_index < stroke->point_count; point_index++) {

            PaintHistoryPoint *previous_point = paint_history_get_point(stroke, point_index - 1);
            PaintHistoryPoint *current_point = paint_history_get_point(stroke, point_index);

            if (paint_history_line_intersects_area( previous_point->x, previous_point->y, current_point->x, current_point->y, x_min, y_min, x_max, y_max)) {
                touch_stamp_line( previous_point->x, previous_point->y, current_point->x, current_point->y);
            }
        }
    }
    // Восстанавливаем текущие настройки кисти пользователя
    touch_set_color(current_color);
    touch_set_brush_size(current_brush_size);
    touch_set_brush_shape(current_brush_shape);
}

/**
 * @brief Получение цвета под указателем пипетки.
 *
 * Поиск выполняется от последнего штриха к первому,
 * поэтому при пересечении штрихов выбирается цвет
 * верхнего, то есть последнего нарисованного штриха.
 *
 * @param x     Координата X на экране.
 * @param y     Координата Y на экране.
 * @param color Указатель для результата.
 *
 * @return 1 — цвет найден.
 * @return 0 — рисунок в данной точке не найден.
 */
int paint_history_pick_color(int x, int y, uint16_t *color)
{
    int stroke_index;
    if (color == 0) return 0;
    /*
     * Ищем с конца истории.
     * Последний штрих находится визуально сверху.
     */
    for (stroke_index = (int)paint_history_stroke_count - 1; stroke_index >= 0; stroke_index--) {

        PaintHistoryStroke *stroke = &paint_history_strokes[stroke_index];
        uint16_t point_index;

        if (stroke->point_count == 0) continue;

        // Если штрих состоит только из одной точки
        if (stroke->point_count == 1) {

            PaintHistoryPoint *point =
                paint_history_get_point(stroke, 0);

            if (paint_history_point_hits(x, y, point, stroke->brush_size)) {

                *color = stroke->color;
                return 1;
            }

            continue;
        }

        // Проверяем все сегменты штриха
        for (point_index = 1; point_index < stroke->point_count; point_index++) {

            PaintHistoryPoint *point0 = paint_history_get_point(stroke, point_index - 1);
            PaintHistoryPoint *point1 = paint_history_get_point(stroke, point_index);

            if (paint_history_segment_hits(x, y, point0, point1,  stroke->brush_size)) {
                *color = stroke->color;
                return 1;
            }
        }
    }

    return 0;
}

/**
 * @brief Отмена последнего видимого штриха
 *
 * После Undo штрих остаётся в истории, но перестаёт быть видимым и может быть восстановлен операцией Redo
 */
void paint_history_undo(void)
{
    // Завершаем текущий штрих, если он ещё активен
    if (paint_history_stroke_active) paint_history_end_stroke();

    // Уже нечего отменять
    if (paint_history_visible_stroke_count == 0) return;

    paint_history_visible_stroke_count--;

    // Если после Undo не осталось ни одного видимого штриха - очищаем холст 
    if (paint_history_visible_stroke_count == 0) {

        ClearMassDMA( 480, 280, 0, 40, 0xFFFF);
        return;
    }

    // Очищаем область холста
    ClearMassDMA( 480, 280, 0, 40, 0xFFFF);

    // Временно ограничиваем количество штрихов текущей видимой позицией и перерисовываем историю
    {
        uint16_t saved_stroke_count;

        saved_stroke_count = paint_history_stroke_count;
        paint_history_stroke_count =  paint_history_visible_stroke_count;

        paint_history_redraw();

        paint_history_stroke_count = saved_stroke_count;
    }
}

/**
 * @brief Повтор последнего отменённого штриха.
 */
void paint_history_redo(void)
{
    // Уже нечего возвращать
    if (paint_history_visible_stroke_count >= paint_history_stroke_count) return;

    paint_history_visible_stroke_count++;

    //Перерисовываем весь холст
    ClearMassDMA(480, 280, 0, 40, 0xFFFF);

    {
        uint16_t saved_stroke_count;

        saved_stroke_count = paint_history_stroke_count;

        paint_history_stroke_count = paint_history_visible_stroke_count;

        paint_history_redraw();

        paint_history_stroke_count = saved_stroke_count;
    }
}

/**
 * @brief Получение точки истории.
 *
 * @param stroke  Сохранённый штрих.
 * @param index   Номер точки внутри штриха.
 *
 * @return Указатель на точку.
 */
static PaintHistoryPoint *paint_history_get_point(const PaintHistoryStroke *stroke, uint16_t index)
{
    return &paint_history_points[stroke->first_point + index];
}

/**
 * @brief Проверка попадания точки в область
 *
 * Границы верхнего и левого края включаются, правый и нижний края не включаются
 *
 * @return 1 - точка находится внутри области
 * @return 0 - точка находится вне области
 */
static int paint_history_point_inside_area(int x, int y, int x_min, int y_min, int x_max, int y_max)
{
    if (x >= x_min && x < x_max && y >= y_min && y < y_max) return 1;
    return 0;
}

/**
 * @brief Проверка пересечения отрезка с областью
 *
 * Сначала проверяются концы отрезка, после чего используется дискретная проверка алгоритмом Брезенхэма
 *
 * @return 1 - отрезок пересекает область
 * @return 0 - отрезок не пересекает область
 */
static int paint_history_line_intersects_area(int x0,int y0,int x1,int y1,int x_min,int y_min,int x_max,int y_max)
{
    // Сначала проверяем концы линии
    if (paint_history_point_inside_area(x0, y0, x_min, y_min, x_max, y_max)) return 1;
    if (paint_history_point_inside_area(x1, y1, x_min, y_min, x_max, y_max)) return 1;

    // Если концы находятся снаружи, проверяем саму линию. Используется тот же дискретный алгоритм Брезенхэма, что и при рисовании
    {
        int dx;
        int dy;
        int sx;
        int sy;
        int err;
        int e2;

        int x = x0;
        int y = y0;

        dx = ABS(x1 - x0);
        dy = -ABS(y1 - y0);

        sx = (x0 < x1) ? 1 : -1;
        sy = (y0 < y1) ? 1 : -1;

        err = dx + dy;

        while (1) {

            if (paint_history_point_inside_area(x, y, x_min, y_min, x_max, y_max)) return 1;
            if (x == x1 && y == y1)  break;

            e2 = 2 * err;

            if (e2 >= dy) {
                err += dy;
                x += sx;
            }

            if (e2 <= dx) {
                err += dx;
                y += sy;
            }
        }
    }

    return 0;
}

/**
 * @brief Проверка попадания точки пипетки в штрих
 *
 * Учитывается размер кисти
 *
 * Координаты истории смещаются по X на +12, так как touch_draw_stamp() рисует с таким смещением
 *
 * @return 1 - точка попала в штрих
 * @return 0 - точка не попала в штрих
 */
static int paint_history_point_hits( int x, int y, const PaintHistoryPoint *point, uint8_t brush_size)
{
    int radius;
    int center_x;
    int center_y;

    radius = brush_size / 2;

    center_x = point->x + 12;
    center_y = point->y;

    if (ABS(x - center_x) <= radius && ABS(y - center_y) <= radius) return 1;
    return 0;
}

/**
 * @brief Проверка попадания точки пипетки в сегмент штриха
 *
 * Используется расстояние от точки до отрезка, sqrt() не используется.
 *
 * @return 1 - точка попала в сегмент
 * @return 0 - точка не попала в сегмент
 */
static int paint_history_segment_hits( int x, int y, const PaintHistoryPoint *point0, const PaintHistoryPoint *point1, uint8_t brush_size)
{
    int x0;
    int y0;
    int x1;
    int y1;

    int dx;
    int dy;

    int wx;
    int wy;

    int64_t length_squared;
    int64_t projection;

    int64_t closest_x;
    int64_t closest_y;

    int64_t distance_x;
    int64_t distance_y;
    int64_t distance_squared;

    int radius;

    // Учитываем смещение рисунка по X.
    x0 = point0->x + 12;
    y0 = point0->y;

    x1 = point1->x + 12;
    y1 = point1->y;

    // Сначала проверяем ограничивающий прямоугольник. Это позволяет быстро отсеять далёкие сегменты.
    radius = (brush_size / 2) + 1;

    if (x < ((x0 < x1 ? x0 : x1) - radius) || x > ((x0 > x1 ? x0 : x1) + radius) || y < ((y0 < y1 ? y0 : y1) - radius) || y > ((y0 > y1 ? y0 : y1) + radius)) return 0;

    dx = x1 - x0;
    dy = y1 - y0;

    //Если сегмент фактически является точкой, используем обычную проверку точки
    if (dx == 0 && dy == 0) return paint_history_point_hits( x, y, point0, brush_size);

    wx = x - x0;
    wy = y - y0;

    length_squared = (int64_t)dx * dx + (int64_t)dy * dy;
    projection = (int64_t)wx * dx + (int64_t)wy * dy;

    // Ближайшая точка находится за пределами отрезка — используем соответствующий конец.
    if (projection <= 0) {
        closest_x = x0;
        closest_y = y0;

    } else if (projection >= length_squared) {
        closest_x = x1;
        closest_y = y1;

    } else {

        closest_x = x0 + ((int64_t)dx * projection) / length_squared;
        closest_y = y0 + ((int64_t)dy * projection) / length_squared;
    }

    distance_x = (int64_t)x - closest_x;
    distance_y = (int64_t)y - closest_y;

    distance_squared = distance_x * distance_x +  distance_y * distance_y;

    return distance_squared <= (int64_t)radius * radius;
}
