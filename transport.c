#include <malloc.h>
#include <stdio.h>
#include <alloca.h>
#include <unistd.h>
#include <sys/select.h>
#include "transport.h"

int transport_init(Transport *t)
{
    t->common.rxbuf = malloc(t->options.rxbuf_size);
    if (!t->common.rxbuf)
    {
        fprintf(stderr, "Failed to allocate buffer for transport %s\n",
                t->options.name);
        return 1;
    }

    switch (t->kind)
    {
        case TRANSPORT_KIND_STDIO:
            return transport_create_stdio(&t->value.stdio, &t->common, &t->options);
        case TRANSPORT_KIND_SIZE:
            fprintf(stderr, "Invalid transport kind %d for transport %s\n",
                    (int)t->kind, t->options.name);
            return 1;
        default:
            return 1;
    }
}

void transport_deinit(Transport *t)
{
    free(t->common.rxbuf);
    switch (t->kind)
    {
        case TRANSPORT_KIND_STDIO:
            transport_deinit_stdio(&t->value.stdio);
            break;
        case TRANSPORT_KIND_SIZE:
            fprintf(stderr, "Invalid transport kind %d for transport %s\n",
                    (int)t->kind, t->options.name);
            break;
        default:
            break;
    }
}

int transport_read(Transport *t, uint8_t *buf, unsigned int size)
{
    return read(t->common.fdin, buf, size); 
}

int transport_write(Transport *t, uint8_t *buf, unsigned int size)
{
    return write(t->common.fdout, buf, size); 
}

int transport_select(TransportSelect *t, int n, int tout_ms)
{
    fd_set rfds, wfds, efds;

    FD_ZERO(&rfds);
    for (int i = 0; i < n; i++)
    {
        FD_SET(t[i].t->common.fdin, &rfds);
    }

    FD_ZERO(&wfds);
    for (int i = 0; i < n; i++)
    {
        FD_SET(t[i].t->common.fdout, &wfds);
    }

    FD_ZERO(&efds);
    for (int i = 0; i < n; i++)
    {
        FD_SET(t[i].t->common.fdin, &efds);
    }

    struct timeval dt = {
        .tv_sec = 0,
        .tv_usec = tout_ms * 1000,
    };

    int ret = select(n, &rfds, &wfds, &efds, &dt);
    if (ret > 0)
    {
        for (int i = 0; i < n; i++)
        {
            if (FD_ISSET(t[i].t->common.fdin, &rfds))
            {
                t[i].bitmask |= SELECT_READ;
            }

            if (FD_ISSET(t[i].t->common.fdout, &wfds))
            {
                t[i].bitmask |= SELECT_READ;
            }

            if (FD_ISSET(t[i].t->common.fdin, &efds))
            {
                t[i].bitmask |= SELECT_READ;
            }
        }
    }
    return ret;
}
