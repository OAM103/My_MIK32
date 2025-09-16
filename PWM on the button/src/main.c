#include "mik32_hal_timer32.h"
#include "uart_lib.h"
#include "xprintf.h"

#define PIN_BUTTON   8   // кнопка на GPIO_0

TIMER32_HandleTypeDef htimer32_1;
TIMER32_CHANNEL_HandleTypeDef htimer32_channel3; // Канал для ШИМ

void SystemClock_Config(void);
static void Timer32_1_PWM_Init(void);
void GPIO_Init(void);

//Функция измерения нажатия
// uint32_t get_button_press_time() {
//     uint32_t t = 0; 
//     while (!(GPIO_0->STATE & (1 << PIN_BUTTON))){ 
//         if (__HAL_SCR1_TIMER_GET_TIME() >= TIME/4)
//         {
//             t+=1; // четверть секунды
//             __HAL_SCR1_TIMER_SET_TIME(0);           // Сбросить счетчик системного таймера.   
//         }
//     }
//     return t;
// }

int main()
{
    SystemClock_Config();
    GPIO_Init();
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    Timer32_1_PWM_Init(); // Инициализация таймера для ШИМ

    HAL_Timer32_Channel_Enable(&htimer32_channel3);
    HAL_Timer32_Value_Clear(&htimer32_1);
    HAL_Timer32_Start(&htimer32_1);
    HAL_Timer32_Channel_OCR_Set(&htimer32_channel3, htimer32_1.Top);//изначально светодиод не горит

    // большой OCR - слабый свет
    // малый OCR - сильный свет
    int32_t L = htimer32_1.Top;

    while (1)
    {
        // Изменение яркости (скважности) от 0 до 100% при нажатой кнопке
        if(!(GPIO_0->STATE & (1 << PIN_BUTTON)) && L >= 0){ 
            // Задержка для видимого изменения яркости
            HAL_Timer32_Channel_OCR_Set(&htimer32_channel3, (uint32_t)L);
            L -= 500;
            HAL_DelayMs(100);
        }
        else if((GPIO_0->STATE & (1 << PIN_BUTTON)) && L <= (int32_t)htimer32_1.Top){
            HAL_Timer32_Channel_OCR_Set(&htimer32_channel3, (uint32_t)L);
            L += 500;
            HAL_DelayMs(100);
        } 

    }
}

void SystemClock_Config(void)
{
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

static void Timer32_1_PWM_Init(void)
{
    // Настройка таймера
    htimer32_1.Instance = TIMER32_1;
    htimer32_1.Top = 32000 - 1; // Частота ШИМ ~1 кГц при 32 МГц
    htimer32_1.State = TIMER32_STATE_DISABLE;
    htimer32_1.Clock.Source = TIMER32_SOURCE_PRESCALER;
    htimer32_1.Clock.Prescaler = 0;
    htimer32_1.CountMode = TIMER32_COUNTMODE_FORWARD;
    htimer32_1.InterruptMask = 0;
    HAL_Timer32_Init(&htimer32_1);

    // Настройка канала 1 для ШИМ на Port0_3
    htimer32_channel3.TimerInstance = htimer32_1.Instance;
    htimer32_channel3.ChannelIndex = TIMER32_CHANNEL_3;
    htimer32_channel3.Mode = TIMER32_CHANNEL_MODE_PWM;
    htimer32_channel3.PWM_Invert = TIMER32_CHANNEL_INVERTED_PWM; // инвертируемый ШИМ
    htimer32_channel3.OCR = htimer32_1.Top / 2; // Скважность 50%
    htimer32_channel3.Noise = TIMER32_CHANNEL_FILTER_OFF;
    HAL_Timer32_Channel_Init(&htimer32_channel3);

}

void GPIO_Init(void)
{
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();

}