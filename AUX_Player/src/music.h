#ifndef MUSIC_H
#define MUSIC_H
#include "inttypes.h"
#include <stdlib.h>
#pragma once

// трек 1
#define WHAT_LEN 224000
#define WHAT_FIRST -96
#define WHAT_INDEX 0
extern const uint8_t what[WHAT_LEN];

// трек 2
#define JUMP_LEN 211320
#define JUMP_FIRST 59
#define JUMP_INDEX 42
extern const uint8_t jump[JUMP_LEN];

// трек 3
#define NOIZE_LEN 523364
#define NOIZE_FIRST -1
#define NOIZE_INDEX 0
extern const uint8_t noize[NOIZE_LEN];

#define MUSIC_LEN 448000
extern const uint16_t music[MUSIC_LEN];

#endif