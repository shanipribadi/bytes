
#include <stdlib.h>

#include "bytes.h"

Bytes* bytes_new (double rate) {
    Bytes* self = malloc (sizeof (Bytes));
    
    for (unsigned i = 0; i < NVOICES; ++i) {
        bytes_voice_init (&self->voices[i], rate);
        bytes_eg_init (&self->voices[i].eg);
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
        if (self->voices[i].key == INVALID_KEY || !self->voices[i].eg.alive) {
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
    
    bytes_eg_on (&v->eg);
}

void bytes_note_off (Bytes* self, uint8_t key, uint8_t velocity) {
    Bytes_Voice* v;
    
    for (unsigned i = 0; i < NVOICES; ++i) {
        v = &self->voices[i];
        
        if (v->key == key) {
            bytes_eg_off (&v->eg);
            v->velocity = 0;
        }
    }
}

void bytes_render (Bytes* self, uint32_t start, uint32_t end) {
    Bytes_Voice* v;
    
    static float pulse[2] = { -1, 1 };
    
    for (unsigned vi = 0; vi < NVOICES; ++vi) {
        v = &self->voices[vi];
        
        bytes_eg_setup (&v->eg, self->rate, 0, 0, 1, 0);
        
        if (v->eg.alive) {
            bytes_voice_init (v, self->rate);
            ++v->counter;
            
            for (uint32_t i = start; i < end; ++i) {
                float s = 0;
                
                bytes_eg_next (&v->eg);
                for (unsigned o = 0; o < OVERSAMPLING; ++o) {
                    bytes_voice_next (v, v->hz);
                    for (unsigned b = 0; b < 4; ++b) {
                        s += pulse[!!(self->bytes[b] & (1 << (v->phase >> 29)))];
                    }
                    s *= v->eg.value;
                }
                
                s /= (float) (NVOICES * OVERSAMPLING);
                self->ports.lout[i] += s;
                self->ports.rout[i] += s;
            }
        }
    }
}
