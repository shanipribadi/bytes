
/*
 * dco.h
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

#ifndef BYTES__DCO_H
#define BYTES__DCO_H

#include <stdint.h>

typedef struct Bytes__DCO Bytes_DCO;

struct Bytes__DCO {
    uint32_t phase;
    double rate;
};

static inline void bytes_dco_init (Bytes_DCO* self, double rate) {
    self->rate = rate;
}

static inline void bytes_dco_next (Bytes_DCO* self, double hz) {
    self->phase += lrint (MAX_PHASE / (self->rate / hz));
}

#endif
