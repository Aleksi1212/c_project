#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>

#include "../include/constants.h"
#include "../include/structs.h"
#include "../include/serializer.h"
#include "../include/crc32.h"
// #include "../include/utils.h"

// Creates key structure
static KeyStruct key_struct_create(size_t write_pos, size_t log_size, const char *key)
{
    KeyStruct ks;
    ks.write_pos = write_pos;
    ks.log_size = log_size;
    ks.key = key;
    return ks;
}

// Increment diskstore write position
static void DiskStore_incr_write_pos(DiskStore *store, size_t pos)
{
    store->write_pos += pos;
}

// Writes data to the file and flushes it
static void DiskStore_persist(DiskStore *store, const uint8_t *data, size_t len)
{
    if (fwrite(data, 1, len, store->db_fh) != len) {
        fprintf(stderr, "ERROR: write failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    fflush(store->db_fh);
}

// Append or update key in key_dir
static void DiskStore_update_key_dir(DiskStore *store, const KeyStruct ks)
{
    KeyDirNode *node = store->key_dir;
    printf("node : %p\n", (void*)node);
    while (node) {
        // printf("test1234\n");
        // printf("%s\n", );
        // printf("%s\n", ks.key);

        if (strcmp(node->ks.key, ks.key) == 0) {
            // Update existing
            // printf("testboii\n");
            node->ks.write_pos = ks.write_pos;
            node->ks.log_size = ks.log_size;
            // printf("this\n");
            return;
        }
        // printf("this111\n");
        node = node->next;
    }
    // Append new node
    KeyDirNode *new_node = malloc(sizeof(KeyDirNode));
    if (!new_node) {
        exit(EXIT_FAILURE);
    }
    // printf("")
    new_node->ks.write_pos = ks.write_pos;
    new_node->ks.log_size = ks.log_size;
    new_node->ks.key = ks.key;
    new_node->next = store->key_dir;

    // printf("%s\n", ks.key);
    printf("%lu\n", ks.log_size);
    // printf("new_node address: %p\n", (void*)new_node);
    printf("new_node: %p\n", (void*)new_node);
    printf("store: %p\n", (void*)store);
    printf("store->key_dir (before): %p\n", (void*)store->key_dir);

    store->key_dir = new_node;
    printf("store->key_dir (after): %p\n", (void*)store->key_dir->ks.log_size);
    printf("data: %lu\n", store->key_dir->ks.log_size);


}

// Free key_dir
static void free_key_dir(KeyDirNode *node)
{
    while (node) {
        KeyDirNode *next = node->next;
        free(node->ks.key);
        free(node);
        node = next;
    }
}


// Reads the db file and populates the key_dir
static void DiskStore_init_key_dir(DiskStore *store, uint32_t *crc32_table)
{
    // Ensure file position starts at beginning
    fseek(store->db_fh, 0, SEEK_SET);
    while (1) {
        uint8_t crc_and_header[CRC32_HEADER_OFFSET];
        size_t n = fread(crc_and_header, 1, CRC32_HEADER_OFFSET, store->db_fh);
        if (n != CRC32_HEADER_OFFSET) {
            break; // End of file so we break
        }

        // Extract header bytes
        uint8_t header_bytes[HEADER_SIZE];
        memcpy(header_bytes, crc_and_header + CRC32_SIZE, HEADER_SIZE);
        uint32_t epoch, keysz, valuesz;
        DataType key_type, value_type;
        deserialize_header(header_bytes, &epoch, &keysz, &valuesz, &key_type, &value_type); // Deserialize header bytes

        // Read key_bytes
        uint8_t *key_bytes = malloc(keysz);
        if (!key_bytes) exit(EXIT_FAILURE);
        if (fread(key_bytes, 1, keysz, store->db_fh) != keysz) {
            free(key_bytes);
            break;
        }
        
        // Read value_bytes
        uint8_t *value_bytes = malloc(valuesz);
        if (!value_bytes) {
            exit(EXIT_FAILURE);
        }
        if (fread(value_bytes, 1, valuesz, store->db_fh) != valuesz) {
            free(key_bytes);
            free(value_bytes);
            break;
        }
        
        // Unpack key and value
        char *unpacked_key = unpack_value(key_bytes, keysz, key_type);
        // printf("%s\n", unpacked_key);
        // value is unpacked but not stored beyond for re-insertion
        char *unpacked_value = unpack_value(value_bytes, valuesz, value_type);
        // printf("%s\n", unpack_value);

        // Get the stored CRC from crc_and_header 
        uint32_t file_crc = deserialize_crc32(crc_and_header);
        // Recompute crc for header_bytes + key_bytes + value_bytes
        size_t crc_data_len = HEADER_SIZE + keysz + valuesz;
        uint8_t *crc_data = malloc(crc_data_len);
        if (!crc_data) {
            exit(EXIT_FAILURE);
        }

        memcpy(crc_data, header_bytes, HEADER_SIZE);
        memcpy(crc_data + HEADER_SIZE, key_bytes, keysz);
        memcpy(crc_data + HEADER_SIZE + keysz, value_bytes, valuesz);
        if (!crc32_valid(file_crc, crc_data, crc_data_len, crc32_table)) {
            free(key_bytes);
            free(value_bytes);
            free(crc_data);
            free(unpacked_key);
            free(unpacked_value);
            fprintf(stderr, "file corrupted\n");
            exit(EXIT_FAILURE);
        }
        free(crc_data);
        free(key_bytes);
        free(value_bytes);

        // Calculate log size for the record
        size_t log_size = CRC32_HEADER_OFFSET + keysz + valuesz;
        KeyStruct ks = key_struct_create(store->write_pos, log_size, unpacked_key);
        // printf("%lu\n", log_size);
        DiskStore_update_key_dir(store, ks);
        // printf("%s\n", store->key_dir->ks.key);
        DiskStore_incr_write_pos(store, log_size);

        free(unpacked_value);
        free(unpacked_key);
        // printf("%s\n", store->key_dir->ks.key);

        // printf("success\n");
    }
}

/*
    Initializes new diskstore. Opens db_file in a+b (append+binary) mode.
    Sets write_pos = 0, and initializes the key_dir
*/
DiskStore *DiskStore_new(const char *db_file, uint32_t *crc32_table)
{
    generate_crc32_table(crc32_table);
    // for (int i = 0; i < 256; i++)
    // {
    //     printf("%u\n", crc32_table[256]);
    // }

    DiskStore *store = malloc(sizeof(DiskStore));
    if (!store) {
        exit(EXIT_FAILURE);
    }

    store->db_file = strdup(db_file);
    store->db_fh = fopen(db_file, "a+b");
    if (!store->db_fh) {
        fprintf(stderr, "Error opening file: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    store->write_pos = 0;
    store->key_dir = NULL;
    DiskStore_init_key_dir(store, crc32_table);
    // printf("%d\n", store->key_dir->ks.write_pos);
    return store;
}

// Wipes everything
const char *DiskStore_wipe(DiskStore *store)
{
    // Free current key_dir
    free_key_dir(store->key_dir);
    store->key_dir = NULL;
    store->write_pos = 0;
    // Truncate the file by reopening in write mode
    FILE *fh = fopen(store->db_file, "w");
    if (!fh) {
        static char err_msg[256];
        snprintf(err_msg, sizeof(err_msg), "ERROR: %s", strerror(errno));
        return err_msg;
    }
    fclose(fh);

    // Reopen the file in a+b mode
    fclose(store->db_fh);
    store->db_fh = fopen(store->db_file, "a+b");
    if (!store->db_fh) {
        static char err_msg2[256];
        snprintf(err_msg2, sizeof(err_msg2), "ERROR: %s", strerror(errno));
        return err_msg2;
    }
    return "OK";
}

// Gets all keys in diskstore
char *DiskStore_keys(DiskStore *store)
{
    size_t total_len = 0;
    KeyDirNode *node = store->key_dir;
    // First, calculate needed length
    while (node) {
        total_len += strlen(node->ks.key) + 1; // for comma or null terminator
        node = node->next;
    }
    printf("%d\n", total_len);
    if (total_len == 0) {
        return strdup("");
    }
    char *result = malloc(total_len);
    if (!result) exit(EXIT_FAILURE);
    result[0] = '\0';
    node = store->key_dir;
    int first = 1;
    while (node) {
        if (!first) {
            strcat(result, ",");
        }
        strcat(result, node->ks.key);
        first = 0;
        node = node->next;
    }
    return result;
}

// Finds key in diskstore returns a pointer to KeyStruct, NULL if not found
static KeyStruct *DiskStore_find_key(DiskStore *store, const char *key, uint32_t *crc32_table)
{
    KeyDirNode *node = store->key_dir;
    printf("node : %p\n", (void*)node);
    while (node) {
        printf("%lu\n", node->ks.log_size);

        // size_t test = 36;
        uint8_t *log_data = malloc(node->ks.log_size);
        // printf("test\n");

        uint32_t epoch;
        char *unpacked_key;
        char *value = NULL;
        int test2 = deserialize(log_data, node->ks.log_size, &epoch, &unpacked_key, &value, crc32_table);
        printf("%d\n", test2);

        free(log_data);

        // printf("%s\n", );
        // if (strcmp(node->ks.key, key) == 0)
        //     return &(node->ks);
        node = node->next;
    }
    return NULL;
}

/* 
    Retrieves the value corresponding to the key.
    Returns a newly allocated string containing the value.
    On error, returns an error message string.
*/
char *DiskStore_get(DiskStore *store, const char *key, uint32_t *crc32_table)
{
    printf("%ld\n", store->key_dir->ks.log_size);
    KeyStruct *ks = DiskStore_find_key(store, key, crc32_table);
    if (ks == NULL) {
        return "Not found";
    }

    if (fseek(store->db_fh, ks->write_pos, SEEK_SET) != 0) {
        printf("boi2\n");

        char err_msg[256];
        snprintf(err_msg, sizeof(err_msg), "ERROR: %s", strerror(errno));
        return strdup(err_msg);
    }
    printf("boi3\n");

    uint8_t *log_data = malloc(ks->log_size);
    if (!log_data) exit(EXIT_FAILURE);
    if (fread(log_data, 1, ks->log_size, store->db_fh) != ks->log_size) {
        free(log_data);
        return strdup("ERROR: unable to read log data");
    }

    uint32_t epoch;
    char *unpacked_key;
    char *value = NULL;
    if (!deserialize(log_data, ks->log_size, &epoch, &unpacked_key, &value, crc32_table)) {
        free(log_data);
        free(unpacked_key);
        return strdup("ERROR: CRC validation failed");
    }
    free(unpacked_key);
    free(log_data);
    return value;
}

/* 
   Serializes the key and value and writes the resulting log to disk.
   Also updates the key directory.
   Returns "OK" on success or an error message.
*/
const char *DiskStore_put(DiskStore *store, const char *key, const char *value, uint32_t *crc32_table)
{
    // for (int i = 0; i < 256; i++)
    // {
    //     printf("%u\n", crc32_table[256]);
    // }

    uint32_t current_time = (uint32_t) time(NULL); // Current time as epoch
    uint8_t *data = NULL;
    size_t log_size = serialize(current_time, key, value, &data, crc32_table);
    printf("%lu\n", log_size);
    if (!data) {
        return "ERROR: serialization failed";
    }
    KeyStruct ks = key_struct_create(store->write_pos, log_size, key);
    // printf("%s\n", ks.log_size);
    printf("test\n");
    DiskStore_update_key_dir(store, ks);
    printf("test1\n");
    DiskStore_persist(store, data, log_size);
    printf("test2\n");
    DiskStore_incr_write_pos(store, log_size);
    printf("test3\n");
    free(data);
    printf("test4\n");

    return "OK";
}

/* 
   Deletes the given key. It rebuilds the DB file excluding the deleted key.
   Returns "OK" on success, "NOT FOUND" if the key does not exist, or an error message.
*/
// const char *DiskStore_delete(DiskStore *store, const char *key, uint32_t *crc32_table)
// {
//     KeyStruct *ks = DiskStore_find_key(store, key);
//     if (!ks) {
//         return "NOT FOUND";
//     }
//     // Create an updated key directory by reading all current keys (except the one to delete)
//     KeyDirNode *node = store->key_dir;
//     typedef struct {
//         char *key;
//         char *value;
//     } KVPair;
//     KVPair *pairs = NULL;
//     size_t count = 0;
//     while (node) {
//         if (strcmp(node->ks.key, key) != 0) {
//             pairs = realloc(pairs, sizeof(KVPair) * (count + 1));
//             if (!pairs) {
//                 exit(EXIT_FAILURE);
//             }
//             char *cur_value = DiskStore_get(store, node->ks.key, crc32_table);
//             pairs[count].key = strdup(node->ks.key);
//             pairs[count].value = cur_value;
//             count++;
//         }
//         node = node->next;
//     }
//     const char *wipe_result = DiskStore_wipe(store);
//     if (strcmp(wipe_result, "OK") != 0) {
//         free(pairs);
//         return wipe_result;
//     }
//     // Reinsert the keys
//     for (size_t i = 0; i < count; i++) {
//         DiskStore_put(store, pairs[i].key, pairs[i].value, crc32_table);
//         free(pairs[i].key);
//         free(pairs[i].value);
//     }
//     free(pairs);
//     return "OK";
// }