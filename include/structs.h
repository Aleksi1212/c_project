#include <stdint.h>

#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct {
    char *name;
    char *info;
    int indent;
} Command_list;

typedef struct {
    char alias[256];
    char link[1024];
} Repo;

#endif