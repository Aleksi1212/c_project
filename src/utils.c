#include <string.h>
#include <stdint.h>

#include "../include/utils.h"

int get_command_code(const char *command)
{
    if (strcmp(command, "--add") == 0 || strcmp(command, "-a") == 0)
        return 0;
    if (strcmp(command, "--get") == 0 || strcmp(command, "-g") == 0)
        return 1;
    if (strcmp(command, "--list") == 0 || strcmp(command, "-l") == 0)
        return 2;
    if (strcmp(command, "--delete") == 0 || strcmp(command, "-d") == 0)
        return 3;
    if (strcmp(command, "--help") == 0 || strcmp(command, "-h") == 0)
        return 4;

    return -1;
}

void remove_new_line(char *str)
{
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n')
    {
        str[len - 1] = '\0';
    }
}

void write_uint32_le(uint8_t *buffer, uint32_t value)
{
    buffer[0] = (uint8_t)(value & 0xff);
    buffer[1] = (uint8_t)((value >> 8) & 0xff);
    buffer[2] = (uint8_t)((value >> 16) & 0xff);
    buffer[3] = (uint8_t)((value >> 24) & 0xff);
}

uint32_t read_uint32_le(const uint8_t *buffer)
{
    return ((uint32_t)buffer[0]) |
        (((uint32_t)buffer[1]) << 8) |
        (((uint32_t)buffer[2]) << 16) |
        (((uint32_t)buffer[3]) << 24);
}

void write_uint16_le(uint8_t *buffer, uint16_t value)
{
    buffer[0] = (uint8_t)(value & 0xff);
    buffer[1] = (uint8_t)((value >> 8) & 0xff);
}

uint16_t read_uint16_le(const uint8_t *buffer)
{
    return (uint16_t)(buffer[0] | (buffer[1] << 8));
}