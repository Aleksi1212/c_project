#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../include/utils.h"
#include "../include/structs.h"
#include "../include/constants.h"
#include "../include/serializer.h"
#include "../include/crc32.h"

/*
    serialize_header: packs epoch, keysz, valuesz, key_type, value_type in little-endian order.
    Header format: [uint32_t epoch][uint32_t keysz][uint32_t valuesz][uint16_t key_type][uint16_t value_type]
    Total size: 16 bytes.
*/
void serialize_header(uint8_t *header, uint32_t epoch, uint32_t keysz, uint32_t valuesz, uint16_t key_type, uint16_t value_type)
{
    write_uint32_le(header, epoch);
    write_uint32_le(header + 4, keysz);
    write_uint32_le(header + 8, valuesz);
    write_uint16_le(header + 12, key_type);
    write_uint16_le(header + 14, value_type);
}

/*
    deserialize_header: unpacks the header_data (16 bytes) into the provided pointers.
    Also looks up the type using DATA_TYPE_LOOK_UP.
*/
void deserialize_header(const uint8_t *header_data, uint32_t *epoch, uint32_t *keysz, uint32_t *valuesz, DataType *key_type, DataType *value_type)
{
    *epoch = read_uint32_le(header_data);
    *keysz = read_uint32_le(header_data + 4);
    *valuesz = read_uint32_le(header_data + 8);

    /* The stored types are uint16_t; use them directly */
    uint16_t kt = read_uint16_le(header_data + 12);
    uint16_t vt = read_uint16_le(header_data + 14);

    /* Lookup: DATA_TYPE_LOOK_UP: 1 => TYPE_INTEGER, 2 => TYPE_FLOAT, 3 => TYPE_STRING */
    *key_type = (kt == TYPE_INTEGER ? TYPE_INTEGER : (kt == TYPE_FLOAT ? TYPE_FLOAT : TYPE_STRING));
    *value_type = (vt == TYPE_INTEGER ? TYPE_INTEGER : (vt == TYPE_FLOAT ? TYPE_FLOAT : TYPE_STRING));
}

/*
    pack_value: Converts attribute (given as a string for TYPE_STRING, or pointer to appropriate type for TYPE_INTEGER/TYPE_FLOAT) into a byte array.
    This function allocates memory.
    For TYPE_INTEGER: expects pointer to int64_t, packs 8 bytes little-endian.
    For TYPE_FLOAT: expects pointer to float, packs 4 bytes little-endian.
    For TYPE_STRING: expects a null terminated char* string.
*/
uint8_t *pack_value(const void *attribute, DataType attribute_type, size_t *out_len)
{
    uint8_t *result = NULL;

    if (attribute_type == TYPE_INTEGER)
    {
        *out_len = sizeof(int64_t);
        result = malloc(*out_len);
        if (!result) {
            exit(EXIT_FAILURE);
        }

        int64_t val = *(int64_t *)attribute;
        for (size_t i = 0; i < 8; i++) {
            result[i] = (uint8_t)((val >> (8 * i)) & 0xff);
        }
    }
    else if (attribute_type == TYPE_FLOAT)
    {
        *out_len = sizeof(float);
        result = malloc(*out_len);
        if (!result) {
            exit(EXIT_FAILURE);
        }
        float fval = *(float *)attribute;
        uint32_t temp;
        memcpy(&temp, &fval, sizeof(uint32_t));
        write_uint32_le(result, temp);
    }
    else if (attribute_type == TYPE_STRING)
    {
        const char *str = (const char *)attribute;
        *out_len = strlen(str);
        result = malloc(*out_len);
        if (!result) {
            exit(EXIT_FAILURE);
        }
        memcpy(result, str, *out_len);
    }
    else
    {
        fprintf(stderr, "Invalid attribute_type in pack_value\n");
        exit(EXIT_FAILURE);
    }

    return result;
}

/*
    unpack: Converts a byte array into the appropriate type.
    For TYPE_INTEGER: returns a newly allocated string with the integer value.
    For TYPE_FLOAT: returns a newly allocated string with the float value.
    For TYPE_STRING: returns a newly allocated null-terminated string.
*/
char *unpack_value(const uint8_t *attribute, size_t len, DataType attribute_type)
{
    char *result = NULL;
    if (attribute_type == TYPE_INTEGER)
    {
        if (len < sizeof(int64_t)) {
            fprintf(stderr, "Invalid length for integer unpack\n");
            exit(EXIT_FAILURE);
        }
        int64_t val = 0;
        for (size_t i = 0; i < 8; i++) {
            val |= ((int64_t)attribute[i]) << (8 * i);
        }
        result = malloc(32);
        if (!result) {
            exit(EXIT_FAILURE);
        }
        snprintf(result, 32, "%lld", (long long)val);
    }
    else if (attribute_type == TYPE_FLOAT)
    {
        if (len < sizeof(float)) {
            fprintf(stderr, "Invalid length for float unpack\n");
            exit(EXIT_FAILURE);
        }
        uint32_t temp = read_uint32_le(attribute);
        float fval;
        memcpy(&fval, &temp, sizeof(float));
        result = malloc(32);
        if (!result) {
            exit(EXIT_FAILURE);
        }
        snprintf(result, 32, "%f", fval);
    }
    else if (attribute_type == TYPE_STRING)
    {
        result = malloc(len + 1);
        if (!result) {
            exit(EXIT_FAILURE);
        }

        memcpy(result, attribute, len);
        result[len] = '\0';
    }
    else
    {
        fprintf(stderr, "Invalid attribute_type in unpack_value\n");
        exit(EXIT_FAILURE);
    }

    return result;
}

