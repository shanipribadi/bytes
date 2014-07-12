
/*
 * bytes.h
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

#ifndef BYTES__H
#define BYTES__H

#include "base.h"
#include "voice.h"

typedef struct Bytes_ Bytes;

typedef enum {
    MOD_MANUAL,
    MOD_ENVELOPE,
    
    /* TODO: */
    MOD_LFO,
    MOD_LFO_BY_ENVELOPE
} Bytes_ModulationMethod;

typedef enum {
    CONTROL,
    LOUT,
    ROUT,
    
    BYTE1,
    BYTE2,
    BYTE3,
    BYTE4,
    
    LSYNC1,
    LSYNC2,
    LSYNC3,
    LSYNC4,
    
    LMOD1,
    LMOD2,
    LMOD3,
    LMOD4,
    
    RSYNC1,
    RSYNC2,
    RSYNC3,
    RSYNC4,
    
    RMOD1,
    RMOD2,
    RMOD3,
    RMOD4,
    
    GAIN1,
    GAIN2,
    GAIN3,
    GAIN4,
    
    EG1_ATTACK,
    EG1_DECAY,
    EG1_SUSTAIN,
    EG1_RELEASE,
    
    EG2_ATTACK,
    EG2_DECAY,
    EG2_SUSTAIN,
    EG2_RELEASE,
    
    MOD_METHOD,
    MODULATION,
    
    NUM_PORTS
} Bytes_PortIndex;

struct Bytes_ {
    struct {
        const LV2_Atom_Sequence* control;
        float* lout;
        float* rout;
        const float* byte1;
        const float* byte2;
        const float* byte3;
        const float* byte4;
        const float* lsync1;
        const float* lsync2;
        const float* lsync3;
        const float* lsync4;
        const float* lmod1;
        const float* lmod2;
        const float* lmod3;
        const float* lmod4;
        const float* rsync1;
        const float* rsync2;
        const float* rsync3;
        const float* rsync4;
        const float* rmod1;
        const float* rmod2;
        const float* rmod3;
        const float* rmod4;
        const float* gain1;
        const float* gain2;
        const float* gain3;
        const float* gain4;
        const float* eg1_attack;
        const float* eg1_decay;
        const float* eg1_sustain;
        const float* eg1_release;
        const float* eg2_attack;
        const float* eg2_decay;
        const float* eg2_sustain;
        const float* eg2_release;
        const float* mod_method;
        const float* modulation;
    } ports;
    
    LV2_URID_Map* map;
    
    struct {
        LV2_URID midi_MidiEvent;
    } uris;
    
    uint8_t bytes[4];
    
    float lsync[4];
    float rsync[4];
    float lmod[4];
    float rmod[4];
    
    float gain[4];
    
    Bytes_ModulationMethod method;
    
    Bytes_Voice voices[NVOICES];
    double rate;
};

Bytes* bytes_new (double rate);
void bytes_note_on (Bytes* self, uint8_t key, uint8_t velocity);
void bytes_note_off (Bytes* self, uint8_t key, uint8_t velocity);
void bytes_render (Bytes* self, uint32_t start, uint32_t end);

#endif
