#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>

#define NAME_SIZE_MAX 64
#define ADDR_SIZE_MAX 128

typedef struct {
    int rxbuf_size;
    bool server;
    unsigned int port;
    char name[NAME_SIZE_MAX];
    char addr[ADDR_SIZE_MAX];
} Options;

int options_set_defaults(Options *o);

#endif
