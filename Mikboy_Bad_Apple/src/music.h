// music.h
#ifndef __MUSIC__
#define __MUSIC__

#ifdef __cplusplus
extern "C"
{
#endif
#include "mik32_hal.h"

#define DAC_BUF_SIZE 1024 // кол-во семплов
#define DAC_MID 2048     // 12-бит DAC
#define BUF_COUNT 2

    struct Buf
    {
        uint16_t buf[DAC_BUF_SIZE]; // Готовые значения для ЦАП (0-4095)
        uint8_t is_busy;
    };

    RAM_ATTR extern struct Buf buf_music[BUF_COUNT];

    typedef struct
    {
        const char *name;    // имя трека
        const uint8_t *data; // указатель на массив ADPCM
        uint32_t len;        // длина трека
        int32_t first;       // predictor - текущее значение сигнала
        int32_t index;       // ADPCM index - на сколько большой может быть следующий шаг
    } MusicTrack;

    extern MusicTrack tracks[1];
    extern const int step_table[89];
    extern const int index_table[16];
    extern uint8_t volume;
    extern uint32_t music_pos;
    extern uint16_t dac_buf0[DAC_BUF_SIZE];
    extern uint16_t dac_buf1[DAC_BUF_SIZE];
    extern int active_buf;
    extern int current_track;
    extern int32_t predictor;
    extern int32_t adpcm_index;

    int16_t adpcm_decode_nibble(uint8_t nibble);
    void fill_dac_buf(uint16_t *buf);

#ifdef __cplusplus
}
#endif

#endif // __MUSIC__