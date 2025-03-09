#include <stdint.h>
#include "structs.h"

#ifndef DISK_STORE_H
#define DISK_STORE_H

DiskStore *DiskStore_new(const char *db_file, uint32_t *crc32_table);
const char *DiskStore_wipe(DiskStore *store);
char *DiskStore_keys(DiskStore *store);
char *DiskStore_get(DiskStore *store, const char *key, uint32_t *crc32_table);
const char *DiskStore_put(DiskStore *store, const char *key, const char *value, uint32_t *crc32_table);
// const char *DiskStore_delete(DiskStore *store, const char *key, uint32_t *crc32_table);



#endif