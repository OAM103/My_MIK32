#include "Mikboy.hpp"

TIMER32_HandleTypeDef htimer32;
TIMER32_CHANNEL_HandleTypeDef htimer32_channel;
DAC_HandleTypeDef hdac2;

Mikboy::Mikboy()
{
    SystemClock_Config();
    HAL_Time_SCR1TIM_Init();
    SPI0_Init();
    GPIO_init();
    DAC2_Init();
    Timer32_Init();
    lcd_ = LCD_Init();
    lcd_.init(160, 128, P1_4, -1, -1, P1_0);
}

ST7735::LCD Mikboy::LCD_Init()
{
    DMA_ChannelHandleTypeDef hdma_ch_;
    hdma_.Instance = DMA_CONFIG;
    hdma_.CurrentValue = DMA_CURRENT_VALUE_ENABLE;
    
    if (HAL_DMA_Init(&hdma_) != HAL_OK)
    {
        xprintf("DMA_Init Error\n");
    }
    
    hdma_ch_.dma = &hdma_;
    hdma_ch_.ChannelInit.Channel = DMA_CHANNEL_0;
    hdma_ch_.ChannelInit.Priority = DMA_CHANNEL_PRIORITY_VERY_HIGH;
    hdma_ch_.ChannelInit.ReadMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_ch_.ChannelInit.ReadInc = DMA_CHANNEL_INC_ENABLE;
    hdma_ch_.ChannelInit.ReadSize = DMA_CHANNEL_SIZE_BYTE;
    hdma_ch_.ChannelInit.ReadBurstSize = 0;
    hdma_ch_.ChannelInit.ReadRequest = DMA_CHANNEL_SPI_1_REQUEST;
    hdma_ch_.ChannelInit.ReadAck = DMA_CHANNEL_ACK_DISABLE;
    hdma_ch_.ChannelInit.WriteMode = DMA_CHANNEL_MODE_PERIPHERY;
    hdma_ch_.ChannelInit.WriteInc = DMA_CHANNEL_INC_DISABLE;
    hdma_ch_.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_BYTE;
    hdma_ch_.ChannelInit.WriteBurstSize = 0;
    hdma_ch_.ChannelInit.WriteRequest = DMA_CHANNEL_SPI_1_REQUEST;
    hdma_ch_.ChannelInit.WriteAck = DMA_CHANNEL_ACK_DISABLE;
    
    ST7735::LCD lcds(SPI_1, hdma_ch_);
    return lcds;
}

void Mikboy::SPI0_Init()
{
    SPI_HandleTypeDef hspi0;
    hspi0.Instance = SPI_1;
    hspi0.Init.SPI_Mode = HAL_SPI_MODE_MASTER;
    hspi0.Init.CLKPhase = SPI_PHASE_OFF;
    hspi0.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi0.Init.ThresholdTX = 1;
    hspi0.Init.BaudRateDiv = SPI_BAUDRATE_DIV4;
    hspi0.Init.Decoder = SPI_DECODER_NONE;
    hspi0.Init.ManualCS = SPI_MANUALCS_OFF;
    hspi0.Init.ChipSelect = SPI_CS_1;
    HAL_SPI_Init(&hspi0);
    HAL_SPI_Enable(&hspi0);
}

void Mikboy::SystemClock_Config(void)
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

void Mikboy::GPIO_init()
{
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_PAD_CONFIG_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();
    
    for (int i = 0; i < uint8_t(BUTTON_PIN::SIZE); i++)
    {
        ll_gpio_set_mode(uint8_t(button_mas[i]), PIN_AS_GPIO);
        ll_gpio_set_input(uint8_t(button_mas[i]));
    }
}

void Mikboy::DAC2_Init(void)
{
    hdac2.Instance = ANALOG_REG;

    hdac2.Instance_dac = HAL_DAC1;
    hdac2.Init.DIV = 0;
    hdac2.Init.EXTRef = DAC_EXTREF_OFF;    /* Выбор источника опорного напряжения: «1» - внешний; «0» - встроенный */
    hdac2.Init.EXTClb = DAC_EXTCLB_DACREF; /* Выбор источника внешнего опорного напряжения: «1» - внешний вывод; «0» - настраиваемый ОИН */

    HAL_DAC_Init(&hdac2);
}
#define TIMER_CLK 64000000UL  // 64 МГц 
#define SAMPLE_RATE 64000

void Mikboy::Timer32_Init(void)
{
    htimer32.Instance = TIMER32_1;
    // htimer32.Top =  5000;
    htimer32.Top = TIMER_CLK / SAMPLE_RATE; 
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

