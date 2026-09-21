#ifndef TRANSPORT_H
#define TRANSPORT_H

#include "types.h"

int transport_create(Transport *t, TransportParams *p);
int transport_do(Transport *t);

#endif
