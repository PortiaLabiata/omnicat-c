#include <string.h>
#include "args.h"

static const char *UDP_SUFFIX = "udp";
static const char *TCP_SUFFIX = "tcp";
static const char *STDIO_SUFFIX = "stdio";

void parse_arg(const char *s, TransportParams *params)
{
    if (memcmp(s, UDP_SUFFIX, strlen(UDP_SUFFIX)) == 0)
    {
        params->label = TRANSP_UDP;
        params->address.addr.type = ADDR_UDP;
        return;
    }

    if (memcmp(s, TCP_SUFFIX, strlen(TCP_SUFFIX)) == 0)
    {
        params->label = TRANSP_TCP;
        params->address.addr.type = ADDR_TCP;
        return;
    }

    if (memcmp(s, STDIO_SUFFIX, strlen(STDIO_SUFFIX)) == 0)
    {
        params->label = TRANSP_STDIO;
        return;
    }

    memcpy(params->address.name.name, s, strlen(s));
}
