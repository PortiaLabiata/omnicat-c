#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include "tstdio.h"

int transport_create_stdio(TransportParams *p, Transport *t)
{
    TransportStdio *s = &t->value.stdio;
    t->common.fd = 0;
    s->rx_buffer = malloc(p->options.rxbuf_size);

    return 0;
}

int transport_do_stdio(Transport *t)
{
    TransportStdio *s = &t->value.stdio;
    return read(0, s->rx_buffer, t->common.rxbuf_size);
}

int transport_write_stdio(Transport *t, uint8_t *data, int size)
{
    (void)t; (void)data; (void)size;
    return 0;
}

void transport_free_stdio(Transport *t)
{
    free(t->value.stdio.rx_buffer);
}
