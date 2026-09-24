#ifndef TSTDIO_H
#define TSTDIO_H

#include "common.h"
#include "options.h"
#include <stdint.h>

typedef struct {
    int placeholder;
} TransportStdio;

int transport_create_stdio(TransportStdio *s, TransportCommon *c, Options *o);
void transport_deinit_stdio(TransportStdio *s);

#endif
