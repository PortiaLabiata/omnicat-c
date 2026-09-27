#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>

#define NAME_SIZE_MAX 64
#define ADDR_SIZE_MAX 128

typedef struct {
    char name[NAME_SIZE_MAX];
    char addr[ADDR_SIZE_MAX];
    int rxbuf_size;

    bool server;
    unsigned int port;
    bool reuseaddr;
    int so_rcvbuf;
    int so_sndbuf;

    unsigned int baud_rate;
    char parity[16];
    int stop_bits;
    bool echo;
    bool canon;
    bool raw;
} Options;

int options_set_defaults(Options *o);

#endif
