
/*
 * eg.h
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

#ifndef BYTES__EG_H
#define BYTES__EG_H

typedef struct Bytes__EG Bytes_EG;

struct Bytes__EG {
    float rate;
    float value;
    float attack;
    float decay;
    float sustain;
    float release;
    float a_coeff;
    float d_coeff;
    float r_coeff;
    int gate;
    int alive;
    int rising;
};

void bytes_eg_init (Bytes_EG* self);
void bytes_eg_setup (Bytes_EG*, float rate, float attack, float decay, float sustain, float release);
void bytes_eg_on (Bytes_EG* self);
void bytes_eg_off (Bytes_EG* self);
float bytes_eg_value (Bytes_EG* self);
float bytes_eg_next (Bytes_EG* self);

#endif
