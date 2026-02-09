#include "mik32_hal_pcc.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal.h"
#include "mik32_hal_timer32.h"
#include "mik32_hal_dma.h"
#include "mik32_hal_dac.h"
#include "setting.h"

TIMER32_HandleTypeDef htimer32;
TIMER32_CHANNEL_HandleTypeDef htimer32_channel;
DMA_InitTypeDef hdma;
DMA_ChannelHandleTypeDef hdma_ch0;
DMA_ChannelHandleTypeDef hdma_ch1;
DAC_HandleTypeDef hdac1;
DAC_HandleTypeDef hdac2;

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
    PCC_OscInit.LSI32KCalibrationValue = 128;
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_AUTO;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&PCC_OscInit);
}

void GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();
    __HAL_PCC_GPIO_IRQ_CLK_ENABLE();

    GPIO_InitStruct.Pin = SLAVE_CS0 | SLAVE_CS1 | SLAVE_CS2;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_CS, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = POT0_INC | POT0_UP_DOWN;
    HAL_GPIO_Init(GPIO_POT0, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = ENC_A | ENC_B;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_INPUT;
    HAL_GPIO_Init(GPIO_ENC, &GPIO_InitStruct);

    HAL_GPIO_InitInterruptLine(GPIO_MUX_PORT1_4_LINE_0, GPIO_INT_MODE_FALLING); // ENC_A
    HAL_GPIO_InitInterruptLine(GPIO_MUX_PORT1_6_LINE_2, GPIO_INT_MODE_FALLING); // ENC_B
    HAL_GPIO_InitInterruptLine(GPIO_MUX_PORT0_14_LINE_6, GPIO_INT_MODE_CHANGE); // ENC_But

    
}

// void GPIO_Init()
// {
//     GPIO_InitTypeDef GPIO_InitStruct = {0};

//     __HAL_PCC_GPIO_0_CLK_ENABLE();
//     __HAL_PCC_GPIO_1_CLK_ENABLE();
//     __HAL_PCC_GPIO_2_CLK_ENABLE();
//     __HAL_PCC_GPIO_IRQ_CLK_ENABLE();

//     GPIO_InitStruct.Pin = SLAVE_CS0 | SLAVE_CS1 | SLAVE_CS2;
//     GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
//     GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
//     HAL_GPIO_Init(GPIO_CS, &GPIO_InitStruct);

//     GPIO_InitStruct.Pin = ENC_A | ENC_B;
//     GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_INPUT;
//     HAL_GPIO_Init(GPIO_ENC, &GPIO_InitStruct);

//     HAL_GPIO_InitInterruptLine(GPIO_MUX_PORT1_4_LINE_0, GPIO_INT_MODE_FALLING); // ENC_A
//     HAL_GPIO_InitInterruptLine(GPIO_MUX_PORT1_6_LINE_2, GPIO_INT_MODE_FALLING); // ENC_B
//     HAL_GPIO_InitInterruptLine(GPIO_MUX_PORT0_14_LINE_6, GPIO_INT_MODE_CHANGE); // ENC_But
// }

void Timer32_Init(void)
{
    htimer32.Instance = TIMER32_1;
    //htimer32.Top = 320;
    uint32_t sr = (32000*100)/100; //100-норма
    htimer32.Top = TIMER_CLK / sr;
    htimer32.State = TIMER32_STATE_DISABLE;
    htimer32.Clock.Source = TIMER32_SOURCE_PRESCALER;
    htimer32.Clock.Prescaler = 0;
    htimer32.InterruptMask = 0;
    htimer32.CountMode = TIMER32_COUNTMODE_FORWARD;
    HAL_Timer32_Init(&htimer32);

    htimer32_channel.TimerInstance = htimer32.Instance;
    htimer32_channel.ChannelIndex = TIMER32_CHANNEL_0;
    htimer32_channel.PWM_Invert = TIMER32_CHANNEL_NON_INVERTED_PWM;
    htimer32_channel.Mode = TIMER32_CHANNEL_MODE_COMPARE;
    htimer32_channel.CaptureEdge = TIMER32_CHANNEL_CAPTUREEDGE_RISING;
    htimer32_channel.OCR = htimer32.Top / 2;
    htimer32_channel.Noise = TIMER32_CHANNEL_FILTER_OFF;
    HAL_Timer32_Channel_Init(&htimer32_channel);
}

