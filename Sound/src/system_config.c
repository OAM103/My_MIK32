#include "system_config.h"
#include "mik32_hal.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_spi.h"
#include "mik32_hal_dma.h"
#include "xprintf.h"
#include "mik32_hal_timer32.h"
#include "mik32_hal_dac.h"

SPI_HandleTypeDef hspi0;
SPI_HandleTypeDef hspi1;
DMA_InitTypeDef hdma;
DMA_ChannelHandleTypeDef hdma_ch0;
TIMER32_HandleTypeDef htimer32;
TIMER32_CHANNEL_HandleTypeDef htimer32_channel;
DAC_HandleTypeDef hdac1;

// Два буфера для построчной двойной буферизации
uint16_t lineBuffer1[TFT_WIDTH];
uint16_t lineBuffer2[TFT_WIDTH];

uint8_t* DMA_BUF; // Статический буфер


void set_dma_buf(uint8_t* MASS){
    DMA_BUF = MASS;
}

void GPIO_Init() {
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();
    __HAL_PCC_GPIO_IRQ_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = CS;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    GPIO_InitStruct.DS = HAL_GPIO_DS_8MA;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = RS;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = RESET;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = LED;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = SLAVE_CS0 | SLAVE_CS1 | SLAVE_CS2;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    if (HAL_GPIO_Init(GPIO_CS, &GPIO_InitStruct) != HAL_OK)
    {
        xprintf("SPI_Init_Error\n");
    }
    HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS_ALL, 0); // Выбрать клавиатуру
    HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS1, 1); // Выбрать клавиатуру
    //HAL_GPIO_WritePin(GPIO_0, CS , GPIO_PIN_HIGH);
    //HAL_GPIO_WritePin(GPIO_0, RS , 1);
    HAL_GPIO_WritePin(GPIO_0, RESET , 1);
    HAL_GPIO_WritePin(GPIO_0, LED , 1);
}


void SystemClock_Config(void) {
    PCC_InitTypeDef PCC_OscInit = {0};

    PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_ALL;
    PCC_OscInit.FreqMon.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;
    PCC_OscInit.FreqMon.ForceOscSys = PCC_FORCE_OSC_SYS_UNFIXED;
    PCC_OscInit.FreqMon.Force32KClk = PCC_FREQ_MONITOR_SOURCE_OSC32K;
    PCC_OscInit.AHBDivider = 0;
    PCC_OscInit.APBMDivider = 0;
    PCC_OscInit.APBPDivider = 0;
    PCC_OscInit.HSI32MCalibrationValue = 125;
    PCC_OscInit.LSI32KCalibrationValue = 8;
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_AUTO;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&PCC_OscInit);
}

void SPI0_Init(void) {
    hspi0.Instance = SPI_0;

    /* Режим SPI */
    hspi0.Init.SPI_Mode = HAL_SPI_MODE_MASTER;

    /* Настройки */
    hspi0.Init.CLKPhase = SPI_PHASE_ON;
    hspi0.Init.CLKPolarity = SPI_POLARITY_HIGH;
    hspi0.Init.ThresholdTX = 4;
    //hspi0.Init.ThresholdTX = SPI_THRESHOLD_DEFAULT;
    //hspi0.Init.CLKPolarity = SPI_POLARITY_LOW;

    /* Настройки для ведущего */
    hspi0.Init.BaudRateDiv = 0;
    hspi0.Init.Decoder = SPI_DECODER_NONE;
    hspi0.Init.ManualCS = SPI_MANUALCS_OFF;
    hspi0.Init.ChipSelect = SPI_CS_0;

    if (HAL_SPI_Init(&hspi0) != HAL_OK) {
        xprintf("SPI_Init_Error\n");
    }
}
// static void SPI0_Init(void)
// {
// hspi0.Instance = SPI_0;

//     /* Режим SPI */
//     hspi0.Init.SPI_Mode = HAL_SPI_MODE_MASTER;

//     /* Настройки */
//     hspi0.Init.CLKPhase = SPI_PHASE_ON;
//     hspi0.Init.CLKPolarity = SPI_POLARITY_HIGH;
//     hspi0.Init.ThresholdTX = 4;

//     /* Настройки для ведущего */
//     hspi0.Init.BaudRateDiv = SPI_BAUDRATE_DIV16;
//     hspi0.Init.Decoder = SPI_DECODER_NONE;
//     hspi0.Init.ManualCS = SPI_MANUALCS_OFF;
//     hspi0.Init.ChipSelect = SPI_CS_0;      

