// music.h
#ifndef __MUSIC__
#define __MUSIC__

#ifdef __cplusplus
extern "C"
{
#endif
#include "mik32_hal.h"
#include "bad_guitar.h"

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
        const uint8_t *data;
        uint32_t len;
        uint32_t pos;
        int32_t predictor;
        int32_t index;
        bool active;

    } AudioChannel;

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
    extern AudioChannel music_channel;
    extern AudioChannel bad_channel;

    /// @brief Из 4-битного "кода изменения сигнала" восстановить PCM-сэмпл (16-bit звук)
    /// @param ch состояние канала (хранит память сигнала)
    /// @param nibble 4 бита закодированного изменения
    /// @return новый PCM-сэмпл (16-bit)
    int16_t adpcm_decode_nibble(AudioChannel *ch, uint8_t nibble);

    void fill_dac_buf(uint16_t *buf);
    void fill_dac_buf_start(uint16_t *buf);
    void play_bad_sound();
    void decode_bad_guitar_to_pcm();

#ifdef __cplusplus
}
#endif

#endif // __MUSIC__