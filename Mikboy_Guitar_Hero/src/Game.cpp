#include "Game.hpp"
#include "Mikboy.hpp"
#include "Music.hpp"
#include <cstdio>
#include "png.hpp"
#include "mik32_hal_usart.h"

Arrow arrows[MAX_ARROWS]; // массив стрелочек[максимальное количество стрелочек на экране одновременно]
uint16_t lane_x[4] = {20, 60, 100, 140}; // позиции стрелочек по x
bool prev_btn[4] = {true, true, true, true}; // прошлое состояние кнопок 0 - нажата, 1 - ненажата (для определения момента нажатия кнопки, а не удержания)

#define SEC 1000            //секунда
#define START_Y 20          // отступ от шапочки
#define HIT 96              // Y хит-зоны
#define ARROW_SPEED 100.0f  // пикселей в секунду

int32_t hits = 0;          // общий счёт
uint8_t hit_arrows = 0;     // попавшие стрелочки
uint8_t mistakes_arrows = 0;// не те нажатия стрелочек
int hit_fail = 0;           // флаг неверного нажатия
int hit_fall = 0;           // флаг ушедшей стрелочки
int hit_luck = 0;           // флаг верного попадания

uint32_t game_start_time = 0;

const uint16_t* arrows_img[4] = {           //набор летающих стрелок
    arrow_up,
    arrow_down,
    arrow_left,
    arrow_right
};
const uint16_t* arrows_contour_img[4] = {   //набор стрелок для hit-зоны
    arrow_contour_up,   
    arrow_contour_down,
    arrow_contour_left,
    arrow_contour_right 
};
int8_t lane_btns[4] = { // список кнопок
    int8_t(BUTTON_PIN::UP),
    int8_t(BUTTON_PIN::DOWN),
    int8_t(BUTTON_PIN::LEFT),
    int8_t(BUTTON_PIN::RIGHT)
};
int time_offset = 300; // переменная подгона стлелочек под музыку

GameState game_state = GameState::START;

