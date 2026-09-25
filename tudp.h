#ifndef TUDP_H
#define TUDP_H

#include "common.h"
#include "options.h"

#include <stdbool.h>
#include <stdint.h>
#include <netinet/in.h>
#include <unistd.h>

typedef struct {
    struct sockaddr_in peer_addr;
    socklen_t addr_len;
    bool server;
    bool connected;
} TransportUDP;

int transport_create_udp(TransportUDP *s, TransportCommon *c, Options *o);
int transport_read_udp(TransportUDP *s, TransportCommon *c, uint8_t *buf, unsigned int size);
int transport_write_udp(TransportUDP *s, TransportCommon *c, uint8_t *buf, unsigned int size);
void transport_deinit_udp(TransportUDP *s);

#endif
