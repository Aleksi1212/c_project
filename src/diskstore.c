
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/diskstore.h"
#include "../include/serializer.h"

#include "../include/constants.h"
#include "../include/structs.h"

/* Forward declarations for internal helper functions */
static KeyDirEntry *find_key_entry(DiskStore *ds, const char *key);
static void add_key_entry(DiskStore *ds, const char *key, size_t write_pos, size_t log_size);
static void free_key_dir(KeyDirEntry *head);
static int init_key_dir(DiskStore *ds);

/* Returns a newly allocated KeyDirEntry key struct */
static KeyDirEntry *create_key_entry(const char *key, size_t write_pos, size_t log_size) {
    KeyDirEntry *entry = (KeyDirEntry *)malloc(sizeof(KeyDirEntry));
    if (!entry) return NULL;
    entry->key = strdup(key);
    entry->write_pos = write_pos;
    entry->log_size = log_size;
    entry->next = NULL;
    return entry;
}

/* Finds a key entry in the key directory */
static KeyDirEntry *find_key_entry(DiskStore *ds, const char *key) {
    KeyDirEntry *current = ds->key_dir;
    while (current) {
        if (strcmp(current->key, key) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}

/* Adds or updates a key entry in the key directory */
static void add_key_entry(DiskStore *ds, const char *key, size_t write_pos, size_t log_size) {
    KeyDirEntry *existing = find_key_entry(ds, key);
    if (existing) {
        existing->write_pos = write_pos;
        existing->log_size = log_size;
    } else {
        KeyDirEntry *entry = create_key_entry(key, write_pos, log_size);
        if (!entry) return;
        entry->next = ds->key_dir;
        ds->key_dir = entry;
    }
}

/* Frees the entire key directory linked list */
static void free_key_dir(KeyDirEntry *head) {
    KeyDirEntry *current = head;
    while (current) {
        KeyDirEntry *temp = current;
        current = current->next;
        free(temp->key);
        free(temp);
    }
}

/* Increments the write position */
static void incr_write_pos(DiskStore *ds, size_t pos) {
    ds->write_pos += pos;
}

/* Persists data to disk (writes and flushes) */
static void persist(DiskStore *ds, const unsigned char *data, size_t data_size) {
    fwrite(data, 1, data_size, ds->db_fh);
    fflush(ds->db_fh);
}

/* Initializes the key directory by reading the database file */
static int init_key_dir(DiskStore *ds) {
    /* Seek to start of file */
    fseek(ds->db_fh, 0, SEEK_SET);
    ds->write_pos = 0;
    while (1) {
        unsigned char crc_and_header[CRC32_HEADER_OFFSET];
        size_t read_bytes = fread(crc_and_header, 1, CRC32_HEADER_OFFSET, ds->db_fh);
        if (read_bytes == 0) {
            break; /* End of file */
        }
        if (read_bytes < CRC32_HEADER_OFFSET) {
            /* Corrupted/incomplete record */
            return -1;
        }
        /* Extract header bytes (after CRC portion) */
        unsigned char header_bytes[HEADER_SIZE];
        memcpy(header_bytes, crc_and_header + CRC32_SIZE, HEADER_SIZE);
        uint32_t epoch = 0, keysz = 0, valuesz = 0;
        uint16_t key_type = 0, value_type = 0;
        deserialize_header(header_bytes, &epoch, &keysz, &valuesz, &key_type, &value_type);
        /* Read key_bytes and value_bytes */
        unsigned char *key_bytes = (unsigned char *)malloc(keysz);
        if (fread(key_bytes, 1, keysz, ds->db_fh) != keysz) {
            free(key_bytes);
            return -1;
        }
        unsigned char *value_bytes = (unsigned char *)malloc(valuesz);
        if (fread(value_bytes, 1, valuesz, ds->db_fh) != valuesz) {
            free(key_bytes);
            free(value_bytes);
            return -1;
        }
        /* Validate CRC */
        uint32_t stored_crc = crc_and_header[0] | (crc_and_header[1] << 8) | (crc_and_header[2] << 16) | (crc_and_header[3] << 24);
        size_t crc_calc_len = HEADER_SIZE + keysz + valuesz;
        unsigned char *crc_buf = (unsigned char *)malloc(crc_calc_len);
        memcpy(crc_buf, header_bytes, HEADER_SIZE);
        memcpy(crc_buf + HEADER_SIZE, key_bytes, keysz);
        memcpy(crc_buf + HEADER_SIZE + keysz, value_bytes, valuesz);
        if (!crc32_valid(stored_crc, crc_buf, crc_calc_len)) {
            free(crc_buf);
            free(key_bytes);
            free(value_bytes);
            return -1;
        }
        free(crc_buf);
        /* Unpack the key */
        char *key_str = unpack_str((const char *)key_bytes, keysz);
        size_t log_size = CRC32_HEADER_OFFSET + keysz + valuesz;
        add_key_entry(ds, key_str, ds->write_pos, log_size);
        free(key_str);
        free(key_bytes);
        free(value_bytes);
        incr_write_pos(ds, log_size);
    }
    return 0;
}

/* Creates a new DiskStore instance */
DiskStore *disk_store_new(const char *db_file) {
    DiskStore *ds = (DiskStore *)malloc(sizeof(DiskStore));
    if (!ds) return NULL;
    if (db_file)
        ds->db_file = strdup(db_file);
    else
        ds->db_file = strdup("ruby_kv.db");
    ds->write_pos = 0;
    ds->key_dir = NULL;
    ds->db_fh = fopen(ds->db_file, "a+b");
    if (!ds->db_fh) {
        free(ds->db_file);
        free(ds);
        return NULL;
    }
    if (init_key_dir(ds) != 0) {
        /* If init fails, we treat it as corruption */
        fclose(ds->db_fh);
        free(ds->db_file);
        free(ds);
        return NULL;
    }
    return ds;
}

/* Wipes the database file and resets the DiskStore */
const char *disk_store_wipe(DiskStore *ds) {
    if (!ds) return "ERROR: DiskStore is NULL";
    free_key_dir(ds->key_dir);
    ds->key_dir = NULL;
    ds->write_pos = 0;
    /* Truncate the database file */
    ds->db_fh = freopen(ds->db_file, "w+b", ds->db_fh);
    if (!ds->db_fh) return "ERROR: Unable to open file for wiping";
    return "OK";
}

/* Returns a comma-separated string of keys */
char *disk_store_keys(DiskStore *ds) {
    if (!ds) return strdup("ERROR: DiskStore is NULL");
    /* First calculate total length */
    size_t total_len = 0;
    KeyDirEntry *current = ds->key_dir;
    int count = 0;
    while (current) {
        total_len += strlen(current->key);
        count++;
        current = current->next;
    }
    if (count > 0) total_len += (count - 1); /* For commas */
    char *keys_str = (char *)malloc(total_len + 1);
    if (!keys_str) return strdup("ERROR: Memory allocation failed");
    keys_str[0] = '\0';
    current = ds->key_dir;
    while (current) {
        strcat(keys_str, current->key);
        if (current->next)
            strcat(keys_str, ",");
        current = current->next;
    }
    return keys_str;
}

/* Retrieves the value associated with the key */
char *disk_store_get(DiskStore *ds, const char *key) {
    if (!ds || !key) return strdup("");
    KeyDirEntry *entry = find_key_entry(ds, key);
    if (!entry)
        return strdup("");
    if (fseek(ds->db_fh, entry->write_pos, SEEK_SET) != 0)
        return strdup("ERROR: fseek failed");
    unsigned char *log_data = (unsigned char *)malloc(entry->log_size);
    if (!log_data) return strdup("ERROR: Memory allocation failed");
    if (fread(log_data, 1, entry->log_size, ds->db_fh) != entry->log_size) {
        free(log_data);
        return strdup("ERROR: Failed to read log data");
    }
    uint32_t epoch = 0;
    char *retrieved_key = NULL;
    char *retrieved_value = NULL;
    if (deserialize(log_data, entry->log_size, &epoch, &retrieved_key, &retrieved_value) != 0) {
        free(log_data);
        free(retrieved_key);
        free(retrieved_value);
        return strdup("ERROR: CRC validation failed");
    }
    free(retrieved_key);
    free(log_data);
    return retrieved_value;
}

/* Puts a key-value pair into the store */
const char *disk_store_put(DiskStore *ds, const char *key, const char *value) {
    if (!ds || !key || !value) return "ERROR: Invalid arguments";
    uint32_t log_size = 0;
    unsigned char *data = NULL;
    size_t data_size = 0;
    uint32_t epoch = (uint32_t)time(NULL);
    if (serialize(epoch, key, value, &log_size, &data, &data_size) != 0) {
        return "ERROR: Serialization failed";
    }
    add_key_entry(ds, key, ds->write_pos, log_size);
    persist(ds, data, data_size);
    incr_write_pos(ds, log_size);
    free(data);
    return "OK";
}

/* Deletes a key from the store */
const char *disk_store_delete(DiskStore *ds, const char *key) {
    if (!ds || !key) return "ERROR: Invalid arguments";
    KeyDirEntry *entry = find_key_entry(ds, key);
    if (!entry)
        return "NOT FOUND";
    /* Rebuild updated key directory excluding the key to delete */
    /* First, collect remaining keys and their values */
    KeyDirEntry *current = ds->key_dir;
    /* Temporary storage for keys and values */
    typedef struct TempKV {
        char *key;
        char *value;
    } TempKV;
    TempKV *tempArr = NULL;
    int count = 0;
    while (current) {
        if (strcmp(current->key, key) != 0) {
            tempArr = (TempKV *)realloc(tempArr, sizeof(TempKV) * (count + 1));
            if (!tempArr) return "ERROR: Memory allocation failed";
            char *val = disk_store_get(ds, current->key);
            tempArr[count].key = strdup(current->key);
            tempArr[count].value = val;
            count++;
        }
        current = current->next;
    }
    const char *wipe_err = disk_store_wipe(ds);
    if (strcmp(wipe_err, "OK") != 0) {
        /* Free temporary storage */
        for (int i = 0; i < count; i++) {
            free(tempArr[i].key);
            free(tempArr[i].value);
        }
        free(tempArr);
        return wipe_err;
    }
    /* Reinsert the remaining key-value pairs */
    for (int i = 0; i < count; i++) {
        disk_store_put(ds, tempArr[i].key, tempArr[i].value);
        free(tempArr[i].key);
        free(tempArr[i].value);
    }
    free(tempArr);
    return "OK";
}

/* Frees the DiskStore object */
void disk_store_free(DiskStore *ds) {
    if (!ds) return;
    if (ds->db_fh) fclose(ds->db_fh);
    if (ds->db_file) free(ds->db_file);
    free_key_dir(ds->key_dir);
    free(ds);
}