Note chart[] = {
    {14 * SEC -200+ 933, 0},
    {15 * SEC -200+ 451, 3},
    {15 * SEC -200+ 903, 3},
    {16 * SEC -200+ 455, 2},
    {16 * SEC -200+ 935, 2},
    {17 * SEC -200+ 418, 1},
    {17 * SEC -200+ 869, 1},
    {18 * SEC -200+ 386, 0},
    {18 * SEC -200+ 831, 1},
    {19 * SEC -200+ 316, 2},
    {19 * SEC -200+ 798, 3},
    {20 * SEC -200+ 282, 2},
    {20 * SEC -200+ 736, 3},
    {21 * SEC -200+ 312, 1},
    {21 * SEC -200+ 770, 1},
    {22 * SEC -200+ 441, 0},
    {22 * SEC -200+ 793, 0},
    {23 * SEC -200+ 243, 2},
    {23 * SEC -200+ 631, 2},
    {24 * SEC -200+ 113, 3},
    {24 * SEC -200+ 598, 1},
    {25 * SEC -200+ 49, 0},
    {25 * SEC -200+ 563, 2},
    {25 * SEC -200+ 980, 0},
    {26 * SEC -200+ 437, 2},
    {27 * SEC -200+ 53, 3},
    {27 * SEC -200+ 401, 3},
    {28 * SEC -200+ 590, 2},
    {28 * SEC -200+ 911, 2},
    {29 * SEC -200+ 948, 1},
    {30 * SEC -200+ 338, 0},
    {30 * SEC -200+ 947, 2},
    {31 * SEC -200+ 270, 2},
    {32 * SEC -200+ 300, 3},
    {32 * SEC -200+ 621, 3},
    {33 * SEC -200+ 716, 1},
    {34 * SEC -200+ 174, 0},
    {34 * SEC -200+ 713, 2},
    {35 * SEC -200+ 10, 2},
    {36 * SEC -200+ 70, 3},
    {36 * SEC -200+ 397, 3},
    {37 * SEC -200+ 419, 1},
    {37 * SEC -200+ 808, 0},
    {38 * SEC -200+ 352, 2},
    {38 * SEC + 673, 2},
    {39 * SEC + 777, 3},
    {40 * SEC + 97, 3},
    {41 * SEC + 153, 1},
    {41 * SEC + 510, 0},
    {42 * SEC + 184, 2},
    {42 * SEC + 445, 3},
    {43 * SEC + 229, 1},
    {43 * SEC + 480, 1},
    {44 * SEC + 219, 3},
    {44 * SEC + 474, 2},
    {44 * SEC + 763, 0},
    {45 * SEC + 200+185, 1},
    {45 * SEC + 200+438, 3},
    {45 * SEC + 200+695, 2},
    {46 * SEC + 200+113, 1},
    {46 * SEC + 200+597, 2},
    {47 * SEC + 200+15, 1},
    {47 * SEC + 200+508, 2},
    {48 * SEC + 200+530, 3},
    {48 * SEC + 200+992, 1},
    {49 * SEC + 200+239, 0},
    {49 * SEC + 200+600, 1},
    {49 * SEC + 200+851, 0},
    {50 * SEC + 200+111, 1},
    {50 * SEC + 200+367, 0},
    {50 * SEC + 200+560, 1},
    {50 * SEC + 200+891, 3},
    {51 * SEC + 200+110, 2},
    {51 * SEC + 200+819, 1},
    {52 * SEC + 200+367, 0},
    {52 * SEC + 200+792, 3},
    {53 * SEC + 200+268, 2},
    {53 * SEC + 200+883, 1},
    {54 * SEC + 200+205, 1},
    {55 * SEC + 200+361, 0},
    {55 * SEC + 200+684, 0},
    {56 * SEC + 200+684, 2},
    {57 * SEC + 200+72, 3},
    {57 * SEC + 200+623, 1},
    {57 * SEC + 200+911, 1},
    {59 * SEC + 200+139, 0},
    {59 * SEC + 200+392, 0},
    {60 * SEC + 200+358, 2},
    {60 * SEC + 200+710, 3},
    {61 * SEC + 200+366, 1},
    {61 * SEC + 200+771, 3},
    {62 * SEC + 200+42, 3},
    {62 * SEC + 200+351, 2},
    {62 * SEC + 200+707, 1},
    {62 * SEC + 200+973, 1},
    {63 * SEC + 200+319, 0},
    {67 * SEC + 500+300+375, 2},
    {67 * SEC + 500+300+728, 3},
    {68 * SEC + 500+300+58, 1},
    {68 * SEC + 500+300+311, 1},
    {68 * SEC + 500+300+594, 0},
    {68 * SEC + 500+300+850, 0},
    {69 * SEC + 500+300+19, 2},
    {69 * SEC + 500+300+273, 3},
    {69 * SEC + 500+300+556, 1},
    {70 * SEC + 500+300+42, 0},
    {70 * SEC + 500+300+468, 2},
    {70 * SEC + 500+300+949, 3},
    {71 * SEC + 500+300+487, 1},
    {72 * SEC + 500+300+84, 3},
    {72 * SEC + 500+300+274, 3},
    {72 * SEC + 500+300+603, 2},
    {72 * SEC + 500+300+885, 2},
    {73 * SEC + 500+300+153, 1},
    {73 * SEC + 500+300+692, 0},
    {74 * SEC + 500+300+175, 3},
    {74 * SEC + 500+300+653, 2},
    {75 * SEC + 500+300+110, 1},
    {75 * SEC + 500+300+618, 1},
    {76 * SEC + 500+300+236, 0},
    {76 * SEC + 500+300+580, 3},
    {77 * SEC + 500+300+15, 2},
    {77 * SEC + 500+300+719, 1},
    {78 * SEC + 500+300+206, 1},
    {78 * SEC + 500+300+838, 2},
    {79 * SEC + 500+300+359, 3},
    {79 * SEC + 500+300+842, 2},
    {80 * SEC + 500+300+357, 0},
    {80 * SEC + 500+300+838, 0},
    {81 * SEC + 500+300+392, 1},
    {81 * SEC + 500+300+902, 0},
    {83 * SEC + 500+300+190, 2},
    {83 * SEC + 500+300+450, 2},
    {83 * SEC + 500+300+738, 3},
    {83 * SEC + 500+300+934, 3},
    {84 * SEC + 500+300+383, 1},
    {84 * SEC + 500+300+895, 0},
    {85 * SEC + 500+300+387, 1},
    {85 * SEC + 500+300+831, 0},
    {86 * SEC + 500+300+285, 2},
    {86 * SEC + 500+300+767, 3},
    {87 * SEC + 500+300+156, 3},
    {87 * SEC + 500+300+601, 2},
    {88 * SEC + 500+300+157, 1},
    {88 * SEC + 500+300+406, 1},
    {88 * SEC + 500+300+676, 0},
    {88 * SEC + 500+300+898, 0},
    {89 * SEC + 500+300+154, 3},
    {89 * SEC + 500+300+356, 3},
    {89 * SEC + 500+300+664, 2},
    {90 * SEC + 500+300+175, 1},
    {90 * SEC + 500+300+626, 2},
    {91 * SEC + 500+300+23, 2},
    {91 * SEC + 500+300+444, 0},
    {91 * SEC + 500+300+986, 1},
    {92 * SEC + 500+300+401, 3},
    {92 * SEC + 500+300+914, 2},
    {93 * SEC + 500+300+501, 0},
    {93 * SEC + 500+300+945, 1},
    {94 * SEC + 500+300+462, 2},
    {94 * SEC + 500+300+914, 0},
    {95 * SEC + 500+300+331, 3},
    {95 * SEC + 500+300+819, 1},
    {96 * SEC + 500+300+332, 2},
    {96 * SEC + 500+300+883, 0},
    {97 * SEC + 500+300+306, 2},
    {99 * SEC + 500+300+686, 3},
    {99 * SEC + 500+800+943, 2},
    {100 * SEC +500+800+ 228, 0},
    {100 * SEC +500+800+ 562, 1},
    {101 * SEC +500+800+ 1, 1},
    {101 * SEC +500+800+ 388, 0},
    {101 * SEC +500+800+ 877, 2},
    {102 * SEC +500+800+ 421, 2},
    {103 * SEC +500+800+ 4, 3},
    {103 * SEC +500+800+ 520, 1},
    {103 * SEC +500+800+ 834, 1},
    {104 * SEC +500+800+ 92, 0},
    {104 * SEC +500+800+ 443, 2},
    {104 * SEC +500+800+ 827, 3},
    {105 * SEC +500+800+ 279, 1},
    {105 * SEC +500+800+ 769, 3},
    {107 * SEC +500+800+ 164, 2},
    {107 * SEC +500+800+ 415, 2},
    {107 * SEC +500+800+ 732, 1},
    {108 * SEC +500+800+ 157, 1},
    {108 * SEC +500+800+ 540, 3},
    {108 * SEC +500+800+ 964, 0},
    {109 * SEC +500+800+ 411, 1},
    {109 * SEC +500+800+ 957, 2},
    {110 * SEC +500+800+ 700, 0},

};

