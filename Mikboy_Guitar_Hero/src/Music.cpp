#include "Music.hpp"
#include "kansas.h"
#include "bad_guitar.h"
#include "zz_top.h"
#include "Game.hpp"

uint8_t volume = 1;

uint16_t dac_buf0[DAC_BUF_SIZE];
uint16_t dac_buf1[DAC_BUF_SIZE];

int active_buf = 0;

AudioChannel music_channel = {KANSAS, KANSAS_LEN, 0, KANSAS_FIRST, KANSAS_INDEX, true};
AudioChannel start_music_channel = {ZZ_TOP, ZZ_TOP_LEN, 0, ZZ_TOP_FIRST, ZZ_TOP_INDEX, false};
AudioChannel bad_channel = {BAD_GUITAR, BAD_GUITAR_LEN, 0, BAD_GUITAR_FIRST, BAD_GUITAR_INDEX, false};

int32_t predictor = 0; //текущее значение сигнала (предыдущий звук)
int32_t adpcm_index = 0;  //индекс шага (его чувствительность)

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

int16_t adpcm_decode_nibble(AudioChannel *ch, uint8_t nibble)
{
    int step = step_table[ch->index]; // берём текущий шаг, index определяет размер шага, чем больше index, тем сильнее изменения сигнала (тихий звук - маленький шаг, громкий - большой шаг)
    int diff = step >> 3; // стартовая минимальная поправка
    // восстановление амплитуды из 4 бит
    if (nibble & 1) diff += step >> 2;
    if (nibble & 2) diff += step >> 1;
    if (nibble & 4) diff += step;
    // бит 8 = знак: 0 - сигнал растёт, 1 - сигнал падает . predictor = предыдущий звук + изменение
    if (nibble & 8) ch->predictor -= diff;
    else ch->predictor += diff;
    // ограничиваем звук, что бы не было переполнения
    if (ch->predictor < -32768) ch->predictor = -32768;
    if (ch->predictor > 32767) ch->predictor = 32767;

    ch->index += index_table[nibble & 0x0F]; // адаптация шага
    // ограничиваем индекс, т.к. step_table имеет размер 89
    if (ch->index < 0) ch->index = 0;
    if (ch->index > 88) ch->index = 88;

    return ch->predictor; // возврвт результата
}
int16_t decode_sample(AudioChannel *ch)
{
    uint8_t byte = ch->data[ch->pos / 2]; // Поскольку 2 sample в одном byte, надо понять, какой байт содержит нужный nibble. Переводит номер nibble в номер байта.
    uint8_t nibble = (ch->pos % 2 == 0) ? (byte & 0x0F) : (byte >> 4); // nibble = 4 бита Функция выбирает либо нижний nibble, либо верхний nibble в зависимости от позиции
    int16_t sample = adpcm_decode_nibble(ch, nibble); // 4-битный код превращается в настоящий PCM sample
    ch->pos++; // возьмёт следующий nibble

    if (ch->pos / 2 >= ch->len) // проверка конца аудио (/2 потому что len хранится в байтах, а pos хранится в nibble
    {
        ch->pos = 0; // перезапуск позиции
        if (ch == &music_channel) {
            game_state = GameState::GAME_OVER;
        }
        else ch->active = false;  // эффекты просто выключаем
    }
    return sample;
}

void fill_dac_buf(uint16_t *buf) // Буфер заполняется значениями 0..4095, которые потом отправляются в DAC
{
    for (int i = 0; i < DAC_BUF_SIZE; i++) // цикл создаёт аудио по одному семплу (мгновенное значение звуковой волны)
    {
        int32_t music_sample = 0; // переменная под текущий семпл музыки
        if (music_channel.active) music_sample = decode_sample(&music_channel); //если музыка активна, то восстанавливает настоящий звук из сжатого формата
        int32_t bad_sample = 0; // получение звука промаха
        if (bad_channel.active) bad_sample = decode_sample(&bad_channel); // если эффект включён, то проигрываем звук промаха
        int32_t mixed = music_sample + (bad_sample / 2); // микширование семпла (/2 чтобы эффект был тише музыки, иначе music + effect могут давать перегрузку)
        // Ограничение
        if (mixed > 32767) mixed = 32767;
        if (mixed < -32768) mixed = -32768;
        // Громкость
        mixed = (mixed * volume) / 50;
        mixed = mixed >> 4; // деление на 16, что бы уменьшить диапазон для DMA
        int32_t out = DAC_MID + mixed; // Смещение в середину DAC
        if (out < 0) out = 0;
        if (out > 4095) out = 4095;
        buf[i] = (uint16_t)out;
    }
}
void fill_dac_buf_start(uint16_t *buf)
{
    for (int i = 0; i < DAC_BUF_SIZE; i++)
    {
        int32_t start_sample = decode_sample(&start_music_channel);
        // Ограничение
        if (start_sample > 32767) start_sample = 32767;
        if (start_sample < -32768) start_sample = -32768;
        // Громкость
        start_sample = (start_sample * volume) / 150;
        start_sample = start_sample >> 4; // деление на 16, что бы уменьшить диапазон для DMA
        int32_t out = DAC_MID + start_sample; // Смещение в середину DAC
        if (out < 0) out = 0;
        if (out > 4095) out = 4095;
        buf[i] = (uint16_t)out;
    }
}

void play_bad_sound()
{
    bad_channel.pos = 0;

    bad_channel.predictor = BAD_GUITAR_FIRST;
    bad_channel.index = BAD_GUITAR_INDEX;

    bad_channel.active = true;
}
