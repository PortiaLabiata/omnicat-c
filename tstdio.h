#ifndef TSTDIO_H
#define TSTDIO_H

#include "types.h"
#include <stdint.h>

int transport_create_stdio(TransportParams *p, Transport *t);
int transport_do_stdio(Transport *t);
int transport_write_stdio(Transport *t, uint8_t *data, int size);
void transport_free_stdio(Transport *t);

#endif
