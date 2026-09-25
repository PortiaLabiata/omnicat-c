#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

#define TO_MAX 128

typedef struct {
    int fdin, fdout;
    uint8_t *rxbuf;
    unsigned int id;
    unsigned int to[TO_MAX];
    unsigned int to_size;
} TransportCommon;

#endif
