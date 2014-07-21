
/*
 * eg.h
 * 
 * Based on an example by mystran <http://is.gd/etoZHj>,
 * Bytes: Copyright (c) 2014 Hannu Haahti <grejppi@gmail.com>
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

#ifndef EG_H
#define EG_H

typedef struct EG_ EG;

struct EG_ {
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

void eg_init (EG* self);
void eg_setup (EG*, float rate, float attack, float decay, float sustain, float release);
void eg_on (EG* self);
void eg_off (EG* self);
float eg_value (EG* self);
float eg_next (EG* self);

#endif
