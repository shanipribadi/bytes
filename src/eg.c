
/*
 * eg.c
 * 
 * Copyright (c) 2014 Hannu Haahti <grejppi@gmail.com>
 * Based on an example by mystran: <http://is.gd/etoZHj>
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
#include <math.h>
#include <stdio.h>

#include "eg.h"

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
    static unsigned long c = 0;
    
    self->rate = rate;
    self->sustain = sustain;
    
    if (self->attack != attack) {
        printf ("update attack %lu\n", c++);
        self->a_coeff = 1.0f - exp (-1.0f / (attack * rate));
        self->attack = attack;
    }
    if (self->decay != decay) {
        printf ("update decay %lu\n", c++);
        self->d_coeff = 1.0f - exp (-1.0f / (decay * rate));
        self->decay = decay;
    }
    if (self->release != release) {
        printf ("update release %lu\n", c++);
        self->r_coeff = 1.0f - exp (-1.0f / (release * rate));
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
