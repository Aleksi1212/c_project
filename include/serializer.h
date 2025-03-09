/* File: serializer.h */
#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <stdint.h>
#include <stdlib.h>

/* Constants and Formats */
#define HEADER_SIZE 16
#define CRC32_SIZE 4
#define CRC32_HEADER_OFFSET (CRC32_SIZE + HEADER_SIZE)

/* Data type definitions */
enum DataType {
    DATA_TYPE_Integer = 1,
    DATA_TYPE_Float   = 2,
    DATA_TYPE_String  = 3
};

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * Serializes the given epoch, key and value into a data block.
 * It returns 0 on success.
 * On success, *log_size is set to the size of the complete log record,
 * *out_data points to a malloc()ed buffer of length *out_data_size.
 */
int serialize(uint32_t epoch, const char *key, const char *value, uint32_t *log_size, unsigned char **out_data, size_t *out_data_size);

/* 
 * Deserializes the given data block.
 * On success, it sets *epoch, *key and *value (which are malloc()ed strings) 
 * and returns 0. If CRC check fails or data is too short, it returns non‐zero 
 * and sets *epoch to 0 and *key and *value to empty strings.
 */
int deserialize(const unsigned char *data, size_t data_size, uint32_t *epoch, char **key, char **value);

/* 
 * Serializes the header into the 16-byte buffer.
 * The header consists of:
 *   - epoch (4 bytes, little-endian)
 *   - keysz (4 bytes, little-endian)
 *   - valuesz (4 bytes, little-endian)
 *   - key_type (2 bytes, little-endian)
 *   - value_type (2 bytes, little-endian)
 */
int serialize_header(uint32_t epoch, uint32_t keysz, uint16_t key_type, uint16_t value_type, uint32_t valuesz, unsigned char header[HEADER_SIZE]);

/*
 * Deserializes the 16-byte header.
 * On success, sets the provided pointers to the header values.
 */
int deserialize_header(const unsigned char header_data[HEADER_SIZE], uint32_t *epoch, uint32_t *keysz, uint32_t *valuesz, uint16_t *key_type, uint16_t *value_type);

/*
 * Computes the CRC32 checksum for the given data.
 */
uint32_t crc32_calc(const unsigned char *data, size_t length);

/*
 * Checks if the provided digest matches the computed CRC32 checksum of data.
 * Returns 1 if valid, 0 otherwise.
 */
int crc32_valid(uint32_t digest, const unsigned char *data, size_t length);

/*
 * "Packs" a string attribute. Since we support only strings,
 * this simply duplicates the string and returns its length.
 * The returned pointer must be freed by the caller.
 */
char *pack_str(const char *attribute, size_t *out_length);

/*
 * "Unpacks" a string attribute by duplicating it and ensuring 
 * it is null-terminated. The returned pointer must be freed by the caller.
 */
char *unpack_str(const char *attribute, size_t length);

#ifdef __cplusplus
}
#endif

#endif