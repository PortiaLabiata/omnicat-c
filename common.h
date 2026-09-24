#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

typedef struct {
    int fdin, fdout;
    uint8_t *rxbuf;
    unsigned int id;
} TransportCommon;

#endif