void DMA_CH0_Init(DMA_InitTypeDef *hdma)
{
    hdma_ch0.dma = hdma;

    /* Настройки канала */
    hdma_ch0.ChannelInit.Channel = DMA_CHANNEL_0;
    hdma_ch0.ChannelInit.Priority = DMA_CHANNEL_PRIORITY_VERY_HIGH;

    hdma_ch0.ChannelInit.ReadMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_ch0.ChannelInit.ReadInc = DMA_CHANNEL_INC_ENABLE;
    hdma_ch0.ChannelInit.ReadSize = DMA_CHANNEL_SIZE_WORD; /* data_len должно быть кратно read_size */
    hdma_ch0.ChannelInit.ReadBurstSize = 2;                /* read_burst_size должно быть кратно read_size */
    hdma_ch0.ChannelInit.ReadRequest = DMA_CHANNEL_TIMER32_1_REQUEST;
    hdma_ch0.ChannelInit.ReadAck = DMA_CHANNEL_ACK_DISABLE;

    hdma_ch0.ChannelInit.WriteMode = DMA_CHANNEL_MODE_PERIPHERY;
    hdma_ch0.ChannelInit.WriteInc = DMA_CHANNEL_INC_DISABLE;
    hdma_ch0.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_WORD; /* data_len должно быть кратно write_size */
    hdma_ch0.ChannelInit.WriteBurstSize = 2;                /* write_burst_size должно быть кратно read_size */
    hdma_ch0.ChannelInit.WriteRequest = DMA_CHANNEL_TIMER32_1_REQUEST;
    hdma_ch0.ChannelInit.WriteAck = DMA_CHANNEL_ACK_ENABLE;
}

void DMA_CH1_Init(DMA_InitTypeDef *hdma)
{
    hdma_ch1.dma = hdma;

    /* Настройки канала */
    hdma_ch1.ChannelInit.Channel = DMA_CHANNEL_1;
    hdma_ch1.ChannelInit.Priority = DMA_CHANNEL_PRIORITY_VERY_HIGH;

    hdma_ch1.ChannelInit.ReadMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_ch1.ChannelInit.ReadInc = DMA_CHANNEL_INC_ENABLE;
    hdma_ch1.ChannelInit.ReadSize = DMA_CHANNEL_SIZE_WORD; /* data_len должно быть кратно read_size */
    hdma_ch1.ChannelInit.ReadBurstSize = 2;                /* read_burst_size должно быть кратно read_size */
    hdma_ch1.ChannelInit.ReadRequest = DMA_CHANNEL_TIMER32_1_REQUEST;
    hdma_ch1.ChannelInit.ReadAck = DMA_CHANNEL_ACK_DISABLE;

    hdma_ch1.ChannelInit.WriteMode = DMA_CHANNEL_MODE_PERIPHERY;
    hdma_ch1.ChannelInit.WriteInc = DMA_CHANNEL_INC_DISABLE;
    hdma_ch1.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_WORD; /* data_len должно быть кратно write_size */
    hdma_ch1.ChannelInit.WriteBurstSize = 2;                /* write_burst_size должно быть кратно read_size */
    hdma_ch1.ChannelInit.WriteRequest = DMA_CHANNEL_TIMER32_1_REQUEST;
    hdma_ch1.ChannelInit.WriteAck = DMA_CHANNEL_ACK_ENABLE;
}

void DMA_Init(void)
{

    /* Настройки DMA */
    hdma.Instance = DMA_CONFIG;
    hdma.CurrentValue = DMA_CURRENT_VALUE_ENABLE;
    /* Инициализация канала */
    DMA_CH0_Init(&hdma);
    DMA_CH1_Init(&hdma);
}

void DAC_Init(void)
{
    hdac1.Instance = ANALOG_REG;

    hdac1.Instance_dac = HAL_DAC0;
    hdac1.Init.DIV = 0;
    hdac1.Init.EXTRef = DAC_EXTREF_OFF;    /* Выбор источника опорного напряжения: «1» - внешний; «0» - встроенный */
    hdac1.Init.EXTClb = DAC_EXTCLB_DACREF; /* Выбор источника внешнего опорного напряжения: «1» - внешний вывод; «0» - настраиваемый ОИН */

    HAL_DAC_Init(&hdac1);
}

void DAC2_Init(void)
{
    hdac2.Instance = ANALOG_REG;

    hdac2.Instance_dac = HAL_DAC1;
    hdac2.Init.DIV = 0;
    hdac2.Init.EXTRef = DAC_EXTREF_OFF;    /* Выбор источника опорного напряжения: «1» - внешний; «0» - встроенный */
    hdac2.Init.EXTClb = DAC_EXTCLB_DACREF; /* Выбор источника внешнего опорного напряжения: «1» - внешний вывод; «0» - настраиваемый ОИН */

    HAL_DAC_Init(&hdac2);
}
