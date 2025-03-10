#ifndef UTILS_H
#define UTILS_H

int get_command_code(const char *command);
void remove_new_line(char *str);
void rand_str(char *str, size_t length);
int valid_char_digit(const char *str);
int is_file_extension(const char *file_name, const char *extension);

#endif