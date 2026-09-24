#ifndef OPTIONS_H
#define OPTIONS_H

#define NAME_SIZE_MAX 64

typedef struct {
    int rxbuf_size;
    char name[NAME_SIZE_MAX];
} Options;

int options_set_defaults(Options *o);

#endif
