#include "cube.h"
#include "joystick.h"
#include "lcd.h"

Vec3 cube[] = {
    {-1, -1, -1},
    { 1, -1, -1},
    { 1,  1, -1},
    {-1,  1, -1},
    {-1, -1,  1},
    { 1, -1,  1},
    { 1,  1,  1},
    {-1,  1,  1}
};

uint8_t edges[][2] = {
    {0,1},{1,2},{2,3},{3,0},
    {4,5},{5,6},{6,7},{7,4},
    {0,4},{1,5},{2,6},{3,7}
};

Vec2 project(Vec3 v)//проектирование куба
{
    int scale = 150; //размер куба
    int px = (v.x / (v.z + 3)) * scale + 230; //начальный х
    int py = (v.y / (v.z + 3)) * scale + 480 / 3; //начальный у
    Vec2 res = {px, py};
    return res;
}

void draw_cube(uint16_t BG_COLOR, uint16_t FG_COLOR)
{
    static Vec2 last_projected[8];//для старого куба
    static int flag = 0;// флаг для инициализации нового куба

    if (!cube_update && flag) return;

    Vec2 projected[8]; 

    float cosX = cosf(angle_x);
    float sinX = sinf(angle_x);
    float cosY = cosf(angle_y);
    float sinY = sinf(angle_y);

    // Если куб уже был нарисован — стираем старый
    if (flag) {
        for (int i = 0; i < 12; i++) {
            int a = edges[i][0];
            int b = edges[i][1];
            draw_line(last_projected[a].x, last_projected[a].y,
                      last_projected[b].x, last_projected[b].y,
                      BG_COLOR);
        }
    }

    // Считаем новые вершины
    for (int i = 0; i < 8; i++) {
        Vec3 v = cube[i];

        // вращение по Y
        float x1 = v.x * cosY - v.z * sinY;
        float z1 = v.x * sinY + v.z * cosY;
        v.x = x1; v.z = z1;

        // вращение по X
        float y1 = v.y * cosX - v.z * sinX;
        z1 = v.y * sinX + v.z * cosX;
        v.y = y1; v.z = z1;

        projected[i] = project(v);
    }

    // Рисуем новый куб
    for (int i = 0; i < 12; i++) {
        int a = edges[i][0];
        int b = edges[i][1];
        draw_line(projected[a].x, projected[a].y,
                  projected[b].x, projected[b].y,
                  FG_COLOR);
    }

    for (int i = 0; i < 8; i++)
        last_projected[i] = projected[i];

    flag = 1;
}
