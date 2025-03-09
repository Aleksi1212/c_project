#ifndef DISKSTORE_H
#define DISKSTORE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque DiskStore structure */
typedef struct DiskStore DiskStore;

/*
 * Creates a new DiskStore object for the given database file.
 * If db_file is NULL, "ruby_kv.db" is used.
 * Returns a pointer to the DiskStore object, or NULL on error.
 */
DiskStore *disk_store_new(const char *db_file);

/*
 * Wipes (clears) the database.
 * Returns "OK" on success or an error string.
 */
const char *disk_store_wipe(DiskStore *ds);

/*
 * Returns a comma-separated string of keys in the store.
 * The returned string must be freed by the caller.
 * On error, returns an error string.
 */
char *disk_store_keys(DiskStore *ds);

/*
 * Retrieves the value for the given key.
 * Returns the value (allocated string) or an empty string if not found.
 * The returned string must be freed by the caller.
 */
char *disk_store_get(DiskStore *ds, const char *key);

/*
 * Puts the key-value pair into the store.
 * Returns "OK" on success or an error string.
 */
const char *disk_store_put(DiskStore *ds, const char *key, const char *value);

/*
 * Deletes the given key from the store.
 * Returns "OK" on success, "NOT FOUND" if the key does not exist, or an error string.
 */
const char *disk_store_delete(DiskStore *ds, const char *key);

/*
 * Frees the DiskStore object and closes any associated files.
 */
void disk_store_free(DiskStore *ds);

#ifdef __cplusplus
}
#endif

#endif