const int CHART_SIZE = sizeof(chart) / sizeof(chart[0]); //размер нотной карты
int current_note = 0; // индекс текущей ноты - показывает, какая стрелочка должна появиться следующей

void spawn_arrow_lane(int lane) //создание стрелочки
{
    for (int i = 0; i < MAX_ARROWS; i++) // ищем свободное место в массиве стрелочек
    {
        if (!arrows[i].active) // стрелочка не активна - слот свободен
        {
            arrows[i].active = true;    // активируем стрелочку
            arrows[i].y = 20;           // начальная позиция сверху экрана
            arrows[i].lane = lane;      // назначаем дорожку
            break;
        }
    }
}

bool score_changed = true; // флаг - нужно ли перерисовывать счёт

void draw_score(Mikboy mik)                             // отрисовка счёта
{
    if (!score_changed) return;                         // счёт не изменился - ничего не рисуем
    score_changed = false;                              // сбрасываем флаг
    mik.lcd().draw_fill_rect(0, 0, 30, 17, 0x99E6);    // закрашиваем шапочку
    char buf[32];                                       // буфер для текста
    if(hits < 0) hits = 0;
    sprintf(buf, "%u", hits);             // формируем строку
    mik.lcd().print_word(5, 5, buf, 0xFFFF, 1);         // вывод текста на экран
}

