#include <poll.h>
#include <alloca.h>

#include "types.h"
#include "tstdio.h"

int transport_create(Transport *t, TransportParams *p)
{
    t->common.rxbuf_size = p->options.rxbuf_size;

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

void transport_free(Transport *t)
{
    switch (t->kind)
    {
        case TRANSP_STDIO:
            transport_free_stdio(t);
        default:
            break;
    }
}

int transport_write(Transport *t, uint8_t *data, int size)
{
    switch (t->kind)
    {
        case TRANSP_STDIO:
            return transport_write_stdio(t, data, size);
        default:
            return 1;
    }
}

int transport_poll(Transport t[], void cb(int i), int n, int timeout)
{
    struct pollfd *fds = alloca(n*sizeof(struct pollfd)); 
    for (int i = 0; i < n; i++)
    {
        fds[i].events = POLLIN;
        fds[i].fd = t[i].common.fd;
        fds[i].revents = 0;
    }

    int ret = poll(fds, n, timeout);
    if (ret > 0)
    {
        for (int i = 0; i < n; i++)
        {
            if (fds[i].revents & POLLIN)
            {
                cb(i);
            }
        }
    } 
    else if (ret == -1)
    {
        return -1;
    }
    return 0;
}

