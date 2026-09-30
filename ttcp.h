#ifndef TTCP_H
#define TTCP_H

#include "options.h"
#include "common.h"
#include <netinet/in.h>
#include <unistd.h>

typedef struct {
    bool server; 
    bool connected;
    struct sockaddr_in peer_addr;
    socklen_t peer_size;
} TransportTCP;

int transport_create_tcp(TransportTCP *s, TransportCommon *c, Options *o);
int transport_read_tcp(TransportTCP *s, TransportCommon *c, uint8_t *buf, unsigned int size);
int transport_write_tcp(TransportTCP *s, TransportCommon *c, uint8_t *buf, unsigned int size);
void transport_deinit_tcp(TransportTCP *s, TransportCommon *c);

#endif
