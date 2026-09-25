#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <malloc.h>
#include <alloca.h>

#include "cJSON/cJSON.h"
#include "transport.h"
#include "json.h"

int main(int argc, char **argv)
{
    (void)argc; (void)argv;
    int ret = 0;

    if (argc < 2)
    {
        fprintf(stderr, "Invalid number of arguments: file name required\n");
        return 1;
    }

    FILE *config_file = fopen(argv[1], "r");
    if (!config_file)
    {
        fprintf(stderr, "Failed to open file %s: %s\n",
                argv[1], strerror(errno));
        return 1;
    }

    fseek(config_file, 0, SEEK_END);
    unsigned long size = ftell(config_file);
    fseek(config_file, 0, SEEK_SET);

    char *buffer = malloc(size);
    if (!buffer)
    {
        fprintf(stderr, "Failed to load file into memory: %s\n",
                strerror(errno));
        return 1;
    }

    if (fread(buffer, 1, size, config_file) != size)
    {
        fprintf(stderr, "Failed to read file: %s\n",
                strerror(errno));
        return 1;
    }

    cJSON *json = cJSON_Parse(buffer);
    if (!json)
    {
        fprintf(stderr, "Failed to parse config file at %s\n",
                cJSON_GetErrorPtr());
        ret = 1;
        goto cleanup;
    }

    JSONState json_state = {0};
    int num_transports = json_validate(&json_state, json);
    if (num_transports < 0)
    {
        ret = 1;
        goto cleanup;
    }

    Transport *transports = malloc(num_transports*sizeof(Transport));
    if (!transports)
    {
        fprintf(stderr, "Failed to allocate transports, somehow\n");
        ret = 1;
        goto cleanup;
    }

    if (json_init(&json_state, json, transports) < 0)
    {
        ret = 1;
        goto cleanup_transports;
    }

    Transport **id_map = alloca(num_transports*sizeof(Transport*));
    for (int i = 0; i < num_transports; i++)
    {
        Transport *t = &transports[i];
        id_map[t->common.id] = t;
    }

    TransportSelect *selects = alloca(num_transports);
    for (int i = 0; i < num_transports; i++)
    {
        selects[i].t = &transports[i];
        selects[i].bitmask = 0;
    }

    while (1)
    {
        if (transport_select(selects, num_transports, 1) > 0)
        {
            for (int i = 0; i < num_transports; i++)
            {
                TransportSelect *s = &selects[i];
                if (s->bitmask & SELECT_READ)
                {
                    printf("Got data from %s\n", s->t->options.name);
                    int read_bytes = transport_read(s->t, s->t->common.rxbuf, s->t->options.rxbuf_size);
                }
            }
        }
    }

cleanup_transports:
    free(transports);
cleanup:
    free(buffer);
    return ret;
}
