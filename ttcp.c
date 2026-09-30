#include <errno.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "ttcp.h"
#include "common.h"

int transport_create_tcp(TransportTCP *s, TransportCommon *c, Options *o)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        fprintf(stderr, "Failed to allocate a TCP socket: %s\n",
                strerror(errno));
        return -1;
    }

    c->fdin = c->fdout = fd;
    s->server = o->server;

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    s->connected = false;

    s->peer_size = sizeof(s->peer_addr);

    if (o->server)
    {
        addr.sin_port = htons(o->port);

        if (inet_pton(AF_INET, o->addr, &addr.sin_addr) != 1)
        {
            fprintf(stderr, "Failed to parse address %s\n",
                    o->addr);
            return -1;
        }
    }
    else
    {
        s->peer_addr.sin_port = htons(o->port);
        s->peer_addr.sin_family = AF_INET;
        if (inet_pton(AF_INET, o->addr, &s->peer_addr.sin_addr) != 1)
        {
            fprintf(stderr, "Failed to parse address %s\n",
                    o->addr);
            return -1;
        }
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

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)))
    {
        fprintf(stderr, "Failed to bind socket: %s\n",
                strerror(errno));
        return -1;
    }

    if (o->server)
    {
        if (listen(fd, 1) == -1)
        {
            fprintf(stderr, "Failed to set socket to listen mode: %s\n",
                    strerror(errno));
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

int transport_read_tcp(TransportTCP *s, TransportCommon *c, uint8_t *buf, unsigned int size)
{
    if (!s->connected)
    {
        if (s->server)
        {
            // We already polled with POLLIN, so surely 
            // we have an incoming connection
            int ret = accept(c->fdin, (struct sockaddr*)&s->peer_addr, &s->peer_size);
            if (ret == -1)
            {
                fprintf(stderr, "Failed to accept TCP connection: %s\n",
                        strerror(errno));
                return -1;
            }
            s->peerfd = ret;
            s->connected = true;
            return 0;
        }
        else
        {
            // Can't read if not connected and connection attempts
            // will be done on write calls
            return 0;
        }
    }
    else 
    {
        return read(s->peerfd, buf, size);
    }
}

int transport_write_tcp(TransportTCP *s, TransportCommon *c, uint8_t *buf, unsigned int size)
{
    if (!s->connected)
    {
        if (s->server)
        {
            // Can't write if there is noone to write to
            return 0;
        }
        else
        {
            int ret = connect(c->fdin, (struct sockaddr*)&s->peer_addr, sizeof(s->peer_addr));
            if (ret == -1)
            {
                fprintf(stderr, "Failed to connect to TCP peer: %s\n",
                        strerror(errno));
                return -1;
            }
            s->connected = true;
            s->peerfd = ret;
        }
    }
    return write(s->peerfd, buf, size);
}

void transport_deinit_tcp(TransportTCP *s, TransportCommon *c)
{
    (void)s; (void)c;
}
