
#include "base.h"
#include "voice.h"
#include "eg.h"

void bytes_voice_init (Bytes_Voice* self, double rate) {
    self->rate = rate;
}

void bytes_voice_next (Bytes_Voice* self, double hz) {
    self->phase += lrint (MAX_PHASE / (self->rate / hz));
}
