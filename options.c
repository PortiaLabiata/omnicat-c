#include <string.h>
#include "options.h"

int options_set_defaults(Options *o)
{
    if (!o)
    {
        return -1;
    }

    strcpy(o->name, "");
    strcpy(o->addr, "127.0.0.1");

    o->rxbuf_size = 1024;
    o->port = 0;

    return 0;
}
