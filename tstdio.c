#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include "tstdio.h"

int transport_create_stdio(TransportParams *p, Transport *t)
{
    t->common.fd = 0;
    return 0;
}

int transport_do_stdio(Transport *t)
{
    return read(0, t->common.rx_buffer, t->common.rxbuf_size);
}

int transport_write_stdio(Transport *t, uint8_t *data, int size)
{
    write(1, data, size);
    return 0;
}

void transport_free_stdio(Transport *t)
{
    (void)t;
}
