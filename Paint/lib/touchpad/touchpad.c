/**
 * @file    touchpad.c
 * @brief   Драйвер тачпада и рисование по касанию
 */

/* Includes ------------------------------------------------------------------*/
#include "touchpad.h"
#include "paint_history.h"
#include <stdint.h>

static uint16_t brush_color = TOUCH_DEFAULT_COLOR;  // цвет текущей кисти
static uint16_t brush_size = BRUSH_SIZE_DEFAULT;    // размер текущей кисти

static BrushShape brush_shape = BRUSH_SHAPE_SQUARE;

static int last_x = -1;
static int last_y = -1;

static uint32_t spray_seed = 123456789u;

static uint32_t spray_random(void)
{
    spray_seed = spray_seed * 1664525u + 1013904223u;
    return spray_seed;
}

void touch_set_color(uint16_t color)
{
    brush_color = color;
}

void brush_set_size(uint16_t size)
{
    if (size < 1) size = 1;

    brush_size = size;
}

void touch_set_brush_shape(BrushShape shape)
{
    brush_shape = shape;
}

BrushShape touch_get_brush_shape(void)
{
    return brush_shape;
}

uint16_t touch_get_color(void)
{
    return brush_color;
}

void touch_set_brush_size(uint8_t size)
{
    if (size < TOUCH_MIN_BRUSH_SIZE) size = TOUCH_MIN_BRUSH_SIZE;
    if (size > TOUCH_MAX_BRUSH_SIZE) size = TOUCH_MAX_BRUSH_SIZE;

    brush_size = size;
}

uint8_t touch_get_brush_size(void)
{
    return brush_size;
}

/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Выбор тачпада на шине SPI 
 */
void touch_select(void)
{
    HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS_ALL, __LOW); // сброс всех CS
    HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS1, __HIGH);   // выбор тачпада 
}

/**
 * @brief  Снятие выбора тачпада
 */
void touch_unselect(void)
{
    HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS_ALL, __LOW);
}

/**
 * @brief         Ограничение координат границами экрана
 * @param  x      Горизонтальная координата 
 * @param  y      Вертикальная координата 
 */
void touch_clamp_screen(int *x, int *y)
{
    if (*x < 0) *x = 0;                   // левая граница
    if (*y < 0) *y = 0;                   // верхняя граница
    if (*x >= TOUCH_SCREEN_HEIGHT) *x = TOUCH_SCREEN_HEIGHT - 1;  // правая граница
    if (*y >= TOUCH_SCREEN_WIDTH) *y = TOUCH_SCREEN_WIDTH - 1;  // нижняя граница
}

/**
 * @brief Ограничивает координаты точки границами холста.
 *
 * Центр кисти может находиться непосредственно на границе
 * холста. Выходящие за границу пиксели кисти должны
 * отбрасываться непосредственно при рисовании.
 *
 * @param x Горизонтальная координата.
 * @param y Вертикальная координата.
 */
void touch_clamp_canvas(int *x, int *y)
{
    if (x == 0 || y == 0) {
        return;
    }

    if (*y < 0) {
        *y = 0;
    }

    if (*y >= TOUCH_SCREEN_WIDTH) {
        *y = TOUCH_SCREEN_WIDTH - 1;
    }

    if (*x < TOUCH_CANVAS_TOP) {
        *x = TOUCH_CANVAS_TOP;
    }

    if (*x > TOUCH_CANVAS_BOTTOM) {
        *x = TOUCH_CANVAS_BOTTOM;
    }
}
static void touch_draw_ellipse(int x, int y)
{
    int width = brush_size / 3;
    int height = brush_size;

    int rx;
    int ry;
    int cx;
    int cy;

    if (width < 1) width = 1;

    rx = width / 2;
    ry = height / 2;

    if (rx < 1) rx = 1;
    if (ry < 1) ry = 1;

    cx = x + 12;
    cy = y;

    for (int py = -ry; py <= ry; py++) {

        int value;
        int half_width;
        int left;
        int draw_width;
        int draw_y;

        /*
         * Уравнение эллипса: x²/rx² + y²/ry² <= 1
         * Находим максимальный X для текущего Y.
         */
        value = (rx * rx) * (ry * ry - py * py);

        if (value < 0)  continue;

        half_width = 0;

        while ((half_width + 1) <= rx) {

            int test = (half_width + 1) * (half_width + 1) * (ry * ry);
            if (test > value) break;
            half_width++;
        }

        draw_width = half_width * 2 + 1;

        left = cx - half_width;
        draw_y = cy + py;

        // Ограничение по горизонтали
        if (left < 0) {
            draw_width += left;
            left = 0;
        }

        if (left + draw_width > TOUCH_SCREEN_HEIGHT) {
            draw_width = TOUCH_SCREEN_HEIGHT - left;
        }

        // Ограничение по вертикали
        if (draw_y >= 0 && draw_y < TOUCH_SCREEN_WIDTH && draw_width > 0) draw_rect(draw_width, 1, draw_y, left, brush_color);
    }
}

