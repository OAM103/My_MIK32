#ifndef __GAME__
#define __GAME__

#include "Mikboy.hpp"
#include "png.hpp"

#define MAX_ARROWS  10      // максимум стрелочек на экране одновременно
#define HIT         96      // область попадания по Y
#define HIT_WINDOW  7       // уровень сложности (чем больше число, тем проще попасть)

struct Arrow {
    uint8_t lane;     // 0-3 (UP, DOWN, LEFT, RIGHT)
    int16_t prev_y;   // предыдущая позиция
    int16_t y;        // текущая позиция
    bool active;      // жива ли
};
enum class GameState
{
    START,      // начальный экран
    COUNTDOWN,  // обратный отсчёт
    RUNNING,    // сама игра
    GAME_OVER,  // экран итога
};
struct Note
{
    uint32_t time_ms;   // когда появляется (время относительно всей песни)
    uint8_t lane;       // какая стрелка (0-3)
};
struct GradeInfo        // информация об оценке
{
    const char* grade;
    const char* message;
};

extern const uint16_t* arrows_img[4];           //набор летающих стрелок
extern const uint16_t* arrows_contour_img[4];   //набор стрелок для hit-зоны
extern Arrow arrows[MAX_ARROWS];
extern uint32_t game_start_time;
extern GameState game_state;
extern uint32_t game_time();
extern int game_over; // флаг для экрана завершения

void restart_game(Mikboy mik);      // сброс параметров для рестарта
void game_over_screen(Mikboy mik);  // вывод экрана счета
void check_button(Mikboy mik);      // проверка кнопок
void start_screen(Mikboy mik);      // рисуем начальный экран
void arrow_animation(Mikboy mik);   // игровой процесс
void draw_hit_zone(Mikboy mik);     // рисуем зону попадания
void draw_countdown(Mikboy mik);    // экран отсчёта
void draw_score(Mikboy mik);        // вывод счёта

#endif 

