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

typedef enum {
    TYPE_INTEGER = 1,
    TYPE_FLOAT = 2,
    TYPE_STRING = 3
} DataType;

typedef struct KeyStruct {
    size_t write_pos;
    size_t log_size;
    char *key;
} KeyStruct;

typedef struct KeyDirNode {
    KeyStruct ks;
    struct KeyDirNode *next;
} KeyDirNode;

typedef struct DiskStore {
    char *db_file;
    FILE *db_fh;
    size_t write_pos;
    KeyDirNode *key_dir;
} DiskStore;

#endif