void update_chart()   //спавн стрелок по времени
{
    uint32_t now = game_time();  // текущее время работы программы

    while (current_note < CHART_SIZE && now >= chart[current_note].time_ms + time_offset) // пока есть ещё ноты и пришло их время, создаём стрелочки
    {
        spawn_arrow_lane(chart[current_note].lane); // создание стрелочки
        current_note++; // переход к следующей ноте
    }
}

void clear_arrow(Mikboy mik, int i, bool deactivate) // удаление стрелки
{   // получаем координаты стрелочки
    uint16_t x = lane_x[arrows[i].lane];
    uint16_t y = arrows[i].y;
    mik.lcd().draw_fill_rect_gradient(x, y, x + 15, y + 11,BG_UP, BG_DOWN); // закрашиваем старую стрелочку 
    if (deactivate) arrows[i].active = false; // выключаем стрелку когда уходит за экран
}

void update_arrows(Mikboy mik) // обновление позиции стрелочек
{
    static uint32_t last_time = game_time(); // текущее время
    uint32_t now = game_time();  // запоминаем новое время движения
    // сколько времени прошло с прошлого кадра
    float delta_time = (now - last_time) / 1000.0f;

    last_time = now; // запоминаем новое время движения

    for (int i = 0; i < MAX_ARROWS; i++) // обновляем все стрелочки
    {
        if (!arrows[i].active) continue; // если стрелка не существует, то продолжаем
        clear_arrow(mik, i, false); // стираем старую позицию
        // движение по реальному времени
        arrows[i].y += ARROW_SPEED * delta_time;

        if (arrows[i].y > 150) // если стрелка ушла за экран
        {
            hit_fall = 1;
            clear_arrow(mik, i, true); // удаляем стрелку
            time_offset+=10;
            hits-=3;
            if(hits < 0) hits = 0;
            mistakes_arrows++;
        }
    }
}

void draw_arrows(Mikboy mik) // рисуем стрелочки
{
    for (int i = 0; i < MAX_ARROWS; i++) // просматриваем все стрелочки
    {
        if (arrows[i].active) // рисуем только активные
        {
            uint16_t x = lane_x[arrows[i].lane]; // получаем х стрелочки
            mik.lcd().print_mas_bg_gradient(x, arrows[i].y, arrows_img[arrows[i].lane], 16, 12, BG_ARROW_CONTOUR, 0xFFFF, BG_UP,  BG_DOWN); // рисуем стрелочку
        }
    }
}

void draw_hit_zone(Mikboy mik) // рисует стрелочки-HIT-зону
{
    for (int lane = 0; lane < 4; lane++) // Русуем 4 контурных стрелочки
    {   // устонавливаем координаты
        uint16_t x = lane_x[lane];   
        uint16_t y = HIT;
        mik.lcd().print_mas_bg_gradient(x, y, arrows_contour_img[lane], 16, 12, BG_ARROW_CONTOUR, 0x6944, BG_UP, BG_DOWN); // прорисовка хит зоны
        if (hit_fail || hit_fall) mik.lcd().print_mas_bg_gradient(x, y, arrows_contour_img[lane], 16, 12, BG_ARROW_CONTOUR, 0xF800, BG_UP, BG_DOWN); // если промахнулся, то хит зона красная
        if (hit_luck){
            mik.lcd().print_mas_bg_gradient(x, y, arrows_contour_img[lane], 16, 12, BG_ARROW_CONTOUR, 0x07E0, BG_UP, BG_DOWN); // если попал, то хит зона зелёная
        }
    }
    hit_fall = 0;
    hit_luck = 0;
}

bool is_hit(uint16_t y) // Проверяем: находится ли стрелка рядом с HIT-зоной
{
    return (y >= HIT - HIT_WINDOW && y <= HIT + HIT_WINDOW);
}

