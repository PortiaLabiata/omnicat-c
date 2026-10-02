#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <alloca.h>
#include "tserial.h"

int br2speed(int baud_rate)
{
    switch (baud_rate)
    {
    case 9600:
        return B9600;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    default:
        return -1;
    }
}

int transport_create_serial(TransportSerial *s, TransportCommon *c, Options *o)
{
#ifdef __CYGWIN__
    static const char *COM_PREFIX = "COM";
    static const char *CYGWIN_PREFIX = "/dev/ttyS";

    char *ptr = strstr(o->addr, COM_PREFIX);
    if (!ptr || ptr != o->addr)
    {
        fprintf(stderr, "Failed to open serial port: invalid name %s\n",
                o->addr);
        return -1;
    }

    ptr += strlen(COM_PREFIX);
    char *num = alloca(strlen(ptr));
    strcpy(num, ptr);

    sprintf(o->addr, "%s%s", CYGWIN_PREFIX, num);
#endif

    int fd = open(o->addr, O_RDWR);
    if (fd < 0)
    {
        fprintf(stderr, "Failed to open serial device %s: %s\n",
                o->addr, strerror(errno));
        return -1;
    }
    c->fdin = c->fdout = fd;

    tcgetattr(fd, &s->initial_term);
    struct termios attr = s->initial_term;

    if (o->raw && o->canon)
    {
        fprintf(stderr, "Failed to init serial device: cannot be canon and raw at the same time\n");
        return -1;
    }

    int speed = br2speed(o->baud_rate);
    if (speed < 0)
    {
        fprintf(stderr, "Failed to init serial device: unsupported baud rate %d\n",
                o->baud_rate);
        return -1;
    }

    cfsetspeed(&attr, speed);
    if (o->raw)
    {
        cfmakeraw(&attr);
    }
    else if (o->canon)
    {
        attr.c_lflag |= ICANON;
    }

    if (o->echo)
    {
        attr.c_lflag |= ECHO;
    }
    else
    {
        attr.c_lflag &= ~ECHO;
    }

    if (tcsetattr(fd, TCSANOW, &attr) == -1)
    {
        fprintf(stderr, "Failed to set terminal attributes: %s\n",
                strerror(errno));
        return -1;
    }
    return 0;
}

void transport_deinit_serial(TransportSerial *s, TransportCommon *c)
{
    tcsetattr(c->fdin, TCSANOW, &s->initial_term);
}
