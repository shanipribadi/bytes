
#ifndef BYTES__BASE_H
#define BYTES__BASE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "lv2/lv2plug.in/ns/ext/atom/atom.h"
#include "lv2/lv2plug.in/ns/ext/atom/util.h"
#include "lv2/lv2plug.in/ns/ext/midi/midi.h"
#include "lv2/lv2plug.in/ns/ext/urid/urid.h"
#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

#include "fastonebigheader.h"

#define BYTES_URI "http://grejppi.github.io/plugins/bytes"

#define key2hz(k) (440.0 * (pow (2.0, (k - 69) / 12.0)))

#define OVERSAMPLING 4
#define NVOICES 16
#define INVALID_KEY 255

#ifndef M_PI
#   define M_PI 3.14159265358979323846
#endif

#define MAX_PHASE 4294967295.0

#endif