bool is_pressed_now(int lane) // проверка точечного нажатия
{
    bool current = ll_gpio_read(int8_t(lane_btns[lane])); // текущее состояние кнопки
    // было не нажато (1), стало нажато (0)
    bool pressed_event = (prev_btn[lane] == true && current == false);
    prev_btn[lane] = current; // сохраняем состояние
    return pressed_event; // возвращаем: произошло ли новое нажатие
}

void handle_input(Mikboy mik) // обработчик нажатий
{
    hit_fail = 0;
    // uint32_t travel_time = ((HIT - START_Y) / ARROW_SPEED) * 1450; // время полёта стрелки (сколько времени стрелка летит от спавна до HIT-зоны)

    for (int lane = 0; lane < 4; lane++) // перебор дорожек
    {
        if (!is_pressed_now(lane))continue; //проверка нового нажатия
        // запись нот
        // uint32_t now = game_time();
        // uint32_t note_time = now - travel_time;

        // uint32_t sec = note_time / 1000 ;
        // uint32_t ms  = note_time % 1000 ;

        // xprintf("{%lu * SEC + %lu, %d},\n",sec, ms, lane);

        // проверка попадания
        bool hit = false;

        for (int i = 0; i < MAX_ARROWS; i++) // проверка всех стрелок
        {
            if (!arrows[i].active)continue; // проверка активности
            if (arrows[i].lane != lane)continue; // проверка линии (при нажатии определённой стрелки ищем только её)

            if (is_hit(arrows[i].y)) // проверка попадания, попали ли стрелочки в хит зону
            {
                clear_arrow(mik, i, true); //удаление старой стрелки
                hits+=10;  
                hit_luck = 1;
                hit_arrows++; // увеличиваем количество попавших стрелочек
                score_changed = true; //надо поменять счёт
                hit = true;

                break;
            }
        }

        if (!hit)
        {
            time_offset += 100;
            hits -= 5;
            if(hits < 0) hits = 0;
            play_bad_sound();
            hit_fail = 1;
        }
    }
}
const uint32_t SONG_END = (110 * SEC + 800 + 700) -  (14 * SEC);

void draw_progress_bar(Mikboy mik)
{
    uint32_t now = game_time() - (14 * SEC + 933);

    uint16_t width = (now * 160) / SONG_END; // вычисляем ширину полоски

    if (width >= 160) width = 159; // защита от выхода за экран

    mik.lcd().draw_fill_rect(width - 1, 18, width, 19, 0xFFFF); // рисуем полоску
}

void start_screen(Mikboy mik){
    mik.lcd().print_word(51, 9, "welcome to", 0x6944, 1);
    mik.lcd().print_word(43, 95, "mini edition", 0xFF0F, 1);

    mik.lcd().print_word(52, 10, "welcome to", 0xFF0F, 1);
    mik.lcd().print_word(42, 94, "mini edition", 0x6944, 1);
    mik.lcd().print_mas_gradient(30, 20, guitar_hero, 100, 75, 0x0000, 0x0000, BG_UP, BG_DOWN);
    mik.lcd().print_word(33, 112, "press A to start", 0x6900, 1);
}

