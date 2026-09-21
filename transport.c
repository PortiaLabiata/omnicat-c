#include "types.h"
#include "tstdio.h"

int transport_create(Transport *t, TransportParams *p)
{
    switch (p->label)
    {
    case TRANSP_STDIO:
        t->kind = TRANSP_STDIO;
        return transport_create_stdio(p, t);
    default:
        return 1;
    }
}

int transport_do(Transport *t)
{
    switch (t->kind)
    {
        case TRANSP_STDIO:
            return transport_do_stdio(t);
        default:
            return 1;
    }
}