/*
    serialize: Given epoch, key, and value (each provided as pointers).
    Returns based on out_len the total size (crc32_header_offset + data length) and based on out_data the newly allocated byte array.
    The function returns the total length.
*/
size_t serialize(uint32_t epoch, const void *key, const void *value, uint8_t **out_data, uint32_t *crc32_table)
{
    // for (int i = 0; i < 256; i++)
    // {
    //     printf("%u\n", crc32_table[i]);
    // }

    DataType key_type = TYPE_STRING;
    DataType value_type = TYPE_STRING;

    size_t key_len, value_len;
    uint8_t *key_bytes = pack_value(key, key_type, &key_len);
    uint8_t *value_bytes = pack_value(value, value_type, &value_len);

    // Serialize header
    uint8_t header[HEADER_SIZE];
    serialize_header(header, epoch, (uint32_t)key_len, (uint32_t)value_len, (uint16_t)key_type, (uint16_t)value_type);

    // Prepare data: data = key_bytes + value_bytes
    size_t data_len = key_len + value_len;
    uint8_t *data = malloc(data_len);
    if (!data) {
        exit(EXIT_FAILURE);
    }
    memcpy(data, key_bytes, key_len);
    memcpy(data + key_len, value_bytes, value_len);

    // Prepare full record: crc32(header + data) + header + data
    size_t record_len = CRC32_HEADER_OFFSET + data_len;
    uint8_t *record = malloc(record_len);
    if (!record) {
        exit(EXIT_FAILURE);
    }

    // Header + data used for CRC computation
    uint8_t *header_and_data = malloc(HEADER_SIZE + data_len);
    if (!header_and_data) {
        exit(EXIT_FAILURE);
    }
    memcpy(header_and_data, header, HEADER_SIZE);
    memcpy(header_and_data + HEADER_SIZE, data, data_len);

    uint8_t *crc_bytes = crc32_pack(header_and_data, HEADER_SIZE + data_len, crc32_table);
    // printf("%u\n", crc_bytes);
    memcpy(record, crc_bytes, CRC32_SIZE);
    memcpy(record + CRC32_SIZE, header, HEADER_SIZE);
    memcpy(record + CRC32_HEADER_OFFSET, data, data_len);

    free(key_bytes);
    free(value_bytes);
    free(data);
    free(header_and_data);
    free(crc_bytes);

    *out_data = record;
    return record_len;
}

/*
    deserialize: Given the full record data and its length, validates the CRC and extracts epoch, key, and value.
    If CRC validation fails, epoch is returned as 0 and key and value as empty strings.
*/
int deserialize(const uint8_t *data, size_t len, uint32_t *epoch, char **key_out, char **value_out, uint32_t *crc32_table)
{
    
    // Check if enough data
    if (len < CRC32_HEADER_OFFSET) {
        *epoch = 0;
        *key_out = strdup("");
        *value_out = strdup("");
        return 0;
    }

    // Extract stored crc

    uint32_t stored_crc = deserialize_crc32(data);

    size_t header_and_data_len = len - CRC32_SIZE;
    const uint8_t *header_and_data = data + CRC32_SIZE;
    if (!crc32_valid(stored_crc, header_and_data, header_and_data_len, crc32_table)) {
        printf("not valid\n");
        *epoch = 0;
        *key_out = strdup("");
        *value_out = strdup("");
        return 0;
    }

    // Deserialize header
    uint8_t header[HEADER_SIZE];
    memcpy(header, data + CRC32_SIZE, HEADER_SIZE);
    uint32_t rec_epoch, keysz, valuesz;
    DataType key_type, value_type;
    deserialize_header(header, &rec_epoch, &keysz, &valuesz, &key_type, &value_type);


    // Get key_bytes and value_bytes
    printf("%lu\n", keysz);
    if (len < CRC32_HEADER_OFFSET + keysz) {
        *epoch = 0;
        *key_out = strdup("");
        *value_out = strdup("");
        return 0;
    }
    const uint8_t *key_bytes = data + CRC32_HEADER_OFFSET;
    const uint8_t *value_bytes = data + CRC32_HEADER_OFFSET + keysz;

    
    // Unpack key and value
    char *unpacked_key = unpack_value(key_bytes, keysz, key_type);
    char *unpacked_value = unpack_value(value_bytes, valuesz, value_type);

    *epoch = rec_epoch;
    *key_out = unpacked_key;
    *value_out = unpacked_value;

    return 1;
}