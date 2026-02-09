
#include "mik32_hal_pcc.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal.h"
#include "mik32_hal_timer32.h"
#include "mik32_hal_dma.h"
#include "mik32_hal_dac.h"

#include <math.h>

#include "uart_lib.h"
#include "xprintf.h"
#include "melody.h"
#include "setting.h"

#define DAC_BUF_SIZE 1024 //256 
#define DAC_MID (4095/2) // 12-бит DAC
#define TIMER_CLK 64000000UL  // 64 МГц - тональность
#define VOLUME 10 // 0..100 (%)
#define STRING_COUNT (sizeof(eBGDAE) / sizeof(eBGDAE[0]))
#define SPEED 1900

#define TABLE_BITS     8                    // log2(256)
#define PHASE_SHIFT    (PHASE_BITS - TABLE_BITS)

#define MAX_VOICES     2

uint16_t sine_table[DAC_BUF_SIZE];

void init_sine_table() //синусойда
{
    for (int i = 0; i < DAC_BUF_SIZE; i++)
    {
        float phase = 2.0f * M_PI * i / DAC_BUF_SIZE;
        float window = 0.42f - 0.5f * cosf(2 * M_PI * i / (DAC_BUF_SIZE - 1)) + 0.08f * cosf(4 * M_PI * i / (DAC_BUF_SIZE - 1)); //окно Ханна
        float s = sinf(phase) * window;

        int32_t sample = (int32_t)(s * DAC_MID * VOLUME / 100.0f); 
        sine_table[i] = DAC_MID + sample;
    }
}

uint16_t eBGDAE[] = {330,247, 196, 147,110,82,0};

const uint16_t fret_mul_q12[13] = {
    1000, // 2^0/12
    1059, // 1
    1122, // 2
    1189, // 3
    1260, // 4
    1335, // 5
    1414, // 6
    1498, // 7
    1587, // 8
    1682, // 9
    1782, // 10
    1888, // 11
    2000  // 12 = x2
};

uint32_t GuitarNote(int8_t string, uint8_t fret)
{
    if (string < 0 || string >= STRING_COUNT) return 0;
    if (fret > 12) fret = 12;

    return (eBGDAE[string] * fret_mul_q12[fret])/1000;
}

void SetNoteFrequency(uint32_t freq)
{
    // короткий fade-out
    hdac1.Instance_dac->VALUE = DAC_MID;
    hdac2.Instance_dac->VALUE = DAC_MID;

    HAL_Timer32_Stop(&htimer32);

    if (freq == 0) return;

    uint32_t top = TIMER_CLK / (freq * DAC_BUF_SIZE);
    if (top < 10) top = 10;
    if (top > 0xFFFF) top = 0xFFFF;

    htimer32.Top = top;
    HAL_Timer32_Init(&htimer32);
    HAL_Timer32_Value_Clear(&htimer32);
    HAL_Timer32_Start(&htimer32);
}

void StartNote(Note *n)
{
    uint32_t freq = GuitarNote(n->string, n->fret);
    SetNoteFrequency(freq);
}

int main()
{
    // HAL_Init();

    SystemClock_Config();
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    DMA_Init();

    DAC_Init();
    DAC2_Init();

    Timer32_Init();

    GPIO_Init();

    // HAL_Timer32_Channel_Enable(&htimer32_channel);
    HAL_Timer32_Value_Clear(&htimer32);
    HAL_Timer32_Start(&htimer32);

    HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS_ALL, __LOW);           // Все CS в 1
    HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS1 | SLAVE_CS0, __HIGH); // POT0_CS в 0

    HAL_GPIO_WritePin(GPIO_POT0, POT0_INC, __HIGH); // Запретить инкремент

    HAL_GPIO_WritePin(GPIO_POT0, POT0_UP_DOWN, __LOW);

    HAL_DAC_Init(&hdac1);
    HAL_DAC_Init(&hdac2);
    HAL_Timer32_Start(&htimer32);

    init_sine_table();
    int note_timer = 0; 
    uint32_t note_index = 0;

    while (1)
    {
        if (HAL_DMA_GetChannelReadyStatus(&hdma_ch0) == 1)
        {
            HAL_DMA_Start(&hdma_ch0, (void *)&sine_table, (void *)&hdac1.Instance_dac->VALUE, sizeof(sine_table) - 1);
        }

        // if (HAL_DMA_GetChannelReadyStatus(&hdma_ch1) == 1)
        // {
        //     HAL_DMA_Start(&hdma_ch1, (void *)&sine_table, (void *)&hdac2.Instance_dac->VALUE, sizeof(sine_table) - 1);
        // }

        note_timer++;
        if (note_timer >= melody1[note_index].duration*SPEED)
        {
            note_timer = 0;
            note_index++;

            if (note_index >= melody1_len)
                note_index = 0;
            StartNote(&melody1[note_index]);
        }
       
    }
}
