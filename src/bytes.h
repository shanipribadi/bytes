
#ifndef BYTES__H
#define BYTES__H

#include "base.h"
#include "voice.h"

typedef struct Bytes_ Bytes;

typedef enum {
    CONTROL,
    LOUT,
    ROUT,
    BYTE1,
    BYTE2,
    BYTE3,
    BYTE4,
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
    } ports;
    
    LV2_URID_Map* map;
    
    struct {
        LV2_URID midi_MidiEvent;
    } uris;
    
    uint8_t bytes[4];
    Bytes_Voice voices[NVOICES];
    double rate;
};

Bytes* bytes_new (double rate);
void bytes_note_on (Bytes* self, uint8_t key, uint8_t velocity);
void bytes_note_off (Bytes* self, uint8_t key, uint8_t velocity);
void bytes_render (Bytes* self, uint32_t start, uint32_t end);

#endif
