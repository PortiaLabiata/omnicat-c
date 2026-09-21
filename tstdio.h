#ifndef TSTDIO_H
#define TSTDIO_H

#include "types.h"

int transport_create_stdio(TransportParams *p, Transport *t);
int transport_do_stdio(Transport *t);

#endif
