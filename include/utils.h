#include <stdint.h>

#ifndef UTILS_H
#define UTILS_H

int get_command_code(const char *command);
void remove_new_line(char *str);

void write_uint32_le(uint8_t *buffer, uint32_t value);
uint32_t read_uint32_le(const uint8_t *buffer);
void write_uint16_le(uint8_t *buffer, uint16_t value);
uint16_t read_uint16_le(const uint8_t *buffer);

#endif