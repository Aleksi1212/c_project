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

typedef struct KeyDirEntry {
    char *key;
    size_t write_pos;
    size_t log_size;
    struct KeyDirEntry *next;
} KeyDirEntry;

/* DiskStore structure definition */
struct DiskStore {
    char *db_file;
    FILE *db_fh;
    size_t write_pos;
    KeyDirEntry *key_dir;
};

#endif