#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>

#include "json.h"
#include "cJSON/cJSON.h"
#include "options.h"
#include "transport.h"

static int json_validate_item(cJSON *j, JSONState *s)
{
    cJSON *name = cJSON_GetObjectItemCaseSensitive(j, "name");
    if (!name || !cJSON_IsString(name))
    {
        fprintf(stderr, "Invalid config file: \"name\" is not found or of invalid type\n");
        return -1;
    }
    
    const char *name_value = cJSON_GetStringValue(name);
    if (strlen(name_value) > NAME_SIZE_MAX)
    {
        fprintf(stderr, "Failed to create transport %s: name too long\n",
                name_value);
        return -1;
    }

    for (unsigned int i = 0; i < s->i; i++)
    {
        if (strcmp(s->names + i*NAME_SIZE_MAX, name_value) == 0)
        {
            fprintf(stderr, "Failed to create transport %s: repeating name\n",
                    name_value);
            return -1;
        }
    }

    strncpy(s->names + s->i++ * NAME_SIZE_MAX, name_value, NAME_SIZE_MAX);

    return 0;
}

int json_validate(JSONState *s, cJSON *j)
{
    const cJSON *configs = cJSON_GetObjectItemCaseSensitive(j, "configs");
    if (!configs || !cJSON_IsArray(configs))
    {
        fprintf(stderr, "Invalid config file: \"configs\" not found or invalid type\n");
        return -1;
    }

    cJSON *config = NULL;
    int num_transports = 0;
    cJSON_ArrayForEach(config, configs)
    {
        num_transports++;
    }

    s->names = malloc((NAME_SIZE_MAX+1)*num_transports); 

    cJSON_ArrayForEach(config, configs)
    {
        if (json_validate_item(config, s) < 0)
        {
            fprintf(stderr, "Invalid config\n");
            return -1;
        }
    }
    return num_transports;
}

#define streq(a, b) (strcmp(a, b) == 0)
static TransportKind str2kind(const char *s)
{
    if (streq(s, "stdio"))
    {
        return TRANSPORT_KIND_STDIO;
    }
    else if (streq(s, "udp"))
    {
        return TRANSPORT_KIND_UDP;
    }
    else if (streq(s, "file"))
    {
        return TRANSPORT_KIND_FILE;
    }
    else 
    {
        return TRANSPORT_KIND_SIZE;
    }
}

