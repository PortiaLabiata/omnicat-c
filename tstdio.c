#include "tstdio.h"

int transport_create_stdio(TransportStdio *s, TransportCommon *c, Options *o)
{
    (void)s; (void)o;
    c->fdin = 0;
    c->fdout = 1;
    return 0;
}

void transport_deinit_stdio(TransportStdio *s)
{
    (void)s;
}
