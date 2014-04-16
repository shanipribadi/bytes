
/*
 * vstplugin.c
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

#include "vst/aeffectx.h"
#include "vst/lv2_evbuf.h"

#define PARAMOFFSET 3

typedef struct BytesVST_ BytesVST;
typedef struct BytesVST__Port BytesVST_Port;

struct BytesVST_ {
    LV2_Descriptor* descriptor;
    LV2_Handle handle;
    LV2_Evbuf* evbuf;
    float ports[NUM_PORTS - PARAMOFFSET][1];
};

struct BytesVST__Port {
    const char name[8];
    const float min;
    const float max;
    const float def;
};

static const BytesVST_Port ranges[NUM_PORTS - PARAMOFFSET] = {
    { "byte1", 0, 255, 15 },
    { "byte2", 0, 255, 15 },
    { "byte3", 0, 255, 15 },
    { "byte4", 0, 255, 15 },
    
    { "lsync1", 0.5, 8, 1 },
    { "lsync2", 0.5, 8, 1 },
    { "lsync3", 0.5, 8, 1 },
    { "lsync4", 0.5, 8, 1 },
    
    { "lmod1", 0.5, 8, 1 },
    { "lmod2", 0.5, 8, 1 },
    { "lmod3", 0.5, 8, 1 },
    { "lmod4", 0.5, 8, 1 },
    
    { "rsync1", 0.5, 8, 1 },
    { "rsync2", 0.5, 8, 1 },
    { "rsync3", 0.5, 8, 1 },
    { "rsync4", 0.5, 8, 1 },
    
    { "rmod1", 0.5, 8, 1 },
    { "rmod2", 0.5, 8, 1 },
    { "rmod3", 0.5, 8, 1 },
    { "rmod4", 0.5, 8, 1 },
    
    { "gain1", 0, 1, 1 },
    { "gain2", 0, 1, 1 },
    { "gain3", 0, 1, 1 },
    { "gain4", 0, 1, 1 },
    
    { "atk1", 0, 1, 0 },
    { "dec1", 0, 1, 1 },
    { "sus1", 0, 1, 1 },
    { "rel1", 0, 1, 0 },
    
    { "atk2", 0, 1, 0 },
    { "dec2", 0, 1, 1 },
    { "sus2", 0, 1, 0 },
    { "rel2", 0, 1, 0 }
};

static inline float value_to_lv2 (float vstvalue, uint32_t index) {
    return ranges[index].min + (vstvalue * (ranges[index].max - ranges[index].min));
}

static inline float value_to_vst (float lv2value, uint32_t index) {
    return (lv2value - ranges[index].min) / (ranges[index].max - ranges[index].min);
}

enum {
    midi_MidiEvent,
    atom_Chunk,
    atom_Sequence,
    NUM_URIS
};

static const char* uris[NUM_URIS] = {
    LV2_MIDI__MidiEvent,
    LV2_ATOM__Chunk,
    LV2_ATOM__Sequence
};

static LV2_URID map_uri (LV2_URID_Map_Handle handle, const char* uri) {
    for (uint32_t i = 0; i < NUM_URIS; ++i) {
        if (!strcmp (uri, uris[i])) {
            return i;
        }
    }
    return NUM_URIS;
}

static const char* unmap_uri (LV2_URID_Unmap_Handle handle, LV2_URID urid) {
    if (urid < NUM_URIS) {
        return uris[urid];
    }
    return NULL;
}

LV2_URID_Map urid_map = { NULL, &map_uri };
LV2_URID_Unmap urid_unmap = { NULL, &unmap_uri };

static LV2_Feature map_feature = { LV2_URID__map, &urid_map };
static LV2_Feature unmap_feature = { LV2_URID__unmap, &urid_unmap };

static const LV2_Feature* features[4] = {
    &map_feature,
    &unmap_feature,
    NULL
};

static intptr_t dispatcher (AEffect* aeffect, int opcode, int index, intptr_t integer, void* data, float flt) {
    BytesVST* vst = (BytesVST*) aeffect->ptr3;
    Bytes* self = (Bytes*) vst->handle;
    
    VstEvents* events;
    uint8_t msg[3];
    
    switch ((const int) opcode) {
    case effProcessEvents:
        events = (VstEvents*) data;
        if (!events) {
            break;
        }
        
        if (!events->numEvents) {
            break;
        }
        
        lv2_evbuf_reset (vst->evbuf, true);
        LV2_Evbuf_Iterator iter = lv2_evbuf_begin (vst->evbuf);
        
        for (uint32_t i = 0; i < events->numEvents; ++i) {
            const VstMidiEvent* const ev = (const VstMidiEvent* const) events->events[i];
            
            msg[0] = ev->midiData[0] & 0xf0;
            msg[1] = ev->midiData[1] & 0x7f;
            msg[2] = ev->midiData[2] & 0x7f;
            
            lv2_evbuf_write (&iter, ev->deltaFrames, 0, midi_MidiEvent, 3, msg);
        }
        break;
    
    case effSetSampleRate:
        self->rate = (double) flt;
        break;
    
    case effGetParamName:
        if (data) {
            strncpy (data, ranges[index].name, 8);
            return 0;
        }
        break;
    
    case effMainsChanged:
        switch (integer) {
        case 1:
            vst->descriptor->activate (vst->handle);
            break;
        
        case 0:
            vst->descriptor->deactivate (vst->handle);
            break;
        
        default:
            break;
        }
        break;
    
    case effOpen:
        break;
    
    case effCanDo:
        if (!strcmp ((const char*) data, "receiveVstEvents")) {
            return 1;
        } else if (!strcmp ((const char*) data, "receiveVstMidiEvent")) {
            return 1;
        }
        break;
    
    default:
        break;
    }
    
    return 0;
}

static void setParameter (AEffect* aeffect, int index, float value) {
    BytesVST* vst = (BytesVST*) aeffect->ptr3;
    vst->ports[index][0] = value_to_lv2 (value, index);
}

static float getParameter (AEffect* aeffect, int index) {
    BytesVST* vst = (BytesVST*) aeffect->ptr3;
    return value_to_vst (vst->ports[index][0], index);
}

static void processReplacing (AEffect* aeffect, float** inputs, float** outputs, int nframes) {
    BytesVST* vst = (BytesVST*) aeffect->ptr3;
    
    for (uint32_t i = 0; i < NUM_PORTS; ++i) {
        switch (i) {
        case CONTROL:
            vst->descriptor->connect_port (vst->handle, i, lv2_evbuf_get_buffer (vst->evbuf));
            break;
        
        case LOUT:
        case ROUT:
            vst->descriptor->connect_port (vst->handle, i, outputs[i - LOUT]);
            break;
        
        default:
            vst->descriptor->connect_port (vst->handle, i, vst->ports[i - PARAMOFFSET]);
            break;
        }
    }
    
    vst->descriptor->run (vst->handle, nframes);
    lv2_evbuf_reset (vst->evbuf, true);
}

LV2_SYMBOL_EXPORT const AEffect* VSTPluginMain (audioMasterCallback master) {
    AEffect* self = malloc (sizeof (AEffect));
    BytesVST* vst = malloc (sizeof (BytesVST));
    
    vst->descriptor = (LV2_Descriptor*) lv2_descriptor (0);
    vst->evbuf = lv2_evbuf_new (1024, LV2_EVBUF_ATOM, atom_Chunk, atom_Sequence);
    lv2_evbuf_reset (vst->evbuf, true);
    
    vst->handle = vst->descriptor->instantiate (vst->descriptor, 48000, NULL, features);
    
    for (uint32_t i = 0; i < NUM_PORTS - PARAMOFFSET; ++i) {
        vst->ports[i][0] = ranges[i].def;
    }
    
    self->magic = kEffectMagic;
    self->dispatcher = dispatcher;
    self->process = NULL;
    self->setParameter = setParameter;
    self->getParameter = getParameter;
    self->numPrograms = 0;
    self->numParams = NUM_PORTS - PARAMOFFSET;
    self->numInputs = 0;
    self->numOutputs = 2;
    self->flags = effFlagsCanReplacing | effFlagsIsSynth;
    self->ptr1 = NULL;
    self->ptr2 = NULL;
    self->ptr3 = vst;
    self->user = NULL;
    self->uniqueID = 0xf008a9ed;
    self->processReplacing = processReplacing;
    
    return self;
}

#ifdef WTF_BITWIG

/* seriously: why? */
LV2_SYMBOL_EXPORT const AEffect* main (audioMasterCallback master) {
    return VSTPluginMain (master);
}

#endif
