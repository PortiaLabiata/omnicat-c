#ifndef COMMON_H
#define COMMON_H

typedef struct {
    int fdin, fdout;
    char *rxbuf;
    unsigned int rxbuf_size;
    unsigned int id;
} TransportCommon;

#endif