static void touch_draw_spray(int x, int y)
{
    int radius = brush_size / 2;

    if (radius < 2) radius = 2;

    int cx = x + 12;
    int cy = y;

    for (int i = 0; i < brush_size * 2; i++) {

        int px = (int)(spray_random() % (radius * 2 + 1)) - radius;
        int py = (int)(spray_random() % (radius * 2 + 1)) - radius;

        if ((px * px + py * py) > radius * radius) continue;

        int draw_x = cx + px;
        int draw_y = cy + py;

        if (draw_x >= 0 && draw_x < TOUCH_SCREEN_HEIGHT && draw_y >= 0 && draw_y < TOUCH_SCREEN_WIDTH) draw_rect(1, 1, draw_y, draw_x, brush_color);
    }
}

static void touch_draw_square(int x, int y)
{
    int size = brush_size;
    int left = x - size / 2 + 12;
    int top = y - size / 2;

    touch_clamp_canvas(&left, &top);

    draw_rect(size, size, top, left, brush_color);
}
static void touch_draw_circle(int x, int y)
{
    int radius = brush_size / 2;
    int cx = x + 12;
    int cy = y;

    for (int dy = -radius; dy <= radius; dy++) {
        int value = radius * radius - dy * dy;
        int half_width = 0;
        int width;
        int left;
        int top;

        if (value < 0) continue;

        /* Целочисленный sqrt(value) */
        while ((half_width + 1) * (half_width + 1) <= value) half_width++;
        
        width = half_width * 2 + 1;
        left = cx - half_width;
        top = cy + dy;

        /* Обрезка по X */
        if (left < 0) {
            width += left;
            left = 0;
        }

        if (left + width > TOUCH_SCREEN_HEIGHT) width = TOUCH_SCREEN_HEIGHT - left;

        /* Рисуем одну горизонтальную строку круга */
        if (top >= 0 &&
            top < TOUCH_SCREEN_WIDTH &&
            width > 0) {

            draw_rect( 1, width, top, left, brush_color);
        }
    }
}

/**
 * @brief         Отрисовка точки касания 
 * @param  x      Горизонтальная координата 
 * @param  y      Вертикальная координата 
 */
void touch_draw_stamp(int x, int y)
{
    switch (brush_shape) {

        case BRUSH_SHAPE_CIRCLE:
            touch_draw_circle(x, y);
            break;

        case BRUSH_SHAPE_ELLIPSE:
            touch_draw_ellipse(x, y);
            break;

        case BRUSH_SHAPE_SPRAY:
            touch_draw_spray(x, y);
            break;

        case BRUSH_SHAPE_SQUARE:
        default:
            touch_draw_square(x, y);
            break;
    }
}
/**
 * @brief         Линия между двумя точками (алгоритм Брезенхэма)
 * @param  x0     Начало, горизонталь
 * @param  y0     Начало, вертикаль
 * @param  x1     Конец, горизонталь
 * @param  y1     Конец, вертикаль
 */
void touch_stamp_line(int x0, int y0, int x1, int y1)
{
    int dx;
    int dy;
    int sx;
    int sy;
    int err;
    int e2;
    uint16_t steps = 0;
    uint16_t max_steps;

    dx = ABS(x1 - x0);
    max_steps = (uint16_t)(dx + ABS(y1 - y0) + 1);
    sx = (x0 < x1) ? 1 : -1;
    dy = -ABS(y1 - y0);
    sy = (y0 < y1) ? 1 : -1;
    err = dx + dy;

    while (1) {
        touch_draw_stamp(x0, y0);
        if (x0 == x1 && y0 == y1) {
            break;
        }
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
        if (++steps > max_steps) break;
    }
}

/**
 * @brief         Усреднённое чтение пары Y/X 
 * @param  raw_x  Усреднённое сырое значение оси X
 * @param  raw_y  Усреднённое сырое значение оси Y
 * @return        1 — касание есть, 0 — нет валидных значений
 */
