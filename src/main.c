#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "../include/commands.h"
#include "../include/utils.h"
#include "../include/repos.h"

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        printf("No arguments were provided. Available commands:\n");
        help();
        return 1;
    }
    int return_value = 0;

    int repos_count = 0;
    char repos_path[] = "data/repos.txt";
    Repo *repos = NULL;

    int status = get_repos(repos_path, &repos, &repos_count);
    if (status == -1) {
        printf("Invalid data file %s. Must be .txt\n", repos_path);
        return_value = 1;
    }
    else if (status == -2) {
        printf("Memory allocation failed\n");
        return_value = 1;
    }
    else {
        switch (get_command_code(argv[1]))
        {
        case 0:
            if (argc < 3) {
                printf("Invalid arguments\n");
                return_value = 1;
            }
            else {
                add(repos_path, repos, &repos_count, argv[2], argv[3]);
            }
            break;
        case 1:
            if (argc < 2) {
                get(repos, &repos_count, NULL);
            } else {
                get(repos, &repos_count, argv[2]);
            }
            break;
        case 2:
            list(repos, &repos_count);
            break;
        case 3:
            if (argc < 2) {
                delete(repos_path, repos, &repos_count, NULL);
            } else {
                delete(repos_path, repos, &repos_count, argv[2]);
            }
            break;
        case 4:
            if (argc < 4) {
                printf("Invalid arguments\n");
                return_value = 1;
            }
            else {
                int count_valid = valid_char_digit(argv[2]);
                int key_size_valid = valid_char_digit(argv[2]);
                int value_size_valid = valid_char_digit(argv[3]);
        
                if (count_valid == 0 || key_size_valid == 0 || value_size_valid == 0) {
                    printf("Invalid arguments\n");
                    return_value = 1;
                }
                else {
                    int count = atoi(argv[2]);
                    size_t key_size = strtoull(argv[3], NULL, 10);
                    size_t value_size = strtoull(argv[4], NULL, 10);
            
                    populate(repos_path, repos, &repos_count, count, key_size, value_size);
                }
            }
            break;
        case 5:
            help();
            break;
        default:
            printf("Invalid command %s. Available commands:\n", argv[1]);
            help();
            break;
        }
    }

    free(repos);
    return return_value;
}