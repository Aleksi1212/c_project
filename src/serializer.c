#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/serializer.h"
#include "../include/constants.h"

/* --- CRC32 Implementation --- */
static uint32_t crc32_table[256];
static int crc32_table_computed = 0;

static void make_crc32_table(void) {
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t c = i;
        for (size_t j = 0; j < 8; j++) {
            if (c & 1)
                c = 0xEDB88320UL ^ (c >> 1);
            else
                c = c >> 1;
        }
        crc32_table[i] = c;
    }
    crc32_table_computed = 1;
}

uint32_t crc32_calc(const unsigned char *data, size_t length) {
    if (!crc32_table_computed) {
        make_crc32_table();
    }
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < length; i++) {
        crc = crc32_table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
}

int crc32_valid(uint32_t digest, const unsigned char *data, size_t length) {
    uint32_t computed = crc32_calc(data, length);
    return (computed == digest);
}

/* --- Helper functions for packing/unpacking strings --- */
char *pack_str(const char *attribute, size_t *out_length) {
    if (!attribute) {
        *out_length = 0;
        return NULL;
    }
    *out_length = strlen(attribute);
    char *dup = (char *)malloc(*out_length);
    if (dup) {
        memcpy(dup, attribute, *out_length);
    }
    return dup;
}

char *unpack_str(const char *attribute, size_t length) {
    char *str = (char *)malloc(length + 1);
    if (str) {
        memcpy(str, attribute, length);
        str[length] = '\0';
    }
    return str;
}

/* --- Header Serialization --- */
int serialize_header(uint32_t epoch, uint32_t keysz, uint16_t key_type, uint16_t value_type, uint32_t valuesz, unsigned char header[HEADER_SIZE]) {
    /* Write epoch (4 bytes little-endian) */
    header[0] = epoch & 0xFF;
    header[1] = (epoch >> 8) & 0xFF;
    header[2] = (epoch >> 16) & 0xFF;
    header[3] = (epoch >> 24) & 0xFF;
    /* Write keysz (4 bytes) */
    header[4] = keysz & 0xFF;
    header[5] = (keysz >> 8) & 0xFF;
    header[6] = (keysz >> 16) & 0xFF;
    header[7] = (keysz >> 24) & 0xFF;
    /* Write valuesz (4 bytes) */
    header[8]  = valuesz & 0xFF;
    header[9]  = (valuesz >> 8) & 0xFF;
    header[10] = (valuesz >> 16) & 0xFF;
    header[11] = (valuesz >> 24) & 0xFF;
    /* Write key_type (2 bytes) */
    header[12] = key_type & 0xFF;
    header[13] = (key_type >> 8) & 0xFF;
    /* Write value_type (2 bytes) */
    header[14] = value_type & 0xFF;
    header[15] = (value_type >> 8) & 0xFF;
    return 0;
}

int deserialize_header(const unsigned char header_data[HEADER_SIZE], uint32_t *epoch, uint32_t *keysz, uint32_t *valuesz, uint16_t *key_type, uint16_t *value_type) {
    if (!header_data) return -1;
    *epoch = header_data[0] | (header_data[1] << 8) | (header_data[2] << 16) | (header_data[3] << 24);
    *keysz = header_data[4] | (header_data[5] << 8) | (header_data[6] << 16) | (header_data[7] << 24);
    *valuesz = header_data[8]  | (header_data[9] << 8) | (header_data[10] << 16) | (header_data[11] << 24);
    *key_type = header_data[12] | (header_data[13] << 8);
    *value_type = header_data[14] | (header_data[15] << 8);
    return 0;
}

/* --- Serialization of the full data record --- */
int serialize(uint32_t epoch, const char *key, const char *value, uint32_t *log_size, unsigned char **out_data, size_t *out_data_size) {
    if (!key || !value || !out_data || !out_data_size || !log_size) return -1;

    /* Determine types. We only support strings for both key and value. */
    uint16_t key_type = DATA_TYPE_String;
    uint16_t value_type = DATA_TYPE_String;

    /* Pack the key and value (duplicate them as raw bytes) */
    size_t key_len = 0, value_len = 0;
    char *key_bytes = pack_str(key, &key_len);
    char *value_bytes = pack_str(value, &value_len);

    /* Serialize the header */
    unsigned char header[HEADER_SIZE];
    serialize_header(epoch, (uint32_t)key_len, key_type, value_type, (uint32_t)value_len, header);

    /* Prepare the data block: header and data bytes will be concatenated */
    size_t data_bytes_len = key_len + value_len;
    size_t total_len = CRC32_HEADER_OFFSET + data_bytes_len;

    unsigned char *buffer = (unsigned char *)malloc(total_len);
    if (!buffer) {
        free(key_bytes);
        free(value_bytes);
        return -1;
    }

    /* Compute positions:
       - First 4 bytes: CRC32 digest placeholder
       - Next 16 bytes: header
       - Then: key_bytes followed by value_bytes
    */
    memcpy(buffer + CRC32_SIZE, header, HEADER_SIZE);
    memcpy(buffer + CRC32_HEADER_OFFSET, key_bytes, key_len);
    memcpy(buffer + CRC32_HEADER_OFFSET + key_len, value_bytes, value_len);

    /* Compute the CRC32 of header+data */
    uint32_t crc = crc32_calc(buffer + CRC32_SIZE, HEADER_SIZE + data_bytes_len);
    /* Write the CRC into first 4 bytes in little-endian */
    buffer[0] = crc & 0xFF;
    buffer[1] = (crc >> 8) & 0xFF;
    buffer[2] = (crc >> 16) & 0xFF;
    buffer[3] = (crc >> 24) & 0xFF;

    *log_size = (uint32_t)total_len;
    *out_data = buffer;
    *out_data_size = total_len;

    free(key_bytes);
    free(value_bytes);
    return 0;
}

/* --- Deserialization of the full data record --- */
int deserialize(const unsigned char *data, size_t data_size, uint32_t *epoch, char **key, char **value) {
    if (!data || data_size < CRC32_HEADER_OFFSET || !epoch || !key || !value) return -1;

    /* Read stored CRC from first 4 bytes (little-endian) */
    uint32_t stored_crc = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
    size_t remainder_len = data_size - CRC32_SIZE;
    /* Validate the CRC for the remainder of the data */
    if (!crc32_valid(stored_crc, data + CRC32_SIZE, remainder_len)) {
        /* If invalid, return 0, empty key and value */
        *epoch = 0;
        *key = strdup("");
        *value = strdup("");
        return -1;
    }
    /* Deserialize header */
    unsigned char header_data[HEADER_SIZE];
    memcpy(header_data, data + CRC32_SIZE, HEADER_SIZE);
    uint32_t keysz = 0, valuesz = 0;
    uint16_t key_type = 0, value_type = 0;
    deserialize_header(header_data, epoch, &keysz, &valuesz, &key_type, &value_type);

    /* Read key and value bytes */
    if (data_size < CRC32_HEADER_OFFSET + keysz + valuesz) {
        *epoch = 0;
        *key = strdup("");
        *value = strdup("");
        return -1;
    }
    const unsigned char *key_bytes = data + CRC32_HEADER_OFFSET;
    const unsigned char *value_bytes = data + CRC32_HEADER_OFFSET + keysz;

    *key = unpack_str((const char *)key_bytes, keysz);
    *value = unpack_str((const char *)value_bytes, valuesz);
    return 0;
}