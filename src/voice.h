
#ifndef BYTES__VOICE_H
#define BYTES__VOICE_H

#include <stdint.h>

#include "eg.h"

typedef struct Bytes__Voice Bytes_Voice;

struct Bytes__Voice {
    uint32_t phase;
    
    double rate;
    
    uint8_t key;
    uint8_t velocity;
    
    double hz;
    float gain;
    
    Bytes_EG eg1;
    Bytes_EG eg2;
    
    uint32_t counter;
};

void bytes_voice_init (Bytes_Voice* self, double rate);
void bytes_voice_next (Bytes_Voice* self, double hz);

#endif
