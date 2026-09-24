#include <stdio.h>
#include "transport.h"

int main(int argc, char **argv)
{
    (void)argc; (void)argv;

    Transport t1 = {0};
    t1.kind = TRANSPORT_KIND_STDIO;
    t1.options.rxbuf_size = 1024;

    Transport t2 = {0};
    t2.kind = TRANSPORT_KIND_STDIO;
    t2.options.rxbuf_size = 1024;

    transport_init(&t1);
    transport_init(&t2);

    TransportSelect s1 = {0};
    s1.t = &t1;
    TRANSPORT_SEL_CLEAR(s1);

    while (1)
    {
        int res = transport_select(&s1, 1, 1);
        if (res > 0)
        {
            TRANSPORT_SEL_CLEAR(s1);
            int ret = transport_read(&t1, t1.common.rxbuf, t1.options.rxbuf_size);
            transport_write(&t1, t1.common.rxbuf, ret);
        }
    }

    return 0;
}
