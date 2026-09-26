#ifndef TFILE_H
#define TFILE_H

#include <stdio.h>

#include "common.h"
#include "options.h"

typedef struct {
    int placeholder;
} TransportFile;

int transport_create_file(TransportFile *s, TransportCommon *c, Options *o);
void transport_deinit_file(TransportFile *s);

#endif
