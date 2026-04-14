#include "mik32_hal_pcc.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal.h"
#include "mik32_hal_timer32.h"
#include "mik32_hal_dac.h"
#include "mik32_hal_irq.h"

#include "uart_lib.h"
#include "xprintf.h"

#include "system_config.h"
#include "music.h"
#include "tracks.h"
#include "lcd.h"
#include "touchpad.h"

volatile int buf_pos = 0;
volatile int need_fill = 0;

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    uint8_t dma_buffer[DMA_BUFFER_SIZE];
    set_dma_buf(&dma_buffer);

    GPIO_Init();
    SPI0_Init();
    SPI1_Init();
    Lcd_Init();
    DMA_Init();
    DAC_Init();
    Timer32_Init();

    ClearMassDMA_Fast(480, 320, 0, 0, 0x0000);

    fill_dac_buf(dac_buf0);
    fill_dac_buf(dac_buf1);

    HAL_Timer32_Value_Clear(&htimer32);
    HAL_EPIC_MaskLevelSet(HAL_EPIC_TIMER32_1_MASK);
    HAL_IRQ_EnableInterrupts();
    HAL_Timer32_Compare_Start_IT(&htimer32, &htimer32_channel);

    while (1)
    {
        process_touchpad();
        if (need_fill)
        {
            need_fill = 0;
            if (active_buf == 0) fill_dac_buf(dac_buf1);
            else fill_dac_buf(dac_buf0);
        }
        // update_circles();
    }
}

void trap_handler()
{
    if (EPIC_CHECK_TIMER32_1())
    {
        uint32_t status = HAL_Timer32_InterruptFlags_Get(&htimer32);

        if (status & TIMER32_INT_OC_M(TIMER32_CHANNEL_0))
        {
            if (play_sound == 0) { 
                hdac1.Instance_dac->VALUE = DAC_MID; // тишина
            } 
            else {
                uint16_t *current_buf = (active_buf == 0) ? dac_buf0 : dac_buf1;

                hdac1.Instance_dac->VALUE = current_buf[buf_pos];

                buf_pos++;

                if (buf_pos >= DAC_BUF_SIZE)
                {
                    buf_pos = 0;
                    active_buf ^= 1;
                    need_fill = 1;
                }
            }
        }

        HAL_TIMER32_INTERRUPTFLAGS_CLEAR(&htimer32);
    }

    HAL_EPIC_Clear(0xFFFFFFFF);
}
