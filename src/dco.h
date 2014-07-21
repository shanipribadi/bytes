
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

#ifndef DCO_H
#define DCO_H

#include <stdint.h>

typedef struct DCO_ DCO;

struct DCO_ {
    uint32_t phase;
    double rate;
};

static inline void dco_init (DCO* self, double rate) {
    self->rate = rate;
}

static inline void dco_next (DCO* self, double hz) {
    self->phase += lrint (MAX_PHASE / (self->rate / hz));
}

#endif
