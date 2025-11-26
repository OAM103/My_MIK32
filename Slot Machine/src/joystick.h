#ifndef JOYSTICK_H
#define JOYSTICK_H

#define GPIO_BT GPIO_0
#define j_but GPIO_PIN_14

extern uint16_t axisY, axisX;

void Joystick();
void trap_handler();

#endif