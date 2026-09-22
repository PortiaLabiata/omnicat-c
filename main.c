#include <alloca.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "dictionary.h"
#include "types.h"
#include "transport.h"
#include "iniparser.h"

#define NUM_TRANSPORTS 2

TransportKind str2kind(const char *s)
{
    if (strcmp(s, "stdio") == 0)
    {
        return TRANSP_STDIO; 
    }

    return TRANSP_SIZE;
}

void params_default(TransportParams *p)
{
    memset(p, 0, sizeof(TransportParams));
    *p = (TransportParams) {
        .label = TRANSP_STDIO,
        .options = {
            .nobuf = false,
            .rxbuf_size = 1024,
        },
    };
}

static char id_lookup[TRANSP_MAX][TRANSP_NAME_MAX] = {0};
static int num_transports = 0;

void config_init_to(const char *to, TransportParams *p)
{
    const char *ptr = to;
    char *c = NULL;
    int to_offset = 0;

    while ((c = strchr(ptr, ',')))
    {
        char *end = strchr(c, ',');
        bool valid_key = false;

        for (int i = 0; i < num_transports; i++)
        {
            if (strncmp(c, id_lookup[i], end - c) == 0)
            {
                p->to[to_offset] = i;
                valid_key = true;
                break;
            }
        }

        if (!valid_key)
        {
            char *name = alloca(TRANSP_NAME_MAX);
            snprintf(name, end - c, "%s", c);

            printf("Invalid transport destination: %s\n",
                   name);
            exit(1);
        }

        ptr = c+1;
        to_offset++;
    }
    p->to_size = to_offset;
}

void config_init_keys(dictionary *d, const char **keys, int num_keys, TransportParams p[])
{
    for (int i = 0; i < num_keys; i++)
    {
        const char *key_raw = keys[i];
        const char *key = strchr(key_raw, ':') + 1;

        if (strcmp(key, "kind") == 0)
        {
            const char *value = iniparser_getstring(d, key_raw, "stdio");
            printf("Kind: %s\n", value);
            if (value)
            {
                p[i].label = str2kind(value); 
            }
        }

        if (strcmp(key, "addr") == 0)
        {
            const char *value = iniparser_getstring(d, key_raw, "stdio");
            if (value)
            {
                strncpy(p[i].address.name.name, value, strlen(value));
            }
        }

        if (strcmp(key, "rxbuf") == 0)
        {
            int value = iniparser_getint(d, key_raw, -1);
            if (value <= 0)
            {
                fprintf(stderr, "Failed to parse \"%s\": invalid value",
                        key_raw);
                exit(1);
            }
            else
            {
                p[i].options.rxbuf_size = value;
            }
        }

        if (strcmp(key, "to") == 0)
        {
            const char *value = iniparser_getstring(d, key_raw, NULL);
            config_init_to(value, p); 
        }
    }
}

void config_init(dictionary *d, TransportParams p[])
{
    for (int i = 0; i < num_transports; i++)
    {
        const char *transport_name = iniparser_getsecname(d, i);
        printf("Found transport %s\n", 
               transport_name);

        strncpy(id_lookup[i], transport_name, TRANSP_NAME_MAX);

        const int num_keys = iniparser_getsecnkeys(d, transport_name);
        const char **keys = alloca(num_keys);
        if (!iniparser_getseckeys(d, transport_name, keys))
        {
            fprintf(stderr, "Failed to init transports\n");
            exit(1);
        }

        config_init_keys(d, keys, num_keys, p); 
    }
}

Transport *transports = NULL;

void transport_available_cb(int i)
{
    Transport *t = &transports[i];
    int ret = transport_do(t);

    for (int j = 0; j < t->to_size; j++)
    {
        printf("%d\n", t->to[j]);
        transport_write(&transports[t->to[j]], (uint8_t*)t->common.rx_buffer, ret);
    }
}

int main(int argc, char **argv) 
{
    if (argc < 2)
    {
        fprintf(stderr, "Not enough arguments: need config filename\n");
        exit(1);
    }

    dictionary *ini = iniparser_load(argv[1]);
    if (!ini)
    {
        fprintf(stderr, "Failed to load config file %s\n", argv[1]);
        exit(1);
    }

    printf("Opened config file %s OK\n", argv[1]);
    num_transports = iniparser_getnsec(ini);

    TransportParams *params = malloc(num_transports*sizeof(TransportParams));
    for (int i = 0; i < num_transports; i++)
    {
        params_default(&params[i]);
    }

    config_init(ini, params); 

    transports = malloc(num_transports*sizeof(Transport));
    for (int i = 0; i < num_transports; i++)
    {
        transport_create(&transports[i], &params[i]);
    }

    while (1)
    {
        transport_poll(transports, transport_available_cb, num_transports, 1);        
    }

    return 0;
}
