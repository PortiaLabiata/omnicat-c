#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "tudp.h"

int transport_create_udp(TransportUDP *s, TransportCommon *c, Options *o)
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        fprintf(stderr, "Failed to init socket: %s\n",
                strerror(errno));
        return 1;
    }

    c->fdin = c->fdout = fd;
    struct sockaddr_in addr = {0};
    addr.sin_port = htons(o->port);

    if (o->server)
    {
        if (inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr) != 1)
        {
            fprintf(stderr, "Failed to parse hardcoded address, somehow\n");
            return 1;
        }
    }
    else
    {
        if (inet_pton(AF_INET, o->addr, &addr.sin_addr) != 1)
        {
            fprintf(stderr, "Failed to parse socket address %s\n",
                    o->addr);
            return 1;
        }
    }

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)))
    {
        fprintf(stderr, "Failed to bind socket: %s\n",
                strerror(errno));
        return 1;
    }
}
