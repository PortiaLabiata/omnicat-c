#ifndef TRANSPORT_H
#define TRANSPORT_H

#include "common.h"
#include "options.h"

#include "tstdio.h"
#include "tudp.h"
#include "tfile.h"

#include <stdint.h>

typedef enum {
    TRANSPORT_KIND_STDIO,
    TRANSPORT_KIND_UDP,
    TRANSPORT_KIND_FILE,
    TRANSPORT_KIND_SIZE
} TransportKind;

typedef struct {
    Options options; 
    TransportCommon common;

    union {
        TransportStdio stdio;
        TransportUDP udp;
        TransportFile file;
    } value;

    TransportKind kind;
} Transport;

int transport_init(Transport *t);
int transport_read(Transport *t, uint8_t *buf, unsigned int size);
int transport_write(Transport *t, uint8_t *buf, unsigned int size);
void transport_deinit(Transport *t);

#endif
