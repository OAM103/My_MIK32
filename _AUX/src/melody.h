#ifndef MELODY_H
#define MELODY_H
#include "inttypes.h"
#include <stdlib.h>

typedef struct {
    uint8_t string;     
    uint8_t fret;
    uint32_t duration; 
} Note;

extern Note melody1[];
extern const uint32_t melody1_len;

extern Note melody2[];
extern const uint32_t melody2_len;

extern Note melody3[];
extern const uint32_t melody3_len;

#endif
