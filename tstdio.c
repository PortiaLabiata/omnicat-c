#include "tstdio.h"

int transport_create_stdio(TransportParams *p, Transport *t)
{
    TransportStdio *s = &t->value.stdio;
    if (p->options.nobuf)
    {
        s->nobuf = true;
    }
    return 0;
}

int transport_do_stdio(Transport *t)
{
    (void)t;
    return 0;   
}