int touch_read_averaged(int *raw_x, int *raw_y)
{
    int32_t sum_x = 0;    
    int32_t sum_y = 0;    
    uint8_t valid = 0;

    touch_select();

    for (uint8_t i = 0; i < TOUCH_FILTER_SAMPLES; i++) {
        int vy = read_coordinate(READ_Y);  // сначала читаем Y
        int vx = read_coordinate(READ_X);  // затем X

        if (vx > 0 && vy > 0) {
            sum_x += vx;
            sum_y += vy;
            valid++;
        }
        HAL_DelayMs(TOUCH_FILTER_DELAY);  // пауза между измерениями
    }

    touch_unselect();

    if (valid == 0) return 0;  // нет касания 

    *raw_x = (int)(sum_x / valid);  // среднее по X
    *raw_y = (int)(sum_y / valid);  // среднее по Y
    return 1;
}

/**
 * @brief         Преобразование координат в пиксели экрана
 * @param  raw_x  Сырое значение оси X тачпада
 * @param  raw_y  Сырое значение оси Y тачпада
 * @param  sx     Горизонтальная координата на дисплее
 * @param  sy     Вертикальная координата на дисплее
 * @return        1 — координаты получены, 0 — касания нет
 */
int touch_raw_to_screen(int raw_x, int raw_y, int *sx, int *sy)
{
    int rx = raw_x;     int span_x;        
    int ry = raw_y;     int span_y;

    if (rx <= 0 || ry <= 0) return 0;  // выход, если нет касания

    /* ограничение значений допустимым диапазоном */
    if (rx < TOUCH_MIN_RAW_X) rx = TOUCH_MIN_RAW_X;
    if (rx > TOUCH_MAX_RAW) rx = TOUCH_MAX_RAW;
    if (ry < TOUCH_MIN_RAW_Y) ry = TOUCH_MIN_RAW_Y;
    if (ry > TOUCH_MAX_RAW) ry = TOUCH_MAX_RAW;

    span_x = TOUCH_MAX_RAW - TOUCH_MIN_RAW_X;  // диапазон по X
    span_y = TOUCH_MAX_RAW - TOUCH_MIN_RAW_Y;  // диапазон по Y

    if (span_x <= 0 || span_y <= 0) return 0;

    /* x  - горизонталь, y - вертикаль */
    *sx = (rx - TOUCH_MIN_RAW_X) * (TOUCH_SCREEN_HEIGHT - 1) / span_x;
    *sy = (TOUCH_SCREEN_WIDTH - 1) - (ry - TOUCH_MIN_RAW_Y) * (TOUCH_SCREEN_WIDTH - 1) / span_y;

    return 1;
}

/**
 * @brief           Чтение одной координаты тачпада по SPI
 * @param  command  Команда чтения: READ_X (0xD0) или READ_Y (0x90)
 * @return          12-битное значение координат с тачпада, иначе -1 при ошибке SPI
 */
int read_coordinate(unsigned char command)
{
    int16_t value;
    // Подготовка пакета (Команда + 2 байта для приема)
    uint8_t master_output[] = {command, 0x00, 0x00};  // Тачпад отправляет 12-битные координваты, упакованные в 2 байта
    uint8_t master_input[3] = {0}; // Инициализируем нулями
    
    // Обмен данными по SPI (Блокирующий режим)
    HAL_StatusTypeDef SPI_Status = HAL_SPI_Exchange(&hspi1, master_output, master_input, sizeof(master_output), SPI_TIMEOUT_DEFAULT);  // отправка команды

    if (SPI_Status != HAL_OK) {
        xprintf("SPI_Error %d, OVR %d, MODF %d\n", SPI_Status,
                hspi1.ErrorCode & HAL_SPI_ERROR_OVR,
                hspi1.ErrorCode & HAL_SPI_ERROR_MODF);
        HAL_SPI_ClearError(&hspi1);
        return -1;  // ошибка обмена
    }

    /*Комбинирование 12 бит данных

    Байты 1 и 2 содержат 16 бит, из которых 12 бит - это полезная нагрузка (координата).
    Обычно 4 младших бита первого байта бесполезны, а 4 старших бита второго байта бесполезны.
    
    Собираем 12 бит: (master_input[1] << 4) | (master_input[2] >> 4)

    value = ((master_input[1] & 0x0F) << 8) | master_input[2]; // Простая сборка 16 бит

    Старший байт (master_input[1]) содержит 4 старших бита
    Младший байт (master_input[2]) содержит 8 младших бит*/

    value = (master_input[1] << 4) | (master_input[2] >> 4);

    return value;
}

