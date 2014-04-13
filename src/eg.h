
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
