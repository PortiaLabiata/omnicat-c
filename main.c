#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <malloc.h>
#include <alloca.h>
#include <poll.h>

#include "cJSON/cJSON.h"
#include "options.h"
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
        ret = 1;
        goto cleanup;
    }

    cJSON *json = cJSON_Parse(buffer);
    if (!json)
    {
        fprintf(stderr, "Failed to parse config file at %s\n",
                cJSON_GetErrorPtr());
        ret = 1;
        goto cleanup_json;
    }

    JSONState json_state = {0};
    int num_transports = json_validate(&json_state, json);
    if (num_transports < 0)
    {
        fprintf(stderr, "Invalid config\n");
        ret = 1;
        goto cleanup_json;
    }

    Transport *transports = malloc(num_transports*sizeof(Transport));
    if (!transports)
    {
        fprintf(stderr, "Failed to allocate transports, somehow\n");
        ret = 1;
        goto cleanup_json;
    }

    for (int i = 0; i < num_transports; i++)
    {
        options_set_defaults(&transports[i].options);
    }

    if (json_init(&json_state, json, transports) < 0)
    {
        fprintf(stderr, "Failed to create transports\n");
        ret = 1;
        goto cleanup_transports;
    }

    for (int i = 0; i < num_transports; i++)
    {
        if (transport_init(&transports[i]) < 0)
        {
            fprintf(stderr, "Failed to init transports\n");
            return 1;
        }
    }

    /*
    for (int i = 0; i < num_transports; i++)
    {
        Transport *t = &transports[i];
        printf("%s=%d: ", t->options.name,
               t->common.id);
        for (unsigned int j = 0; j < t->common.to_size; j++)
        {
            printf("%d ", t->common.to[j]);
        }
        puts("");
    }
    */

    Transport **id_map = alloca(num_transports*sizeof(Transport*));
    for (int i = 0; i < num_transports; i++)
    {
        Transport *t = &transports[i];
        id_map[t->common.id] = t;
    }

    struct pollfd *fds = alloca(sizeof(struct pollfd)*num_transports);
    for (int i = 0; i < num_transports; i++)
    {
        fds[i].fd = transports[i].common.fdin;
        fds[i].events = POLLIN;
        fds[i].revents = 0;
    }

    while (1)
    {
        int ret = poll(fds, num_transports, 1);
        if (ret < 0)
        {
            fprintf(stderr, "Select error: %s\n", strerror(errno));
            continue;
        }

        if (ret == 0)
        {
            continue;
        }

        for (int i = 0; i < num_transports; i++)
        {
            if (fds[i].revents & POLLIN)
            {
                fds[i].revents = 0;
                Transport *t = &transports[i];

                int read_bytes = transport_read(t, t->common.rxbuf, t->options.rxbuf_size);
                if (read_bytes == 0)
                {
                    continue;
                }
                else if (read_bytes < 0)
                {
                    printf("Error reading transport \"%s\": %s\n",
                            t->options.name, strerror(errno));
                }

                for (unsigned int j = 0; j < t->common.to_size; j++)
                {
                    transport_write(id_map[t->common.to[j]], t->common.rxbuf, read_bytes);
                }
            }
        }
    }

cleanup_transports:
    free(transports);
cleanup_json:
    cJSON_Delete(json);
cleanup:
    free(buffer);
    return ret;
}
