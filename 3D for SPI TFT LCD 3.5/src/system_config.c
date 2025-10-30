#include "system_config.h"
#include "mik32_hal.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_spi.h"
#include "mik32_hal_dma.h"
#include "mik32_hal_irq.h"
#include "mik32_hal_pcc.h"
#include "xprintf.h"

SPI_HandleTypeDef hspi0;
SPI_HandleTypeDef hspi1;
DMA_InitTypeDef hdma;
DMA_ChannelHandleTypeDef hdma_ch0;


#define CS          GPIO_PIN_4 //p0_4
#define RS          GPIO_PIN_8 //p0_8
#define RESET       GPIO_PIN_9
#define LED         GPIO_PIN_10
#define TFT_WIDTH   480
#define TFT_HEIGHT  320

#define GPIO_CS GPIO_1
#define SLAVE_CS0 GPIO_PIN_10 //GPIO_PIN_10
#define SLAVE_CS1 GPIO_PIN_11
#define SLAVE_CS2 GPIO_PIN_14
#define SLAVE_CS_ALL (SLAVE_CS2 | SLAVE_CS1 | SLAVE_CS0)
#define DMA_BUFFER_SIZE (TFT_WIDTH * 3) // Размер буфера для одной строки

// Два буфера для построчной двойной буферизации
uint16_t lineBuffer1[TFT_WIDTH];
uint16_t lineBuffer2[TFT_WIDTH];

uint8_t dma_buffer[DMA_BUFFER_SIZE]; // Статический буфер

void GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();
    __HAL_PCC_GPIO_IRQ_CLK_ENABLE();

    // CS для SPI устройств
    GPIO_InitStruct = (GPIO_InitTypeDef){0};
    GPIO_InitStruct.Pin = SLAVE_CS0 | SLAVE_CS1 | SLAVE_CS2;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_CS, &GPIO_InitStruct);

    // Основные сигналы LCD
    GPIO_InitStruct = (GPIO_InitTypeDef){0};
    GPIO_InitStruct.Pin = CS | RS | RESET | LED;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    GPIO_InitStruct.DS = HAL_GPIO_DS_8MA;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);

    // Кнопка/джойстик
    GPIO_InitStruct = (GPIO_InitTypeDef){0};
    GPIO_InitStruct.Pin = GPIO_PIN_14;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_INPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
    HAL_GPIO_InitInterruptLine(GPIO_MUX_PORT0_14_LINE_6, GPIO_INT_MODE_CHANGE);

    // Начальное состояние 
    HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS_ALL, 0);  
    //HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS1, 1);   
    HAL_GPIO_WritePin(GPIO_0, RESET, 1);
    HAL_GPIO_WritePin(GPIO_0, LED, 1);
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

//     GPIO_InitStruct.Pin = GPIO_PIN_14;
//     GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_INPUT;
//     HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);

//     HAL_GPIO_InitInterruptLine(GPIO_MUX_PORT0_14_LINE_6, GPIO_INT_MODE_CHANGE); // ENC_But

// }

// void GPIO_Init() {
//     __HAL_PCC_GPIO_0_CLK_ENABLE();
//     __HAL_PCC_GPIO_1_CLK_ENABLE();
//     GPIO_InitTypeDef GPIO_InitStruct = {0};
//     GPIO_InitStruct.Pin = CS;
//     GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
//     GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
//     GPIO_InitStruct.DS = HAL_GPIO_DS_8MA;
//     HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
//     GPIO_InitStruct.Pin = RS;
//     HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
//     GPIO_InitStruct.Pin = RESET;
//     HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
//     GPIO_InitStruct.Pin = LED;
//     HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
//     GPIO_InitStruct.Pin = SLAVE_CS0 | SLAVE_CS1 | SLAVE_CS2;
//     GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
//     GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
//     if (HAL_GPIO_Init(GPIO_CS, &GPIO_InitStruct) != HAL_OK)
//     {
//         xprintf("SPI_Init_Error\n");
//     }
//     HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS_ALL, 0); // Выбрать клавиатуру
//     HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS1, 1); // Выбрать клавиатуру
//     //HAL_GPIO_WritePin(GPIO_0, CS , GPIO_PIN_HIGH);
//     //HAL_GPIO_WritePin(GPIO_0, RS , 1);
//     HAL_GPIO_WritePin(GPIO_0, RESET , 1);
//     HAL_GPIO_WritePin(GPIO_0, LED , 1);
// }

// void GPIO_Init(){
//     __HAL_PCC_GPIO_0_CLK_ENABLE();
//     GPIO_InitTypeDef GPIO_InitStruct = {0};
//     GPIO_InitStruct.Pin = CS;
//     GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
//     GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
//     HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
//     GPIO_InitStruct.Pin = RS;
//     HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
//     GPIO_InitStruct.Pin = RESET;
//     HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
//     GPIO_InitStruct.Pin = LED;
//     HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
//     HAL_GPIO_WritePin(GPIO_0, CS , 1);
//     HAL_GPIO_WritePin(GPIO_0, RS , 1);
//     HAL_GPIO_WritePin(GPIO_0, RESET , 1);
//     HAL_GPIO_WritePin(GPIO_0, LED , 1);
// }

void SystemClock_Config(void) {
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

void SPI0_Init(void) {
    hspi0.Instance = SPI_0;

    /* Режим SPI */
    hspi0.Init.SPI_Mode = HAL_SPI_MODE_MASTER;

    /* Настройки */
    hspi0.Init.CLKPhase = SPI_PHASE_ON;
    hspi0.Init.CLKPolarity = SPI_POLARITY_HIGH;
    //hspi0.Init.ThresholdTX = 4;
    hspi0.Init.ThresholdTX = SPI_THRESHOLD_DEFAULT;
    //hspi0.Init.CLKPolarity = SPI_POLARITY_LOW;

    /* Настройки для ведущего */
    hspi0.Init.BaudRateDiv = 0;
    hspi0.Init.Decoder = SPI_DECODER_NONE;
    hspi0.Init.ManualCS = SPI_MANUALCS_ON;
    hspi0.Init.ChipSelect = SPI_CS_0;

    if (HAL_SPI_Init(&hspi0) != HAL_OK) {
        xprintf("SPI_Init_Error\n");
    }
}
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
