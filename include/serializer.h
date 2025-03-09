#include <stdint.h>

#ifndef SERIALIZER_H
#define SERIALIZER_H

void serialize_header(uint8_t *header, uint32_t epoch, uint32_t keysz, uint32_t valuesz, uint16_t key_type, uint16_t value_type);
void deserialize_header(const uint8_t *header_data, uint32_t *epoch, uint32_t *keysz, uint32_t *valuesz, DataType *key_type, DataType *value_type);

uint8_t *pack_value(const void *attribute, DataType attribute_type, size_t *out_len);
char *unpack_value(const uint8_t *attribute, size_t len, DataType attribute_type);

size_t serialize(uint32_t epoch, const void *key, const void *value, uint8_t **out_data, uint32_t *crc32_table);
int deserialize(const uint8_t *data, size_t len, uint32_t *epoch, char **key_out, char **value_out, uint32_t *crc32_table);

#endif