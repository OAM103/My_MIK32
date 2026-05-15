#include "Mikboy.hpp"


Mikboy::Mikboy()
{
    SystemClock_Config();
    HAL_Time_SCR1TIM_Init();
    SPI0_Init();
    GPIO_init();
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

