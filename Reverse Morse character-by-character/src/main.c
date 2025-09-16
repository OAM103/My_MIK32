#include "mik32_hal_gpio.h"
#include "mik32_hal_usart.h"
#include "mik32_hal_pcc.h"
#include "mik32_hal_scr1_timer.h"
#include "xprintf.h"
#include <stdint.h>
#include <string.h>

#define PIN_BUTTON 8    // Кнопка на GPIO_0
#define TIME   32000000  // граница времени в мс
#define SYMBOL_GAP 100  // пауза между символами
#define WORD_GAP   3000 // пауза между словами

typedef struct {
    char ch;
    unsigned char code; // битовая последовательность: 0 = точка, 1 = тире
    unsigned char len;  // длина последовательности
} MorseCode;

void SystemClock_Config();
void GPIO_Init();
// Таблица Морзе для a-z
const MorseCode morse_table[] = {
    {'a', 0b01, 2},   {'b', 0b1000, 4}, {'c', 0b1010, 4}, {'d', 0b100, 3},
    {'e', 0b0, 1},    {'f', 0b0010, 4}, {'g', 0b110, 3},  {'h', 0b0000, 4},
    {'i', 0b00, 2},   {'j', 0b0111, 4}, {'k', 0b101, 3},  {'l', 0b0100, 4},
    {'m', 0b11, 2},   {'n', 0b10, 2},   {'o', 0b111, 3},  {'p', 0b0110, 4},
    {'q', 0b1101, 4}, {'r', 0b010, 3},  {'s', 0b000, 3},  {'t', 0b1, 1},
    {'u', 0b001, 3},  {'v', 0b0001, 4}, {'w', 0b011, 3},  {'x', 0b1001, 4},
    {'y', 0b1011, 4}, {'z', 0b1100, 4},
};



// Распознаем символ по буферу точек/тире

char decode_symbol(unsigned char code, unsigned char len) {
    for (int i = 0; i < sizeof(morse_table) / sizeof(morse_table[0]); i++) {
        if (morse_table[i].len == len && morse_table[i].code == code) return morse_table[i].ch;
    }
}

//Функция измерения нажатия

uint32_t get_button_press_time() {
    uint32_t t = 0; 
    while (!(GPIO_0->STATE & (1 << PIN_BUTTON))){ 
        if (__HAL_SCR1_TIMER_GET_TIME() >= TIME/4)
        {
            t+=1; // четверть секунды
            __HAL_SCR1_TIMER_SET_TIME(0);           // Сбросить счетчик системного таймера.   
        }
    }
    return t;
}



int main() {
    SystemClock_Config();
    GPIO_Init();
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M | UART_CONTROL1_RE_M, 0, 0);
    HAL_SCR1_Timer_Init(HAL_SCR1_TIMER_CLKSRC_INTERNAL, 0);

    unsigned char symbol_buffer = 0;
    unsigned char symbol_len = 0;

    xprintf("Start...\n");

    while (1) {
        uint32_t press_time = get_button_press_time();

        // if (!(GPIO_0->STATE & (1 << 8))) xprintf("OK"); //условие, при котором кнопкa выводит слово

        //определяем точку или тире
        if (press_time < 5 & press_time>1) {
            symbol_buffer = (symbol_buffer << 1) | 0; // точка 
            xprintf(".");
        }
        else if(press_time>5){
            symbol_buffer = (symbol_buffer << 1) | 1; // тире
            xprintf("-");   
 
        }
        else continue;

        symbol_len++;

        // ждем паузу между символами

        uint32_t pause = 0;
        while (GPIO_0->STATE & (1 << PIN_BUTTON)) {
            if (__HAL_SCR1_TIMER_GET_TIME() >= TIME/4)
            {
                pause+=1; // четверть секунды
                __HAL_SCR1_TIMER_SET_TIME(0);           // Сбросить счетчик системного таймера.  
            }
            if (pause >= 12 & symbol_buffer!=0 & symbol_len!=0){
                char decoded = decode_symbol(symbol_buffer, symbol_len);
                xprintf("   %c\n", decoded); 
                // сброс буфера для следующего символа
                symbol_buffer = 0;
                symbol_len = 0;
                pause = 0;
            }
        }
        
        
        // если пауза > SYMBOL_GAP, символ завершён
        

        // // если пауза > WORD_GAP, добавляем пробел
        // if (pause >= WORD_GAP) xprintf(" ");
    }
}


void SystemClock_Config() {
    PCC_InitTypeDef PCC_OscInit = {0};
    PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_ALL;
    PCC_OscInit.FreqMon.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;
    PCC_OscInit.FreqMon.ForceOscSys = PCC_FORCE_OSC_SYS_UNFIXED;
    PCC_OscInit.FreqMon.Force32KClk = PCC_FREQ_MONITOR_SOURCE_OSC32K;
    PCC_OscInit.AHBDivider = 0;
    PCC_OscInit.APBMDivider = 0;
    PCC_OscInit.APBPDivider = 0;
    PCC_OscInit.HSI32MCalibrationValue = 128;
    PCC_OscInit.LSI32KCalibrationValue = 8;
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_AUTO;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&PCC_OscInit);
}

void GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0}; // структура для конфигурации пина
    __HAL_PCC_GPIO_0_CLK_ENABLE(); //тактирование блока GPIO_0 для пинов
    GPIO_InitStruct.Pin = GPIO_PIN_3;//выбираем 3 пин
    
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);//GPIO_0 на выход

}
