#ifndef TRANSPORT_H
#define TRANSPORT_H

#include "types.h"

int transport_create(Transport *t, TransportParams *p);
int transport_do(Transport *t);
int transport_write(Transport *t, uint8_t *data, int size);
int transport_poll(Transport t[], void cb(int i), int n, int timeout);
void transport_free(Transport *t);

#endif
