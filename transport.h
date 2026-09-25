#ifndef TRANSPORT_H
#define TRANSPORT_H

#include "common.h"
#include "options.h"

#include "tstdio.h"
#include "tudp.h"

#include <stdint.h>

#define TRANSPORT_SEL_CLEAR(t) (t.bitmask = 0)

typedef enum {
    TRANSPORT_KIND_STDIO,
    TRANSPORT_KIND_UDP,
    TRANSPORT_KIND_SIZE
} TransportKind;

typedef struct {
    Options options; 
    TransportCommon common;

    union {
        TransportStdio stdio;
        TransportUDP udp;
    } value;

    TransportKind kind;
} Transport;

typedef enum {
    SELECT_READ     = 1,
    SELECT_WRITE    = 2,
    SELECT_ERR      = 4,
    SELEC_MAX       = 8
} TransportSelectBitmask;

typedef struct {
    Transport *t;
    int bitmask; 
} TransportSelect;

int transport_init(Transport *t);
int transport_select(TransportSelect *t, int n, int tout_ms);
int transport_read(Transport *t, uint8_t *buf, unsigned int size);
int transport_write(Transport *t, uint8_t *buf, unsigned int size);
void transport_deinit(Transport *t);

#endif
