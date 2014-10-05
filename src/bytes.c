
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
#include "dco.h"

static float sine[4096] = { -1 };

Bytes* bytes_new (double rate) {
    Bytes* self = calloc (1, sizeof (Bytes));
    
    for (unsigned i = 0; i < NVOICES; ++i) {
        bytes_voice_init (&self->voices[i], rate);
        eg_init (&self->voices[i].eg1);
        eg_init (&self->voices[i].eg2);
    }
    
    bytes_set_rate (self, rate);
    
    dco_init (&self->lfo, rate);
    
    if (sine[0] == -1) {
        float fcos = 1.0;
        float fsin = 0.0;
        float delta = (float) (2 * M_PI / 4096.0);
        
        for (unsigned i = 0; i < 4096; ++i) {
            sine[i] = (fsin * 0.5) + 0.5;
            fsin += delta * fcos;
            fcos -= delta * fsin;
        }
    }
    
    return self;
}

void bytes_set_rate (Bytes* self, double rate) {
    self->oversampling = 32;
    self->rate = rate;
}

static Bytes_Voice* bytes_find_voice (Bytes* self) {
    Bytes_Voice* v;
    Bytes_Voice* alt = NULL;
    
    int tries = NVOICES;
    int found = 0;
    uint32_t longest = 0;
    
    while (tries--) {
        v = &self->voices[(++self->voice_index) % NVOICES];
        
        if (v->counter >= longest) {
            alt = v;
            longest = v->counter;
        }
        
        if (v->key == INVALID_KEY) {
            found = 1;
            break;
        }
    }
    
    return found ? v : alt;
}

void bytes_note_on (Bytes* self, uint8_t key, uint8_t velocity) {
    Bytes_Voice* v;
    
    v = bytes_find_voice (self);
    v->counter = 0;
    v->key = key;
    v->velocity = velocity;
    v->hz = key2hz (v->key) / (double) self->oversampling;
    v->gain = velocity / 255.0f;
    
    eg_on (&v->eg1);
    eg_on (&v->eg2);
}

void bytes_note_off (Bytes* self, uint8_t key, uint8_t velocity) {
    Bytes_Voice* v;
    
    for (unsigned i = 0; i < NVOICES; ++i) {
        v = &self->voices[i];
        
        if (v->key == key) {
            eg_off (&v->eg1);
            eg_off (&v->eg2);
            v->velocity = 0;
        }
    }
}

void bytes_render (Bytes* self, uint32_t start, uint32_t end) {
    Bytes_Voice* v;
    
    float llimits[4];
    float rlimits[4];
    
    float lmultiply[4];
    float rmultiply[4];
    
    float modulation;
    
    for (unsigned b = 0; b < 4; ++b) {
        llimits[b] = self->lmod[b] - self->lsync[b];
        rlimits[b] = self->rmod[b] - self->rsync[b];
    }
    
    for (unsigned vi = 0; vi < NVOICES; ++vi) {
        v = &self->voices[vi];
        
        eg_setup (&v->eg1, self->rate,
            *self->ports.eg1_attack,
            *self->ports.eg1_decay,
            *self->ports.eg1_sustain,
            *self->ports.eg1_release);
        
        eg_setup (&v->eg2, self->rate,
            *self->ports.eg2_attack,
            *self->ports.eg2_decay,
            *self->ports.eg2_sustain,
            *self->ports.eg2_release);
        
        bytes_voice_init (v, self->rate);
            
        for (uint32_t i = start; i < end; ++i) {
            float l = 0;
            float r = 0;
            
            if (vi == 0) {
                dco_next (&self->lfo, 0.5);
            }
            
            eg_next (&v->eg1);
            eg_next (&v->eg2);
            
            switch (self->method) {
            default:
            case MOD_MANUAL:
                modulation = *self->ports.modulation * self->mod_range;
                break;
            
            case MOD_ENVELOPE:
                modulation = v->eg2.value * self->mod_range;
                break;
            
            case MOD_LFO:
                modulation = sine[self->lfo.phase >> 20] * self->mod_range;
                break;
            }
            
            for (unsigned b = 0; b < 4; ++b) {
                lmultiply[b] = self->lsync[b] + ((self->mod_min + modulation) * llimits[b]);
                rmultiply[b] = self->rsync[b] + ((self->mod_min + modulation) * rlimits[b]);
                
                if ((int) *self->ports.rounded) {
                    lmultiply[b] = floor (lmultiply[b]);
                    rmultiply[b] = floor (rmultiply[b]);
                }
            }
            
            if (v->eg1.alive) {
                for (unsigned o = 0; o < self->oversampling; ++o) {
                    bytes_voice_next (v, v->hz);
                    for (unsigned b = 0; b < 4; ++b) {
                        l += !!(self->bytes[b] & (1 << ((uint32_t) (v->dco.phase * lmultiply[b]) >> 29))) * self->gain[b];
                        r += !!(self->bytes[b] & (1 << ((uint32_t) (v->dco.phase * rmultiply[b]) >> 29))) * self->gain[b];
                    }
                }
                
                l *= v->eg1.value;
                r *= v->eg1.value;
                
                l /= (float) ((NVOICES >> 1) * self->oversampling);
                r /= (float) ((NVOICES >> 1) * self->oversampling);
            } else {
                ++v->counter;
            }
            
            v->dc_lout = 0.995f * v->dc_lout + l - v->dc_lin;
            v->dc_rout = 0.995f * v->dc_rout + r - v->dc_rin;
            
            v->dc_lin = l;
            v->dc_rin = r;
            
            self->ports.lout[i] += v->dc_lout;
            self->ports.rout[i] += v->dc_rout;
        }
    }
}
