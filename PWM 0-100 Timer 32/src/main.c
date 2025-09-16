#include "mik32_hal_timer32.h"
#include "uart_lib.h"
#include "xprintf.h"

TIMER32_HandleTypeDef htimer32_1;
TIMER32_CHANNEL_HandleTypeDef htimer32_channel3; // Канал для ШИМ

void SystemClock_Config(void);
static void Timer32_1_PWM_Init(void);
void GPIO_Init(void);

int main()
{
    SystemClock_Config();
    GPIO_Init();
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    Timer32_1_PWM_Init(); // Инициализация таймера для ШИМ

    HAL_Timer32_Channel_Enable(&htimer32_channel3);
    HAL_Timer32_Value_Clear(&htimer32_1);
    HAL_Timer32_Start(&htimer32_1);

  
    while (1)
    {
        // Пример изменения яркости (скважности) от 0 до 100%
        for (uint32_t duty = 0; duty <= htimer32_1.Top; duty += 1000)
        {

            // Задержка для видимого изменения яркости
            HAL_Timer32_Channel_OCR_Set(&htimer32_channel3, duty);
            HAL_Timer32_Value_Clear(&htimer32_1);
            HAL_DelayMs(100);
        }
        //xprintf("Count: %d\n", HAL_Timer32_Value_Get(&htimer32_1));
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
    htimer32_channel3.PWM_Invert = TIMER32_CHANNEL_NON_INVERTED_PWM; //неинвертируемый шим
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
