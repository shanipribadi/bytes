
/*
 * bytes.c
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

#include <stdlib.h>

#include "bytes.h"

Bytes* bytes_new (double rate) {
    Bytes* self = calloc (1, sizeof (Bytes));
    
    for (unsigned i = 0; i < NVOICES; ++i) {
        bytes_voice_init (&self->voices[i], rate);
        bytes_eg_init (&self->voices[i].eg1);
        bytes_eg_init (&self->voices[i].eg2);
    }
    
    self->rate = rate;
    
    return self;
}

Bytes_Voice* bytes_find_voice (Bytes* self) {
    Bytes_Voice* v = NULL;
    unsigned q = NVOICES;
    uint32_t longest = 0;
    static unsigned force = 0;
    
    for (unsigned i = 0; i < NVOICES; ++i) {
        if (self->voices[i].key == INVALID_KEY || !self->voices[i].eg1.alive) {
            q = i;
            break;
        }
        
        if (self->voices[i].counter > longest) {
            longest = self->voices[i].counter;
            q = i;
        }
    }
    
    if (q < NVOICES) {
        v = &self->voices[q];
    } else {
        v = &self->voices[++force % NVOICES];
    }
    
    return v;
}

void bytes_note_on (Bytes* self, uint8_t key, uint8_t velocity) {
    Bytes_Voice* v;
    
    v = bytes_find_voice (self);
    v->counter = 0;
    v->key = key;
    v->velocity = velocity;
    v->hz = key2hz (v->key) / (double) OVERSAMPLING;
    v->gain = velocity / 255.0f;
    
    bytes_eg_on (&v->eg1);
    bytes_eg_on (&v->eg2);
}

void bytes_note_off (Bytes* self, uint8_t key, uint8_t velocity) {
    Bytes_Voice* v;
    
    for (unsigned i = 0; i < NVOICES; ++i) {
        v = &self->voices[i];
        
        if (v->key == key) {
            bytes_eg_off (&v->eg1);
            bytes_eg_off (&v->eg2);
            v->velocity = 0;
        }
    }
}

void bytes_render (Bytes* self, uint32_t start, uint32_t end) {
    Bytes_Voice* v;
    
    float llimits[4];
    float rlimits[4];
    
    for (unsigned b = 0; b < 4; ++b) {
        llimits[b] = self->lmod[b] - self->lsync[b];
        rlimits[b] = self->rmod[b] - self->rsync[b];
    }
    
    for (unsigned vi = 0; vi < NVOICES; ++vi) {
        v = &self->voices[vi];
        
        bytes_eg_setup (&v->eg1, self->rate,
            *(self->ports.eg1_attack),
            *(self->ports.eg1_decay),
            *(self->ports.eg1_sustain),
            *(self->ports.eg1_release));
        
        bytes_eg_setup (&v->eg2, self->rate,
            *(self->ports.eg2_attack),
            *(self->ports.eg2_decay),
            *(self->ports.eg2_sustain),
            *(self->ports.eg2_release));
        
        if (v->eg1.alive || v->eg2.alive) {
            bytes_voice_init (v, self->rate);
            ++v->counter;
            
            for (uint32_t i = start; i < end; ++i) {
                float l = 0;
                float r = 0;
                
                bytes_eg_next (&v->eg1);
                bytes_eg_next (&v->eg2);
                
                for (unsigned o = 0; o < OVERSAMPLING; ++o) {
                    bytes_voice_next (v, v->hz);
                    for (unsigned b = 0; b < 4; ++b) {
                        l += !!(self->bytes[b] & (1 << ((uint32_t) (v->phase * (self->lsync[b] + (v->eg2.value * llimits[b]))) >> 29))) * self->gain[b];
                        r += !!(self->bytes[b] & (1 << ((uint32_t) (v->phase * (self->rsync[b] + (v->eg2.value * rlimits[b]))) >> 29))) * self->gain[b];
                    }
                }
                
                l *= v->eg1.value;
                r *= v->eg1.value;
                
                l /= (float) ((NVOICES >> 1) * OVERSAMPLING);
                r /= (float) ((NVOICES >> 1) * OVERSAMPLING);
                
                v->dc_lout = 0.995f * v->dc_lout + l - v->dc_lin;
                v->dc_rout = 0.995f * v->dc_rout + r - v->dc_rin;
                
                v->dc_lin = l;
                v->dc_rin = r;
                
                self->ports.lout[i] += v->dc_lout;
                self->ports.rout[i] += v->dc_rout;
            }
        }
    }
}
