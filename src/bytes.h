
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
    
    BYTE1_SYNC,
    BYTE2_SYNC,
    BYTE3_SYNC,
    BYTE4_SYNC,
    
    BYTE1_MOD,
    BYTE2_MOD,
    BYTE3_MOD,
    BYTE4_MOD,
    
    BYTE1_GAIN,
    BYTE2_GAIN,
    BYTE3_GAIN,
    BYTE4_GAIN,
    
    EG1_ATTACK,
    EG1_DECAY,
    EG1_SUSTAIN,
    EG1_RELEASE,
    
    EG2_ATTACK,
    EG2_DECAY,
    EG2_SUSTAIN,
    EG2_RELEASE,
    
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
        const float* byte1_sync;
        const float* byte2_sync;
        const float* byte3_sync;
        const float* byte4_sync;
        const float* byte1_mod;
        const float* byte2_mod;
        const float* byte3_mod;
        const float* byte4_mod;
        const float* byte1_gain;
        const float* byte2_gain;
        const float* byte3_gain;
        const float* byte4_gain;
        const float* eg1_attack;
        const float* eg1_decay;
        const float* eg1_sustain;
        const float* eg1_release;
        const float* eg2_attack;
        const float* eg2_decay;
        const float* eg2_sustain;
        const float* eg2_release;
    } ports;
    
    LV2_URID_Map* map;
    
    struct {
        LV2_URID midi_MidiEvent;
    } uris;
    
    uint8_t bytes[4];
    float sync[4];
    float mod[4];
    float gain[4];
    
    Bytes_Voice voices[NVOICES];
    double rate;
};

Bytes* bytes_new (double rate);
void bytes_note_on (Bytes* self, uint8_t key, uint8_t velocity);
void bytes_note_off (Bytes* self, uint8_t key, uint8_t velocity);
void bytes_render (Bytes* self, uint32_t start, uint32_t end);

#endif