/**
 * @brief           Усреднённое чтение одной оси (для button.c)
 * @param  command  Команда чтения: READ_X или READ_Y
 * @param  samples  Число замеров для усреднения
 * @return          Среднее значение координаты, иначе -1
 */
int read_filtered_coordinate(uint8_t command, uint8_t samples)
{
    int32_t sum = 0;
    uint8_t valid = 0;

    for (uint8_t i = 0; i < samples; i++) {
        int val = read_coordinate(command);
        if (val > 0) {   // только корректные измерения 
            sum += val;
            valid++;
        }
        HAL_DelayMs(2);  // пауза между измерениями
    }

    if (valid == 0) return -1;       // соответственно нет данных

    return sum / valid;  // среднее значение
}

/**
 * @brief          Линейное преобразование координаты в другой диапазон
 * @param  value   Исходное значение
 * @param  r1_max  Верхняя граница исходного диапазона
 * @param  r1_min  Нижняя граница исходного диапазона
 * @param  r2_max  Верхняя граница целевого диапазона
 * @param  r2_min  Нижняя граница целевого диапазона
 * @return         Преобразованное значение с ограничением по границам
 */
uint16_t transform(uint16_t value, uint16_t r1_max, uint16_t r1_min,
                   uint16_t r2_max, uint16_t r2_min)
{
    int scale;    
    int res;

    if (value <= r1_min) return r2_min;  
    if (value >= r1_max) return r2_max; 

    scale = (r2_max - r2_min) * 1000 / (r1_max - r1_min);  
    res = (value - r1_min) * scale / 1000;  // преобразование координат с тачпада в координаты экрана

    if (res < r2_min) res = r2_min;  
    if (res > r2_max) res = r2_max; 

    return res;
}
void touch_reset_stroke(void)
{
    paint_history_end_stroke();

    last_x = -1;
    last_y = -1;
}
void touch_draw_point(int x, int y)
{
    if (x < TOUCH_CANVAS_TOP - 10) {
        last_x = -1;
        last_y = -1;
        return;
    }

    if (last_x < 0 || last_y < 0) {
        paint_history_start_stroke(brush_color, (uint8_t)brush_size);

        paint_history_add_point(x, y);

        touch_draw_stamp(x, y);

        last_x = x;
        last_y = y;

        return;
    }

    if (x == last_x && y == last_y)  return;

    {
        int dx = x - last_x;
        int dy = y - last_y;

        if ((dx * dx + dy * dy) > TOUCH_MAX_JUMP_SQ) {
            touch_draw_stamp(x, y);

            last_x = x;
            last_y = y;

            return;
        }
    }

    touch_stamp_line(last_x, last_y, x, y);

    paint_history_add_point(x, y);

    last_x = x;
    last_y = y;
}

/**
 * @brief  Рисование линий при движении стилуса по экрану
 */

void touch_draw_lines(void)
{
    int x;
    int y;
    int raw_x;
    int raw_y;

    if (!touch_read_averaged(&raw_x, &raw_y) || !touch_raw_to_screen(raw_x, raw_y, &x, &y)) {
        touch_reset_stroke();
        return;
    }

    touch_draw_point(x, y);
}

/**
 * @brief  Обработка касания и отрисовка маркера 
 *
 * Рисует маркер (прямоугольник 10×10) в точке касания, если координаты
 * изменились больше чем на 20 пикселей
 */
void process_touchpad(void)
{
    int raw_x;    int sx;    static int last_raw_x = 0;
    int raw_y;    int sy;    static int last_raw_y = 0;

    if (!touch_read_averaged(&raw_x, &raw_y) || !touch_raw_to_screen(raw_x, raw_y, &sx, &sy)) return;  // выход, если нет касания
    /* игнорировать "дрожание" при касании*/
    if (ABS(raw_x - last_raw_x) < TOUCH_NOISE_THRESHOLD && ABS(raw_y - last_raw_y) < TOUCH_NOISE_THRESHOLD) return;

    last_raw_x = raw_x;
    last_raw_y = raw_y;

    /* маркер 10×10 */
    draw_rect(10, 10, sy, sx, TOUCH_DEFAULT_COLOR);
}

void touch_debug_raw(void)
{
    int raw_x;
    int raw_y;

    if (touch_read_averaged(&raw_x, &raw_y)) xprintf("RAW X=%d Y=%d\r\n", raw_x, raw_y);
}