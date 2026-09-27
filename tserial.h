#ifndef TSERIAL_H
#define TSERIAL_H

#include <termios.h>
#include "common.h"
#include "options.h"

typedef struct {
    struct termios initial_term;
} TransportSerial;

int transport_create_serial(TransportSerial *s, TransportCommon *c, Options *o);
void transport_deinit_serial(TransportSerial *s, TransportCommon *c);

#endif
