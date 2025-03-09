#include <stdint.h>

#ifndef CRC32_H
#define CRC32_H

void generate_crc32_table(uint32_t *crc32_table);
uint32_t compute_crc32(const uint8_t *buffer, size_t length, uint32_t *crc32_table);
uint8_t *crc32_pack(const uint8_t *data_bytes, size_t len, uint32_t *crc32_table);
uint32_t deserialize_crc32(const uint8_t *crc);
int crc32_valid(uint32_t digest, const uint8_t *data_bytes, size_t len, uint32_t *crc32_table);


#endif