void draw_countdown(Mikboy mik) //обратный отсчёт
{
    static int prev_stage = -1;

    uint32_t now = game_time();

    int stage;

    if (now < 11500)      stage = 0;
    else if (now < 12500) stage = 1;
    else if (now < 13500) stage = 2;
    else if (now < 14500) stage = 3;
    else                  stage = 4;

    if (stage != prev_stage)
    {
        // очистить область текста
        mik.lcd().draw_fill_rect_gradient(1, 40, 159, 90, BG_UP, BG_DOWN);

        switch (stage)
        {
            case 0:
                draw_hit_zone(mik);
                mik.lcd().print_word(10, 45, "READY?", 0xFFFF, 4);
                break;

            case 1:
                mik.lcd().print_word(40, 45, "3...", 0xFFFF, 4);
                break;

            case 2:
                mik.lcd().print_word(40, 45, "2...", 0xFFFF, 4);
                break;

            case 3:
                mik.lcd().print_word(40, 45, "1...", 0xFFFF, 4);
                break;

            case 4:
                game_state = GameState::RUNNING;
                break; // просто очистили область
        }

        prev_stage = stage;
    }
    
}
int game_over = 1;
const GradeInfo grade_table[] =
{
    {"F",   "   you loser"},
    {"F+",  "   try again"},
    {"F++", " not even close"},  // даже близко не подошёл
 
    {"E--", "    wake up"},
    {"E-",  " keep practicing"},
    {"E",   " getting better"},  // становится лучше
    {"E+",  "    not bad"},
    {"E++", "   almost D"},        // почти хорошо
 
    {"D--", "    beginner"},
    {"D-",  " still learning"},
    {"D",   "    nice try"},
    {"D+",  "  good effort"},     // хорошее усилие
    {"D++", "  keep rocking"},    // продолжай зажигать
 
    {"C--", "  getting good"},
    {"C-",  "   solid run"},       // отличная игра
    {"C",   "   nice job"},
    {"C+",  "   impressive"},
    {"C++", "  great player"},

    {"B--", "   rock on"},         // зажигаем
    {"B-",  "   very good"},
    {"B",   "    skilled"},         // квалифицированно
    {"B+",  "    awesome"},
    {"B++", "   stage ready"},     // готовы к выступлению

    {"A--", "    amazing"},
    {"A-",  "   superstar"},
    {"A",   "   rock hero"},
    {"A+",  "   legendary"},
    {"A++", "  born to rock"},

    {"S",   "you are rock star"}
};

const int GRADE_COUNT = sizeof(grade_table) / sizeof(grade_table[0]);

uint8_t get_grade_index(uint32_t score)
{
    const uint32_t MAX_SCORE = 1910;
    if (score >= MAX_SCORE) return GRADE_COUNT - 1; // что бы значение не выходило за максимум (выводим S)
    float percent = (float)score / MAX_SCORE; // переводим число в %
    return (uint8_t)(percent * (GRADE_COUNT - 1)); // перевод % в индекс оценки
}

void game_over_screen(Mikboy mik)
{
    uint8_t idx = get_grade_index(hits);
    if(idx >= GRADE_COUNT) idx = GRADE_COUNT - 1;

    mik.lcd().print_word(95, 30, grade_table[idx].grade, 0xFEE0, 3);
    mik.lcd().print_word(35, 10, grade_table[idx].message, 0x6900, 1);
    game_over= 0;

    mik.lcd().print_word(10, 30, "RESULT", 0xFFFF,  2);

    char buf[32];                                       // буфер для текста
    sprintf(buf, "srore: %lu ", hits);             // формируем строку
    mik.lcd().print_word(10, 60, buf, 0xFFFF, 1);         // вывод текста на экран

    sprintf(buf, "mistakes: %lu", mistakes_arrows);             // формируем строку
    mik.lcd().print_word(10, 75, buf, 0xFFFF, 1);         // вывод текста на экран

    sprintf(buf, "hits: %lu / 191", hit_arrows);             // формируем строку
    mik.lcd().print_word(10, 90, buf, 0xFFFF, 1);         // вывод текста на экран

    mik.lcd().print_word(33, 112, "press A to restart", 0x6900, 1);
}

void arrow_animation(Mikboy mik)
{
    draw_score(mik);        // вывод счёта
    draw_progress_bar(mik); // вывод продолжительности трека
    update_chart();         // выбор стрелочки из Note chart[]  
    update_arrows(mik);     // обновляем позиции (для полёт стрелочки)
    handle_input(mik);      // проверяем кнопки
    draw_arrows(mik);       // рисуем стрелочки
    draw_hit_zone(mik);     // рисуем зону попадания
}

uint32_t game_time()
{
    return HAL_Millis() - game_start_time;
}
void restart_game(Mikboy mik)
{
    mik.lcd().gradient(0, 0, 160, 128, BG_UP, BG_DOWN); // рисуем фон
    mik.lcd().draw_fill_rect(0, 0, 160, 19, 0x99E6);    // закрашиваем шапочку  
    game_start_time = HAL_Millis();                     // старт отсчёта игры

    hits = 0;
    hit_arrows = 0;
    mistakes_arrows = 0;
    time_offset = 350;
    score_changed = true;

    current_note = 0;      // индекс текущей стрелки
    game_over = 1;

    draw_score(mik);                                    // вывод счёта
}