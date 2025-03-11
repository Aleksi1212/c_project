#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <stdio.h>

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
    if (strcmp(command, "--populate") == 0 || strcmp(command, "-p") == 0)
        return 4;
    if (strcmp(command, "--help") == 0 || strcmp(command, "-h") == 0)
        return 5;

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

void rand_str(char *str, size_t length)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    for (size_t i = 0; i < length; i++) {
        int randomIndex = rand() % (sizeof(charset) - 1);
        str[i] = charset[randomIndex];
    }
    str[length] = '\0';
}

int valid_char_digit(const char *str)
{
    if (str == NULL || *str == '\0') {
        return 0;
    }

    while (*str == ' ') {
        str++;
    }

    if (*str == '-') {
        str++;
    }

    while (*str) {
        if (!isdigit(*str)) {
            return 0;
        }
        str++;
    }

    return 1;
}

int is_file_extension(const char *file_name, const char *extension)
{
    const char *ext = strrchr(file_name, '.');
    return ext != NULL && strcmp(ext, extension) == 0;
}

void remove_file_from_dir_string(char *dir)
{
    if (is_file_extension(dir, ".txt") == 1) {
        char *token = strtok(dir, "/");
        char base_path[256] = "/";
        struct stat sb;
        while (token)
        {
            if (is_file_extension(token, ".txt") != 1) {
                if (stat(token, &sb) != 0) {
                    strcat(base_path, "/");
                    strcat(base_path, token);
                }
            }
            token = strtok(NULL, "/");
        }
        memmove(dir, base_path + 2, strlen(base_path));
    }
}

void make_dir(const char *dir)
{
    char temp[256];
    char *pos = NULL;
    size_t len;

    snprintf(temp, sizeof(temp), "%s", dir);
    len = strlen(temp);

    if (temp[len - 1] == '/')
        temp[len - 1] = '\0';

    for (pos = temp + 1; *pos; pos++) {
        if (*pos == '/') {
            *pos = '\0';
            mkdir(temp, 0777);
            *pos = '/';
        }
    }
    mkdir(temp, 0777);
}

