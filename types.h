#ifndef PARAMS_H
#define PARAMS_H

#include <stdbool.h>
#include <stdint.h>
#define STR_MAX 4096

typedef struct {
    char name[STR_MAX];
} NameArg;

typedef enum {
    ADDR_UDP,
    ADDR_TCP,
    ADDR_SIZE,
} AddrType;

// Ordering here is very important!
typedef struct {
    char addr[32];
    AddrType type;
    uint16_t port;
} AddrArg;

typedef enum {
    TRANSP_STDIO,
    TRANSP_NAME,
    TRANSP_UDP,
    TRANSP_TCP,
    TRANSP_SIZE,
} TransportKind;

typedef struct {
    bool nobuf;
} TransportOptions;

typedef struct {
    union {
        NameArg name;
        AddrArg addr;
    } address;
    TransportKind label;
    TransportOptions options;
} TransportParams;

typedef struct {
    char *rx_buffer;
    char *tx_buffer;
} TransportStdio;

typedef struct {
    union {
        TransportStdio stdio;
    } value;
    TransportKind kind;
} Transport;

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

#endif
