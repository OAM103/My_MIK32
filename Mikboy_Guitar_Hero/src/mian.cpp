#include "Mikboy.hpp"
#include "Music.hpp"
#include "Game.hpp"
#include "png.hpp"
#include <stdlib.h>
#include "mik32_hal_usart.h"

// инициализируем функции Millis от таймера ядра
#define SYSTEM_TIME_SCR1_TIMER
#if defined SYSTEM_TIME_SCR1_TIMER
uint32_t HAL_Micros()
{
    return HAL_Time_SCR1TIM_Micros();
}
uint32_t HAL_Millis()
{
    return HAL_Time_SCR1TIM_Millis();
}
void HAL_DelayUs(uint32_t time_us)
{
    HAL_Time_SCR1TIM_DelayUs(time_us);
}
void HAL_DelayMs(uint32_t time_ms)
{
    HAL_Time_SCR1TIM_DelayMs(time_ms);
}
#endif

volatile uint8_t need_fill = 0;
volatile uint16_t buf_pos = 0;

int main()
{
    Mikboy mik;
    HAL_Init();
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    ll_gpio_set_mode(P1_12,  PIN_AS_GPIO);
    ll_gpio_out_write(P1_12, 1);

    HAL_Timer32_Value_Clear(&htimer32);
    HAL_EPIC_MaskLevelSet(HAL_EPIC_TIMER32_1_MASK);
    HAL_IRQ_EnableInterrupts();
    HAL_Timer32_Compare_Start_IT(&htimer32, &htimer32_channel);

    fill_dac_buf_start(dac_buf0);
    fill_dac_buf_start(dac_buf1);

    mik.lcd().led_on();
    mik.lcd().gradient(0, 0, 160, 128, BG_UP, BG_DOWN); // рисуем фон

    game_state = GameState::START;
    start_screen(mik);

    while (1)
    {
        switch (game_state)
        {
            case GameState::START:
                if (need_fill)
                {
                    need_fill = 0;

                    if (active_buf == 0) fill_dac_buf_start(dac_buf1);
                    else fill_dac_buf_start(dac_buf0);
                }
                if (!ll_gpio_read(int8_t(BUTTON_PIN::A)))
                {
                    mik.lcd().gradient(0, 0, 160, 128, BG_UP, BG_DOWN); // рисуем фон
                    mik.lcd().draw_fill_rect(0, 0, 160, 19, 0x99E6);    // закрашиваем шапочку  
                    game_start_time = HAL_Millis();                     // старт отсчёта игры
                    draw_score(mik);                                    // вывод счёта
                    game_state = GameState::COUNTDOWN;                    // начинаем игру
                }
                break;
            case GameState::COUNTDOWN:
                if (need_fill)
                {
                    need_fill = 0;

                    if (active_buf == 0) fill_dac_buf(dac_buf1);
                    else fill_dac_buf(dac_buf0);
                }
                draw_countdown(mik); //вывод стартовых надписей
                break;

            case GameState::RUNNING:
                if (need_fill)
                {
                    need_fill = 0;

                    if (active_buf == 0) fill_dac_buf(dac_buf1);
                    else fill_dac_buf(dac_buf0);
                }
                arrow_animation(mik);
                break;

            case GameState::GAME_OVER:
                if (game_over) {
                    mik.lcd().gradient(0, 0, 160, 128, BG_UP, BG_DOWN); // рисуем фон
                    game_over_screen(mik);
                }
                if (need_fill)
                {
                    need_fill = 0;

                    if (active_buf == 0) fill_dac_buf_start(dac_buf1);
                    else fill_dac_buf_start(dac_buf0);
                }
                if (!ll_gpio_read(int8_t(BUTTON_PIN::A)))
                {
                    restart_game(mik);  
                    game_state = GameState::COUNTDOWN;  // начинаем игру
                }
                break;
        }
    }
}

extern "C" void trap_handler()
{
    if (EPIC_CHECK_TIMER32_1())
    {
        uint32_t status = HAL_Timer32_InterruptFlags_Get(&htimer32);

        if (status & TIMER32_INT_OC_M(TIMER32_CHANNEL_0))
        {
            uint16_t *current_buf = (active_buf == 0) ? dac_buf0 : dac_buf1;
            hdac2.Instance_dac->VALUE = current_buf[buf_pos];
            buf_pos++;

            if (buf_pos >= DAC_BUF_SIZE)
            {
                buf_pos = 0;
                active_buf ^= 1;   // переключаем буфер
                need_fill = 1;     // просим main заполнить
            }
        }

        HAL_TIMER32_INTERRUPTFLAGS_CLEAR(&htimer32);
    }
    HAL_EPIC_Clear(0xFFFFFFFF);
}