#define GET_OPTION_STRING(j, t, name) \
    cJSON *name = cJSON_GetObjectItemCaseSensitive(j, #name);  \
    if (name)                                                   \
    {                                                           \
        if (!cJSON_IsString(name))                              \
        {                                                       \
            fprintf(stderr, "Failed to create transport %s: items have invalid type\n", \
                    name_value);                                \
            return -1;                                          \
        }                                                       \
        strcpy(t->options.name, cJSON_GetStringValue(name));    \
    }

#define GET_OPTION_TYPE(j, t, name, type) \
    cJSON *name = cJSON_GetObjectItemCaseSensitive(j, #name);  \
    if (name)                                                   \
    {                                                           \
        if (!cJSON_Is##type(name))                              \
        {                                                       \
            fprintf(stderr, "Failed to create transport %s: items have invalid type\n", \
                    name_value);                                \
            return -1;                                          \
        }                                                       \
        t->options.name = cJSON_Get##type##Value(name);         \
    }

#define GET_OPTION_BOOL(j, t, name) \
    cJSON *name = cJSON_GetObjectItemCaseSensitive(j, #name);  \
    if (name)                                                   \
    {                                                           \
        if (!cJSON_IsBool(name))                              \
        {                                                       \
            fprintf(stderr, "Failed to create transport %s: items have invalid type\n", \
                    name_value);                                \
            return -1;                                          \
        }                                                       \
        t->options.name = cJSON_IsTrue(name);                   \
    }

static int json_create_item(cJSON *j, Transport *t)
{
    cJSON *name = cJSON_GetObjectItemCaseSensitive(j, "name");
    if (!name || !cJSON_IsString(name))
    {
        fprintf(stderr, "Failed to create transport: \"name\" not found or of invalid type\n");
        return -1;
    }

    const char *name_value = cJSON_GetStringValue(name); 
    strncpy(t->options.name, name_value, NAME_SIZE_MAX);

    cJSON *kind = cJSON_GetObjectItemCaseSensitive(j, "kind");
    if (!kind || !cJSON_IsString(kind))
    {
        fprintf(stderr, "Failed to create transport %s: \"kind\" not found or of invalid type\n",
                name_value);
        return -1;
    }

    const char *kind_value = cJSON_GetStringValue(kind);
    TransportKind kind_converted = str2kind(kind_value);
    if (kind_converted == TRANSPORT_KIND_SIZE)
    {
        fprintf(stderr, "Failed to create transport %s: invalid kind %s\n",
                name_value, kind_value);
        return -1;
    }
    t->kind = kind_converted;

    cJSON *rxbuf_size = cJSON_GetObjectItemCaseSensitive(j, "rxbuf_size");
    if (rxbuf_size)
    {
        if (!cJSON_IsNumber(rxbuf_size))
        {
            fprintf(stderr, "Failed to create transport %s: \"rxbuf_size\" has invalid type\n",
                    name_value);
            return -1;
        }

        int rxbuf_size_value = cJSON_GetNumberValue(rxbuf_size);
        t->options.rxbuf_size = rxbuf_size_value;
    }

    GET_OPTION_STRING(j, t, addr);
    GET_OPTION_TYPE(j, t, port, Number);
    GET_OPTION_BOOL(j, t, server);
    GET_OPTION_BOOL(j, t, reuseaddr);
    GET_OPTION_TYPE(j, t, so_rcvbuf, Number);
    GET_OPTION_TYPE(j, t, so_sndbuf, Number);

    return 0;
}

static int json_init_id_item(JSONState *s, cJSON *j, Transport *t)
{
    cJSON *to = cJSON_GetObjectItemCaseSensitive(j, "to");
    if (!to || !cJSON_IsArray(to))
    {
        fprintf(stderr, "Failed to create transports: \"to\" not found or of invalid type\n");
        return -1;
    }

    int i = 0;
    cJSON *to_entry = NULL;
    cJSON_ArrayForEach(to_entry, to)
    {
        if (!cJSON_IsString(to_entry))
        {
            fprintf(stderr, "Failed to create transports: invalid type in destination list\n");
            return -1;
        }

        const char *to_entry_value = cJSON_GetStringValue(to_entry);

        bool valid_name = false;
        for (unsigned int j = 0; j < s->i; j++)
        {
            if (strncmp(to_entry_value, s->names + NAME_SIZE_MAX*j, NAME_SIZE_MAX) == 0)
            {
                valid_name = true;
                t->common.to[i] = j;
                break;
            }
        }

        if (!valid_name)
        {
            fprintf(stderr, "Failed to create transports: invalid destination \"%s\"\n",
                    to_entry_value);
            return -1;
        }
        i++;
    }
    t->common.to_size = i;
    return 0;
}

int json_init(JSONState *s, cJSON *j, Transport *ts)
{
    const cJSON *configs = cJSON_GetObjectItemCaseSensitive(j, "configs");

    cJSON *config = NULL;
    int i = 0;
    cJSON_ArrayForEach(config, configs)
    {
        if (json_create_item(config, &ts[i]) < 0)
        {
            fprintf(stderr, "Failed to create transports\n");
            return -1;
        }
        ts[i].common.id = i;
        i++;
    }

    i = 0;
    cJSON_ArrayForEach(config, configs)
    {
        if (json_init_id_item(s, config, &ts[i]) < 0)
        {
            fprintf(stderr, "Failed to create transports\n");
            return -1;
        }
        i++;
    }
    return 0;
}
