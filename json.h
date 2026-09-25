#ifndef JSON_H
#define JSON_H

#include "cJSON/cJSON.h"
#include "transport.h"

typedef struct {
    unsigned int i;
    char *names;
} JSONState;

int json_validate(JSONState *s, cJSON *j);
int json_init(JSONState *s, cJSON *j, Transport *ts);

#endif
