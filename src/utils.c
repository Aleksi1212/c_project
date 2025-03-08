#include <string.h>
#include "../include/utils.h"

int get_command_code(const char *command)
{
    if (strcmp(command, "--add") == 0 || strcmp(command, "-a") == 0)
        return 0;
    if (strcmp(command, "--show") == 0 || strcmp(command, "-s") == 0)
        return 1;
    if (strcmp(command, "--list") == 0 || strcmp(command, "-l") == 0)
        return 2;
    if (strcmp(command, "--delete") == 0 || strcmp(command, "-d") == 0)
        return 3;
    if (strcmp(command, "--help") == 0 || strcmp(command, "-h") == 0)
        return 4;

    return -1;
}