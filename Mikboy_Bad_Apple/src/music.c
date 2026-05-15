#include "music.h"
#include "BAD_APPLE.h"

uint8_t volume = 1;

uint32_t music_pos = 0;

struct Buf buf_music[BUF_COUNT];

uint16_t dac_buf0[DAC_BUF_SIZE];
uint16_t dac_buf1[DAC_BUF_SIZE];

int active_buf = 0;
int current_track = 0;

MusicTrack tracks[1] = {
    {"BAD_APPLE", BAD_APPLE, BAD_APPLE_LEN, BAD_APPLE_FIRST, BAD_APPLE_INDEX},
};


int32_t predictor = BAD_APPLE_FIRST; //текущее значение сигнала (предсказание)
int32_t adpcm_index = BAD_APPLE_INDEX;  //индекс шага

const int step_table[89] = { // размер возможного изменения сигнала (например 7 - меняется медлено, а 3000 - меняется резко)
    7,8,9,10,11,12,13,14,16,17,19,21,23,25,28,31,34,37,41,45,
    50,55,60,66,73,80,88,97,107,118,130,143,157,173,190,209,230,
    253,279,307,337,371,408,449,494,544,598,658,724,796,876,963,
    1060,1166,1282,1411,1552,1707,1878,2066,2272,2499,2749,3024,
    3327,3660,4026,4428,4871,5358,5894,6484,7132,7845,8630,9493,
    10442,11487,12635,13899,15289,16818,18500,20350,22385,24623,
    27086,29794,32767
};

const int index_table[16] = {-1,-1,-1,-1,2,4,6,8,-1,-1,-1,-1,2,4,6,8}; // как изменить ADPCM индекс после каждого нибла - шага

int16_t adpcm_decode_nibble(uint8_t nibble) { 
    int step = step_table[adpcm_index];
    int diff = step >> 3;              //базовое изменение

    if (nibble & 1) diff += step >> 2; // изменение 0
    if (nibble & 2) diff += step >> 1; // изменение 1
    if (nibble & 4) diff += step;      // изменение 2

    if (nibble & 8)
        predictor -= diff; //знак для нибла
    else
        predictor += diff;
    //ограничения, чтобы не выйти за пределы синусойды
    if (predictor < -32768) predictor = -32768;
    if (predictor > 32767) predictor = 32767;

    adpcm_index += index_table[nibble & 0x0F]; //обновляем индекс
    //ограничиваем индекс
    if (adpcm_index < 0) adpcm_index = 0;
    if (adpcm_index > 88) adpcm_index = 88;

    return predictor;
}

void fill_dac_buf(uint16_t *buf)
{
    MusicTrack *track = &tracks[current_track];

    for (int i = 0; i < DAC_BUF_SIZE; i++)
    {
        uint8_t byte = track->data[music_pos / 2]; // чтение данных - 1 байт = 2 сэмпла
        uint8_t nibble = (music_pos % 2 == 0) ? (byte & 0x0F) : (byte >> 4); //выделяем нибл = 4 бита (знак | размер_изменений0 | размер_изменений1 | размер_изменений2)

        int16_t sample = adpcm_decode_nibble(nibble);

        int32_t scaled = ((int32_t)sample * volume) / 100; // регулировка громкости (регулируем амплитуду)

        scaled = scaled >> 4; //деление на 16, чтобы не дёргало

        int32_t out = DAC_MID + scaled; // превращаем сигнал -2048..2047 (PCM (Pulse Code Modulation) - массив чисел) в 0..4095 (значения для DAC)
        if (out < 0) out = 0;
        if (out > 4095) out = 4095;

        buf[i] = (uint16_t)out;

        music_pos++; //следующий семпл
        if (music_pos / 2 >= track->len) music_pos = 0; // зацикливаем трек
    }
}