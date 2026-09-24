#include <string.h>
#include "options.h"

int options_set_defaults(Options *o)
{
    if (!o)
    {
        return -1;
    }

    strcpy(o->name, "");
    o->rxbuf_size = 1024;
    return 0;
}
