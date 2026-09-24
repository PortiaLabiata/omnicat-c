#include <stdio.h>
#include <string.h>
#include "json.h"
#include "cJSON/cJSON.h"
#include "options.h"
#include "transport.h"

static int json_validate_item(cJSON *j)
{
    cJSON *name = cJSON_GetObjectItemCaseSensitive(j, "name");
    if (!name || !cJSON_IsString(name))
    {
        fprintf(stderr, "Invalid config file: \"name\" is not found or of invalid type\n");
        return -1;
    }
    return 0;
}

int json_validate(cJSON *j)
{
    const cJSON *configs = cJSON_GetObjectItemCaseSensitive(j, "configs");
    if (!configs || !cJSON_IsArray(configs))
    {
        fprintf(stderr, "Invalid config file: \"configs\" not found or invalid type\n");
        return -1;
    }

    cJSON *config = NULL;
    int num_configs = 0;
    cJSON_ArrayForEach(config, configs)
    {
        if (json_validate_item(config) < 0)
        {
            fprintf(stderr, "Invalid config\n");
            return -1;
        }
        num_configs++;
    }
    return num_configs;
}

#define streq(a, b) (strcmp(a, b) == 0)
static TransportKind str2kind(const char *s)
{
    if (streq(s, "stdio"))
    {
        return TRANSPORT_KIND_STDIO;
    }
    else 
    {
        return TRANSPORT_KIND_SIZE;
    }
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
    if (strlen(name_value) > NAME_SIZE_MAX)
    {
        fprintf(stderr, "Failed to create transport %s: name too long\n",
                name_value);
        return -1;
    }
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
    return 0;
}

int json_init(cJSON *j, Transport *ts)
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
        i++;
    }
    return 0;
}
