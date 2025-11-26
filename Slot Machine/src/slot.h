#ifndef SLOT_H
#define SLOT_H

typedef struct {
    int img;         // текущая картинка
    int down;        // сдвиг вниз текущий
    int old_down;    // сдвиг вниз старый
    int speed;       // текущая скорость
    int spins;       // оставшиеся картинки
    int active;      // 0-стоп, 1-крутится
} Reel;

void Spin3Slots(uint16_t BG_COLOR);
void InitReels(uint16_t X);

#endif