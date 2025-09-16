#include "mik32_hal_gpio.h"
#include "mik32_hal_usart.h"
#include "mik32_hal_pcc.h"
#include "mik32_hal_scr1_timer.h"
#include "xprintf.h"
#include <stdint.h>
#include <string.h>

#define PIN_BUTTON   8      // кнопка на GPIO_0
#define TIME         32000000  
#define SPACE_TIME   16     // время для пробела
#define WORD_TIME     20    // пауза между словами
#define MAX_WORD_LEN 32      // макс. длина слова
#define SIMBOL_TIME 8 // время для одной буквы
#define MAX_SENTENCE_LEN 32      // макс. длина пердложения
#define SENTENCE_TIME 32    //пауза для предложения

USART_HandleTypeDef husart0;

typedef struct {
    char ch;
    unsigned char code; // битовая последовательность: 0 = точка, 1 = тире
    unsigned char len;  // длина последовательности
} MorseCode;

// Таблица Морзе (a-z)
const MorseCode morse_table[] = {
    {'a', 0b01, 2},   {'b', 0b1000, 4}, {'c', 0b1010, 4}, {'d', 0b100, 3},
    {'e', 0b0, 1},    {'f', 0b0010, 4}, {'g', 0b110, 3},  {'h', 0b0000, 4},
    {'i', 0b00, 2},   {'j', 0b0111, 4}, {'k', 0b101, 3},  {'l', 0b0100, 4},
    {'m', 0b11, 2},   {'n', 0b10, 2},   {'o', 0b111, 3},  {'p', 0b0110, 4},
    {'q', 0b1101, 4}, {'r', 0b010, 3},  {'s', 0b000, 3},  {'t', 0b1, 1},
    {'u', 0b001, 3},  {'v', 0b0001, 4}, {'w', 0b011, 3},  {'x', 0b1001, 4},
    {'y', 0b1011, 4}, {'z', 0b1100, 4},
};


void SystemClock_Config();
void GPIO_Init();
void USART_Init();

// декодирование символа 
char decode_symbol(unsigned char code, unsigned char len) {
    for (int i = 0; i < sizeof(morse_table) / sizeof(morse_table[0]); i++) {
        if (morse_table[i].len == len && morse_table[i].code == code)
            return morse_table[i].ch;
    }
    return '?'; // если символ не найден
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
    USART_Init();
    HAL_SCR1_Timer_Init(HAL_SCR1_TIMER_CLKSRC_INTERNAL, 0);

    unsigned char symbol_buffer = 0;
    unsigned char symbol_len = 0;

    char word_buffer[MAX_WORD_LEN];
    unsigned char word_len = 0;

    char sentence_buffer[MAX_SENTENCE_LEN][MAX_WORD_LEN];
    unsigned char sentence_len = 0;

    xprintf("Start...\n");

    while (1) {
        uint32_t press_time = get_button_press_time();

        //удаление лишнего символа долгим зажатием кнопки
        if(press_time >= 20 && word_len>0){
            xprintf("X");
            word_len--;
            word_buffer[word_len] = '\0';
        }
        // определяем точку или тире
        else if (press_time > 4 ) {
            symbol_buffer = (symbol_buffer << 1) | 1; // тире
            xprintf("-");
            symbol_len++;
        }
        else if (press_time >= 1) {
            symbol_buffer = (symbol_buffer << 1) | 0; // точка
            xprintf(".");
            symbol_len++;
        }

        
        // кнопка не нажата
        uint32_t pause = 0;
        while (GPIO_0->STATE & (1 << PIN_BUTTON)) {
            if (__HAL_SCR1_TIMER_GET_TIME() >= TIME / 4) { //подсчет времени
                pause += 1;
                __HAL_SCR1_TIMER_SET_TIME(0);
            }
            // символ завершен
            if ((pause >= SIMBOL_TIME) && symbol_len != 0) {
                char decoded = decode_symbol(symbol_buffer, symbol_len);//ищется символ
                xprintf("|"); 
                if (word_len < MAX_WORD_LEN - 1) { // добавляем букву, если есть место
                    word_buffer[word_len++] = decoded;//добавляем букву и увеличиваем счетчик
                    word_buffer[word_len] = '\0';
                }

                symbol_buffer = 0;
                symbol_len = 0;
            }
            //слово завршено
            if(pause >= WORD_TIME && word_len > 0){
                if (sentence_len < MAX_SENTENCE_LEN - 1) strcpy(sentence_buffer[sentence_len++], word_buffer);//записываем слово в предложение
                word_buffer[word_len] = '\0';
                word_len = 0;
                xprintf("  "); 
            }

            // предложение завершено
            if (pause >= SENTENCE_TIME && sentence_len > 0) {
                for(int i = 0; i < sentence_len; i++){
                    xprintf("%s", sentence_buffer[i]);
                    if(i<sentence_len-1) xprintf(" ");
                }
                xprintf("\n");
                for(int i = 0; i < MAX_SENTENCE_LEN; i++) sentence_buffer[i][0] = '\0';
                sentence_len = 0; // очистка буфера
                pause = 0;
            }

        }
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

void USART_Init()
{
    husart0.Instance = UART_0;
    husart0.transmitting = Enable;
    husart0.receiving = Enable;
    husart0.frame = Frame_8bit;
    husart0.parity_bit = Disable;
    husart0.parity_bit_inversion = Disable;
    husart0.bit_direction = LSB_First;
    husart0.data_inversion = Disable;
    husart0.tx_inversion = Disable;
    husart0.rx_inversion = Disable;
    husart0.swap = Disable;
    husart0.lbm = Disable;
    husart0.stop_bit = StopBit_1;
    husart0.mode = Asynchronous_Mode;
    husart0.xck_mode = XCK_Mode3;
    husart0.last_byte_clock = Disable;
    husart0.overwrite = Disable;
    husart0.rts_mode = AlwaysEnable_mode;
    husart0.dma_tx_request = Disable;
    husart0.dma_rx_request = Disable;
    husart0.channel_mode = Duplex_Mode;
    husart0.tx_break_mode = Disable;
    husart0.Interrupt.ctsie = Disable;
    husart0.Interrupt.eie = Disable;
    husart0.Interrupt.idleie = Disable;
    husart0.Interrupt.lbdie = Disable;
    husart0.Interrupt.peie = Disable;
    husart0.Interrupt.rxneie = Disable;
    husart0.Interrupt.tcie = Disable;
    husart0.Interrupt.txeie = Disable;
    husart0.Modem.rts = Disable; //out
    husart0.Modem.cts = Disable; //in
    husart0.Modem.dtr = Disable; //out
    husart0.Modem.dcd = Disable; //in
    husart0.Modem.dsr = Disable; //in
    husart0.Modem.ri = Disable;  //in
    husart0.Modem.ddis = Disable;//out
    husart0.baudrate = 9600;
    HAL_USART_Init(&husart0);
}