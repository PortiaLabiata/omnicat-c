#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include "tfile.h"

int transport_create_file(TransportFile *s, TransportCommon *c, Options *o)
{
    (void)s;
    int fd = open(o->addr, O_RDWR);
    if (fd < 0)
    {
        fprintf(stderr, "Failed to open file %s: %s\n",
                o->addr, strerror(errno));
        return -1;
    }

    c->fdin = c->fdout = fd;
    return 0;
}

void transport_deinit_file(TransportFile *s)
{
    (void)s;
}
