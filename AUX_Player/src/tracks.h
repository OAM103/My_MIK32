#ifndef TRACKS_H
#define TRACKS_H
#include "mik32_hal.h"

#define DAC_BUF_SIZE 1024
#define DAC_MID 2048 // 12-бит DAC

typedef struct {
    const char *name;     // имя трека
    const uint8_t *data;  // указатель на массив ADPCM
    uint32_t len;         // длина трека
    int32_t first;        // predictor
    int32_t index;        // ADPCM index
} MusicTrack;

extern MusicTrack tracks[3];
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

void fill_dac_buf(uint16_t *buf);
void switch_track(int track_id);
void play_track0();
void play_track1();
void play_track2();

#endif