//     if (HAL_SPI_Init(&hspi0) != HAL_OK)
//     {
//         xprintf("SPI_Init_Error\n");
//     }
// }
void SPI1_Init(void)
{
    hspi1.Instance = SPI_1;

    /* Режим SPI */
    hspi1.Init.SPI_Mode = HAL_SPI_MODE_MASTER;

    /* Настройки */
    hspi1.Init.CLKPhase = SPI_PHASE_ON;
    hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
    hspi1.Init.ThresholdTX = 4;

    /* Настройки для ведущего */
    hspi1.Init.BaudRateDiv = SPI_BAUDRATE_DIV8;
    hspi1.Init.Decoder = SPI_DECODER_NONE;
    hspi1.Init.ManualCS = SPI_MANUALCS_ON;
    hspi1.Init.ChipSelect = SPI_CS_0;      

    if (HAL_SPI_Init(&hspi1) != HAL_OK)
    {
        xprintf("SPI_Init_Error\n");
    }
}



void DMA_Init(void)
{
    /* Настройки DMA */
    hdma.Instance = DMA_CONFIG;
    hdma.CurrentValue = DMA_CURRENT_VALUE_ENABLE;
    if (HAL_DMA_Init(&hdma) != HAL_OK)
    {
        xprintf("DMA_Init Error\n");
    }

    /* Инициализация канала */
    DMA_CH0_Init(&hdma);
}

static void DMA_CH0_Init(DMA_InitTypeDef *hdma)
{
    hdma_ch0.dma = hdma;
    /* Настройки канала */
    hdma_ch0.ChannelInit.Channel = DMA_CHANNEL_0;
    hdma_ch0.ChannelInit.Priority = DMA_CHANNEL_PRIORITY_VERY_HIGH;
    hdma_ch0.ChannelInit.ReadMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_ch0.ChannelInit.ReadInc = DMA_CHANNEL_INC_ENABLE;
    hdma_ch0.ChannelInit.ReadSize = DMA_CHANNEL_SIZE_BYTE; /* data_len должно быть кратно read_size */
    hdma_ch0.ChannelInit.ReadBurstSize = 0;                /* read_burst_size должно быть кратно read_size */
    hdma_ch0.ChannelInit.ReadRequest = DMA_CHANNEL_SPI_0_REQUEST;
    hdma_ch0.ChannelInit.ReadAck = DMA_CHANNEL_ACK_DISABLE;
    hdma_ch0.ChannelInit.WriteMode = DMA_CHANNEL_MODE_PERIPHERY;
    hdma_ch0.ChannelInit.WriteInc = DMA_CHANNEL_INC_DISABLE;
    hdma_ch0.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_BYTE; /* data_len должно быть кратно write_size */
    hdma_ch0.ChannelInit.WriteRequest = DMA_CHANNEL_SPI_0_REQUEST;
    hdma_ch0.ChannelInit.WriteAck = DMA_CHANNEL_ACK_DISABLE;
}


void Timer32_Init(void)
{
    htimer32.Instance = TIMER32_1;
    htimer32.Top = TIMER_CLK / 32000; 
    htimer32.Clock.Source = TIMER32_SOURCE_PRESCALER;
    htimer32.Clock.Prescaler = 0;
    htimer32.InterruptMask = TIMER32_INT_OC_M(TIMER32_CHANNEL_0);
    htimer32.CountMode = TIMER32_COUNTMODE_FORWARD;
    if (HAL_Timer32_Init(&htimer32) != HAL_OK)
    {
        xprintf("Timer32_Init error\n");
    }
    htimer32_channel.TimerInstance = htimer32.Instance;
    htimer32_channel.ChannelIndex = TIMER32_CHANNEL_0;
    htimer32_channel.Mode = TIMER32_CHANNEL_MODE_COMPARE;
    htimer32_channel.OCR = htimer32.Top / 2;
    htimer32_channel.PWM_Invert = TIMER32_CHANNEL_NON_INVERTED_PWM;
    htimer32_channel.CaptureEdge = TIMER32_CHANNEL_CAPTUREEDGE_RISING;
    htimer32_channel.Noise = TIMER32_CHANNEL_FILTER_OFF;
    if (HAL_Timer32_Channel_Init(&htimer32_channel) != HAL_OK)
    {
        xprintf("Timer32_Channel_Init error\n");
    }
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