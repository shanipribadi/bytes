
/*
 * lv2plugin.c
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

#include "bytes.h"

static LV2_Handle instantiate (const LV2_Descriptor* descriptor, double rate, const char* bundle_path, const LV2_Feature* const* features) {
    LV2_URID_Map* map = NULL;
    Bytes* self;
    
    for (int i = 0; features[i]; ++i) {
        if (!strcmp (features[i]->URI, LV2_URID__map)) {
            map = (LV2_URID_Map*) features[i]->data;
            break;
        }
    }
    
    if (!map) {
        return NULL;
    }
    
    self = bytes_new (rate);
    
    self->map = map;
    self->uris.midi_MidiEvent = map->map (map->handle, LV2_MIDI__MidiEvent);
    
    return (LV2_Handle) self;
}

#define BYTES_CONNECT_SWITCH(p) switch ((Bytes_PortIndex) p)
#define BYTES_CONNECT(c,f,t) case c: self->ports.f = (t) data; break

static void connect_port (LV2_Handle instance, uint32_t port, void* data) {
    Bytes* self = (Bytes*) instance;
    
    BYTES_CONNECT_SWITCH (port) {
        BYTES_CONNECT (CONTROL, control, const LV2_Atom_Sequence*);
        BYTES_CONNECT (LOUT, lout, float*);
        BYTES_CONNECT (ROUT, rout, float*);
        BYTES_CONNECT (BYTE1, byte1, const float*);
        BYTES_CONNECT (BYTE2, byte2, const float*);
        BYTES_CONNECT (BYTE3, byte3, const float*);
        BYTES_CONNECT (BYTE4, byte4, const float*);
        BYTES_CONNECT (LSYNC1, lsync1, const float*);
        BYTES_CONNECT (LSYNC2, lsync2, const float*);
        BYTES_CONNECT (LSYNC3, lsync3, const float*);
        BYTES_CONNECT (LSYNC4, lsync4, const float*);
        BYTES_CONNECT (LMOD1, lmod1, const float*);
        BYTES_CONNECT (LMOD2, lmod2, const float*);
        BYTES_CONNECT (LMOD3, lmod3, const float*);
        BYTES_CONNECT (LMOD4, lmod4, const float*);
        BYTES_CONNECT (RSYNC1, rsync1, const float*);
        BYTES_CONNECT (RSYNC2, rsync2, const float*);
        BYTES_CONNECT (RSYNC3, rsync3, const float*);
        BYTES_CONNECT (RSYNC4, rsync4, const float*);
        BYTES_CONNECT (RMOD1, rmod1, const float*);
        BYTES_CONNECT (RMOD2, rmod2, const float*);
        BYTES_CONNECT (RMOD3, rmod3, const float*);
        BYTES_CONNECT (RMOD4, rmod4, const float*);
        BYTES_CONNECT (GAIN1, gain1, const float*);
        BYTES_CONNECT (GAIN2, gain2, const float*);
        BYTES_CONNECT (GAIN3, gain3, const float*);
        BYTES_CONNECT (GAIN4, gain4, const float*);
        BYTES_CONNECT (EG1_ATTACK, eg1_attack, const float*);
        BYTES_CONNECT (EG1_DECAY, eg1_decay, const float*);
        BYTES_CONNECT (EG1_SUSTAIN, eg1_sustain, const float*);
        BYTES_CONNECT (EG1_RELEASE, eg1_release, const float*);
        BYTES_CONNECT (EG2_ATTACK, eg2_attack, const float*);
        BYTES_CONNECT (EG2_DECAY, eg2_decay, const float*);
        BYTES_CONNECT (EG2_SUSTAIN, eg2_sustain, const float*);
        BYTES_CONNECT (EG2_RELEASE, eg2_release, const float*);
        default: break;
    }
}

static void activate (LV2_Handle instance) {
}

static void run (LV2_Handle instance, uint32_t nframes) {
    Bytes* self = (Bytes*) instance;
    uint32_t offset = 0;
    
    memset (self->ports.lout, 0, sizeof (float) * nframes);
    memset (self->ports.rout, 0, sizeof (float) * nframes);
    
    self->bytes[0] = (uint8_t) *(self->ports.byte1);
    self->bytes[1] = (uint8_t) *(self->ports.byte2);
    self->bytes[2] = (uint8_t) *(self->ports.byte3);
    self->bytes[3] = (uint8_t) *(self->ports.byte4);
    
    self->lsync[0] = *(self->ports.lsync1);
    self->lsync[1] = *(self->ports.lsync2);
    self->lsync[2] = *(self->ports.lsync3);
    self->lsync[3] = *(self->ports.lsync4);
    self->lmod[0] = *(self->ports.lmod1);
    self->lmod[1] = *(self->ports.lmod2);
    self->lmod[2] = *(self->ports.lmod3);
    self->lmod[3] = *(self->ports.lmod4);
    
    self->rsync[0] = *(self->ports.rsync1);
    self->rsync[1] = *(self->ports.rsync2);
    self->rsync[2] = *(self->ports.rsync3);
    self->rsync[3] = *(self->ports.rsync4);
    self->rmod[0] = *(self->ports.rmod1);
    self->rmod[1] = *(self->ports.rmod2);
    self->rmod[2] = *(self->ports.rmod3);
    self->rmod[3] = *(self->ports.rmod4);
    
    self->gain[0] = *(self->ports.gain1);
    self->gain[1] = *(self->ports.gain2);
    self->gain[2] = *(self->ports.gain3);
    self->gain[3] = *(self->ports.gain4);
    
    LV2_ATOM_SEQUENCE_FOREACH (self->ports.control, ev) {
        if (ev->body.type == self->uris.midi_MidiEvent) {
            const uint8_t* const msg = (const uint8_t*) (ev + 1);
            
            switch (lv2_midi_message_type (msg)) {
            case LV2_MIDI_MSG_NOTE_ON:
                bytes_note_on (self, msg[1] & 0x7f, msg[2] & 0x7f);
                break;
            
            case LV2_MIDI_MSG_NOTE_OFF:
                bytes_note_off (self, msg[1] & 0x7f, msg[2] & 0x7f);
                break;
            
            default:
                break;
            }
        }
        
        bytes_render (self, offset, ev->time.frames);
        offset = (uint32_t) ev->time.frames;
    }
    
    bytes_render (self, offset, nframes);
}

static void deactivate (LV2_Handle instance) {
}

static void cleanup (LV2_Handle instance) {
    free (instance);
}

static const void* extension_data (const char* uri) {
    return NULL;
}

static const LV2_Descriptor descriptor = {
    BYTES_URI,
    instantiate,
    connect_port,
    activate,
    run,
    deactivate,
    cleanup,
    extension_data
};

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor (uint32_t index) {
    switch (index) {
    case 0:
        return &descriptor;
    
    default:
        return NULL;
    }
}
