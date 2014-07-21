
/*
 * voice.h
 * 
 * Copyright (c) 2014 Hannu Haahti <grejppi@gmail.com>
 * 
 * This file is part of Bytes.
 *
 * Bytes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Bytes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Bytes.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BYTES__VOICE_H
#define BYTES__VOICE_H

#include <stdint.h>

#include "dco.h"
#include "eg.h"

typedef struct Bytes__Voice Bytes_Voice;

struct Bytes__Voice {
    DCO dco;
    
    uint8_t key;
    uint8_t velocity;
    
    double hz;
    float gain;
    
    EG eg1;
    EG eg2;
    
    uint32_t counter;
    
    float dc_lin;
    float dc_rin;
    float dc_lout;
    float dc_rout;
};

static inline void bytes_voice_init (Bytes_Voice* self, double rate) {
    dco_init (&self->dco, rate);
}

static inline void bytes_voice_next (Bytes_Voice* self, double hz) {
    dco_next (&self->dco, hz);
}

#endif
