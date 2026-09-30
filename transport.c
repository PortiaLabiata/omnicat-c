#include <malloc.h>
#include <stdio.h>
#include <alloca.h>
#include <unistd.h>
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
        case TRANSPORT_KIND_UDP:
            return transport_create_udp(&t->value.udp, &t->common, &t->options);
        case TRANSPORT_KIND_FILE:
            return transport_create_file(&t->value.file, &t->common, &t->options);
        case TRANSPORT_KIND_SERIAL:
            return transport_create_serial(&t->value.serial, &t->common, &t->options);
        case TRANSPORT_KIND_TCP:
            return transport_create_tcp(&t->value.tcp, &t->common, &t->options);
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
    switch (t->kind)
    {
        case TRANSPORT_KIND_STDIO:
            transport_deinit_stdio(&t->value.stdio);
            break;
        case TRANSPORT_KIND_UDP:
            transport_deinit_udp(&t->value.udp);
            break;
        case TRANSPORT_KIND_FILE:
            transport_deinit_file(&t->value.file);
            break;
        case TRANSPORT_KIND_SERIAL:
            transport_deinit_serial(&t->value.serial, &t->common);
            break;
        case TRANSPORT_KIND_TCP:
            transport_deinit_tcp(&t->value.tcp, &t->common);
            break;
        case TRANSPORT_KIND_SIZE:
            fprintf(stderr, "Invalid transport kind %d for transport %s\n",
                    (int)t->kind, t->options.name);
            break;
        default:
            break;
    }
    close(t->common.fdin);
    free(t->common.rxbuf);
}

int transport_read(Transport *t, uint8_t *buf, unsigned int size)
{
    switch (t->kind)
    {
    case TRANSPORT_KIND_STDIO:
    case TRANSPORT_KIND_FILE:
    case TRANSPORT_KIND_SERIAL:
        return read(t->common.fdin, buf, size); 
    case TRANSPORT_KIND_UDP:
        return transport_read_udp(&t->value.udp, &t->common, buf, size);
    case TRANSPORT_KIND_TCP:
        return transport_read_tcp(&t->value.tcp, &t->common, buf, size);
    default:
        return 1;
    }
}

int transport_write(Transport *t, uint8_t *buf, unsigned int size)
{
    switch (t->kind)
    {
    case TRANSPORT_KIND_STDIO:
    case TRANSPORT_KIND_FILE:
    case TRANSPORT_KIND_SERIAL:
        return write(t->common.fdin, buf, size); 
    case TRANSPORT_KIND_UDP:
        return transport_write_udp(&t->value.udp, &t->common, buf, size);
    case TRANSPORT_KIND_TCP:
        return transport_write_tcp(&t->value.tcp, &t->common, buf, size);
    default:
        return 1;
    }
}

