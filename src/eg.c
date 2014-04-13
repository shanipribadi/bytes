
#include <stdlib.h>

#include "eg.h"
#include "fastonebigheader.h"

void bytes_eg_init (Bytes_EG* self) {
    self->value = 0.0f;
    self->a_coeff = 0.0f;
    self->d_coeff = 0.0f;
    self->r_coeff = 0.0f;
    self->gate = 0;
    self->alive = 0;
    self->attack = -1;
    self->decay = -1;
    self->sustain = -1;
    self->release = -1;
    self->rising = 0;
}

void bytes_eg_setup (Bytes_EG* self, float rate, float attack, float decay, float sustain, float release) {
    self->rate = rate;
    self->sustain = sustain;
    
    if (self->attack != attack) {
        self->a_coeff = 1.0f - fastexp (-1.0f / (attack * rate));
        self->attack = attack;
    }
    if (self->decay != decay) {
        self->d_coeff = 1.0f - fastexp (-1.0f / (decay * rate));
        self->decay = decay;
    }
    if (self->release != release) {
    self->r_coeff = 1.0f - fastexp (-1.0f / (release * rate));
        self->release = release;
    }
}

void bytes_eg_on (Bytes_EG* self) {
    self->gate = 1;
    self->alive = 1;
    self->rising = 1;
}

void bytes_eg_off (Bytes_EG* self) {
    self->gate = 0;
}

inline float bytes_eg_value (Bytes_EG* self) {
    return self->value;
}

float bytes_eg_next (Bytes_EG* self) {
    if (self->gate) {
        if (self->rising) {
            self->value += self->a_coeff * ((1 / 0.63f) - self->value);
            if (self->value > 1) {
                self->value = 1;
                self->rising = 0;
            }
        } else {
            self->value += self->d_coeff * (self->sustain - self->value);
        }
    } else {
        self->value += self->r_coeff * (1 - (1 / 0.63f) - self->value);
        if (self->value <= 0) {
            self->alive = 0;
            self->value = 0;
        }
    }
    
    return bytes_eg_value (self);
}
