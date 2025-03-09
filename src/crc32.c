#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "../include/constants.h"
#include "../include/crc32.h"
#include "../include/utils.h"

void generate_crc32_table(uint32_t *crc32_table)
{
    uint32_t c;
    int n, k;
    for (n = 0; n < 256; n++) {
        c = (uint32_t) n;
        for (k = 0; k < 8; k++) {
            if (c & 1)
                c = 0xEDB88320UL ^ (c >> 1);
            else
                c = c >> 1;
        }
        crc32_table[n] = c;
    }
}

uint32_t compute_crc32(const uint8_t *buffer, size_t length, uint32_t *crc32_table)
{
    uint32_t c = 0xFFFFFFFFUL;
    size_t n;

    // for (int i = 0; i < 256; i++)
    // {
    //     printf("%u\n", crc32_table[i]);
    // }
    // printf("%s\n", buffer);

    if (crc32_table == NULL) {
        generate_crc32_table(crc32_table);
    }

    for (n = 0; n < length; n++) {
        // c = i
        c = crc32_table[(c ^ buffer[n]) & 0xFF] ^ (c >> 8);
    }
    return c ^ 0xFFFFFFFFUL;
}

uint8_t *crc32_pack(const uint8_t *data_bytes, size_t len, uint32_t *crc32_table)
{
    // for (int i = 0; i < 256; i++)
    // {
    //     printf("%u\n", crc32_table[i]);
    // }

    uint32_t crc = compute_crc32(data_bytes, len, crc32_table);
    uint8_t *buffer = malloc(CRC32_SIZE);
    if (!buffer) {
        exit(EXIT_FAILURE);
    }

    write_uint32_le(buffer, crc);
    return buffer;
}

// deserialize_crc32: Unpacks the 4-byte CRC value
uint32_t deserialize_crc32(const uint8_t *crc)
{
    return read_uint32_le(crc);
}

// crc32_valid: Compares the digest with the CRC32 computed for data_bytes
int crc32_valid(uint32_t digest, const uint8_t *data_bytes, size_t len, uint32_t *crc32_table)
{
    return digest == compute_crc32(data_bytes, len, crc32_table);
}
