#include <stdint.h>
#include "constants.h"

#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct {
    char *name;
    char *info;
    int indent;
} Command_list;

typedef struct {
    char alias[MAX_KEY_SIZE];
    char link[MAX_VALUE_SIZE];
} Repo;

#endif