
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
        BYTES_CONNECT (BYTE1_SYNC, byte1_sync, const float*);
        BYTES_CONNECT (BYTE2_SYNC, byte2_sync, const float*);
        BYTES_CONNECT (BYTE3_SYNC, byte3_sync, const float*);
        BYTES_CONNECT (BYTE4_SYNC, byte4_sync, const float*);
        BYTES_CONNECT (BYTE1_MOD, byte1_mod, const float*);
        BYTES_CONNECT (BYTE2_MOD, byte2_mod, const float*);
        BYTES_CONNECT (BYTE3_MOD, byte3_mod, const float*);
        BYTES_CONNECT (BYTE4_MOD, byte4_mod, const float*);
        BYTES_CONNECT (BYTE1_GAIN, byte1_gain, const float*);
        BYTES_CONNECT (BYTE2_GAIN, byte2_gain, const float*);
        BYTES_CONNECT (BYTE3_GAIN, byte3_gain, const float*);
        BYTES_CONNECT (BYTE4_GAIN, byte4_gain, const float*);
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
    
    self->sync[0] = *(self->ports.byte1_sync);
    self->sync[1] = *(self->ports.byte2_sync);
    self->sync[2] = *(self->ports.byte3_sync);
    self->sync[3] = *(self->ports.byte4_sync);
    
    self->mod[0] = *(self->ports.byte1_mod);
    self->mod[1] = *(self->ports.byte2_mod);
    self->mod[2] = *(self->ports.byte3_mod);
    self->mod[3] = *(self->ports.byte4_mod);
    
    self->gain[0] = *(self->ports.byte1_gain);
    self->gain[1] = *(self->ports.byte2_gain);
    self->gain[2] = *(self->ports.byte3_gain);
    self->gain[3] = *(self->ports.byte4_gain);
    
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
