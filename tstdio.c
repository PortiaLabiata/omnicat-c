#include <stdio.h>
#include "tstdio.h"

int transport_create_stdio(TransportStdio *s, TransportCommon *c, Options *o)
{
    (void)s; (void)o;
    if (o->reuseaddr || o->so_rcvbuf > 0 || o->so_sndbuf > 0 ||
        o->server || o->port > 0)
    {
        fprintf(stderr, "Error creating stdio transport: invalid options for stdio set\n");
        return -1;
    }


    c->fdin = 0;
    c->fdout = 1;
    return 0;
}

void transport_deinit_stdio(TransportStdio *s)
{
    (void)s;
}
