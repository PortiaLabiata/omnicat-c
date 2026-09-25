#ifndef TUDP_H
#define TUDP_H

#include "common.h"
#include "options.h"

typedef struct {
    int placeholder;
} TransportUDP;

int transport_create_udp(TransportUDP *s, TransportCommon *c, Options *o);
int transport_deinit_udp(TransportUDP *s);

#endif
