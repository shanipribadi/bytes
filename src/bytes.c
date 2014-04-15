
#include <stdlib.h>

#include "bytes.h"

Bytes* bytes_new (double rate) {
    Bytes* self = malloc (sizeof (Bytes));
    
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
    
    float limits[4];
    
    static float pulse[2] = { -1, 1 };
    
    for (unsigned b = 0; b < 4; ++b) {
        limits[b] = self->mod[b] - self->sync[b];
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
        
        if (v->eg1.alive) {
            bytes_voice_init (v, self->rate);
            ++v->counter;
            
            for (uint32_t i = start; i < end; ++i) {
                float s = 0;
                
                bytes_eg_next (&v->eg1);
                bytes_eg_next (&v->eg2);
                
                for (unsigned o = 0; o < OVERSAMPLING; ++o) {
                    bytes_voice_next (v, v->hz);
                    for (unsigned b = 0; b < 4; ++b) {
                        s += pulse[!!(self->bytes[b] & (1 << ((uint32_t) (v->phase * (self->sync[b] + (v->eg2.value * limits[b]))) >> 29)))] * self->gain[b];
                    }
                    s *= v->eg1.value;
                }
                
                s /= (float) (NVOICES * OVERSAMPLING);
                self->ports.lout[i] += s;
                self->ports.rout[i] += s;
            }
        }
    }
}
