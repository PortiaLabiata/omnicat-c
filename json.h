#ifndef JSON_H
#define JSON_H

#include "cJSON/cJSON.h"
#include "transport.h"

int json_validate(cJSON *j);
int json_init(cJSON *j, Transport *ts);

#endif
