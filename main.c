#include <alloca.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "args.h"
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

        if (strcmp(key, "nobuf") == 0)
        {
            int value = iniparser_getboolean(d, key_raw, -1);
            if (value != -1)
            {
                p[i].options.nobuf = value;
            }
            else 
            {
                fprintf(stderr, "Invalid value of \"%s\": %s\n",
                        key_raw, iniparser_getstring(d, key_raw, ""));
                exit(1);
            }
        }
    }
}

void config_init(dictionary *d, TransportParams p[])
{
    const int num_transports = iniparser_getnsec(d);
    for (int i = 0; i < num_transports; i++)
    {
        const char *transport_name = iniparser_getsecname(d, i);
        printf("Found transport %s\n", 
               transport_name);

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

    const int num_transports = iniparser_getnsec(ini);
    TransportParams *params = malloc(num_transports*sizeof(TransportParams));
    config_init(ini, params); 

    Transport *transports = malloc(num_transports*sizeof(Transport));
    for (int i = 0; i < num_transports; i++)
    {
        transport_create(&transports[i], &params[i]);
    }

    return 0;
}
