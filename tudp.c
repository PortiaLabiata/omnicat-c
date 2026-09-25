#include <asm-generic/socket.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "tudp.h"

int transport_create_udp(TransportUDP *s, TransportCommon *c, Options *o)
{
    (void)s;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        fprintf(stderr, "Failed to init socket: %s\n",
                strerror(errno));
        return -1;
    }

    c->fdin = c->fdout = fd;
    s->server = o->server;

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;

    if (o->server)
    {
        addr.sin_port = htons(o->port);
        s->connected = false;
    }
    else
    {
        addr.sin_port = 0;
        s->peer_addr.sin_port = htons(o->port);
        s->peer_addr.sin_family = AF_INET;
        s->connected = true;
        if (inet_pton(AF_INET, o->addr, &s->peer_addr.sin_addr) != 1)
        {
            fprintf(stderr, "Failed to parse hardcoded address, somehow\n");
            return -1;
        }
    }

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)))
    {
        fprintf(stderr, "Failed to bind socket: %s\n",
                strerror(errno));
        return -1;
    }

    if (o->reuseaddr)
    {
        int opt = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        {
            fprintf(stderr, "Failed to set SO_REUSEADDR\n");
            return -1;
        }
    }

    if (o->so_rcvbuf > 0)
    {
        if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &o->so_rcvbuf, sizeof(o->so_rcvbuf)) < 0)
        {
            fprintf(stderr, "Failed to set SO_RCVBUF\n");
            return -1;
        }
    }

    if (o->so_sndbuf > 0)
    {
        if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &o->so_sndbuf, sizeof(o->so_sndbuf)) < 0)
        {
            fprintf(stderr, "Failed to set SO_SNDBUF\n");
            return -1;
        }
    }
    return 0;
}

int transport_read_udp(TransportUDP *s, TransportCommon *c, uint8_t *buf, unsigned int size)
{
    s->addr_len = sizeof(s->peer_addr);
    int ret = recvfrom(c->fdin, buf, size, 0, (struct sockaddr*)&s->peer_addr, &s->addr_len);
    if (ret > 0)
        s->connected = true;
    return ret;
}

int transport_write_udp(TransportUDP *s, TransportCommon *c, uint8_t *buf, unsigned int size)
{
    if (s->connected)
        return sendto(c->fdout, buf, size, 0, (struct sockaddr*)&s->peer_addr, sizeof(s->peer_addr));
    else
        return 0;
}

void transport_deinit_udp(TransportUDP *s)
{
    (void)s;
}
