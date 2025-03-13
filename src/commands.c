#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

#include "../include/commands.h"
#include "../include/structs.h"
#include "../include/repos.h"
#include "../include/utils.h"
#include "../include/constants.h"

void add(char *repos_path, const Repo *repos, int *repos_count, const char *alias, const char *link)
{
    if (strchr(alias, '|') != NULL || strchr(link, '|') != NULL) {
        printf("Invalid alias or link.\n");
        return;
    }

    if (repos != NULL) {
        for (int i = 0; i < *repos_count; i++)
        {
            if (strcmp(repos[i].alias, alias) == 0) {
                printf("Already exists\n");
                return;
            }
        }
    }

    char path_copy[2048];
    strcpy(path_copy, repos_path);

    remove_file_from_dir_string(path_copy);
    make_dir(path_copy);

    FILE *file = fopen(repos_path, "a+");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }

    fprintf(file, "%s|%s\n", alias, link);
    fclose(file);

    printf("OK\n");
}

void get(const Repo *repos, int *repos_count, const char *alias)
{   
    bool not_found = true;

    if (repos != NULL) {
        for (int i = 0; i < *repos_count; i++)
        {
            if (alias != NULL) {
                if (strcmp(repos[i].alias, alias) == 0) {
                    printf("%s\n", repos[i].link);
                    return;
                }
            } else {
                not_found = false;
                printf("%s\n", repos[i].link);
            }
        }
    }

    if (not_found)
        printf("Not found\n");
}

void list(const Repo *repos, int *repos_count)
{
    if (repos != NULL) {
        for (int i = 0; i < *repos_count; i++)
            printf("%s\n", repos[i].alias);
    }
}

void delete(const char *repos_path, Repo *repos, int *repos_count, const char *alias)
{
    if (repos != NULL) {
        if (alias == NULL) {
            remove(repos_path);
            printf("OK\n");
            return;
        }

        FILE *file = fopen(repos_path, "w");
        if (file == NULL) {
            printf("Error opening file\n");
            return;
        }

        for (int i = 0; i < *repos_count; i++) {
            if (strcmp(repos[i].alias, alias) != 0) {
                fprintf(file, "%s|%s\n", repos[i].alias, repos[i].link);
            }
        }
        fclose(file);
        printf("OK\n");
        return;
    }
    printf("File is empty\n");
}

void populate(char* repos_path, const Repo *repos, int *repos_count, int count, size_t key_size, size_t value_size)
{
    if (key_size > MAX_KEY_SIZE || value_size > MAX_VALUE_SIZE) {
        printf("Key or value size too large. MAX key size: %d, MAX value size: %d\n", MAX_KEY_SIZE, MAX_VALUE_SIZE);
        return;
    }

    srand(time(NULL));

    for (int i = 0; i < count; i++) {
        char *rand_alias = malloc(key_size);
        char *rand_link = malloc(value_size);

        rand_str(rand_alias, key_size);
        rand_str(rand_link, value_size);

        add(repos_path, repos, repos_count, rand_alias, rand_link);

        free(rand_alias);
        free(rand_link);
    }
}

void help()
{
    Command_list command_list_arr[COMMAND_LIST_ARR_SIZE];

    command_list_arr[0].name = "--add <alias> <repository_link> | -a <alias> <repository_link>";
    command_list_arr[0].info = "adds a new repository";
    command_list_arr[0].indent = strlen(command_list_arr[0].info) + INFO_INDENTATION;

    command_list_arr[1].name = "--get <alias> | -g <alias> | --get | -g";
    command_list_arr[1].info = "get the repository link of a given alias or get all links";
    command_list_arr[1].indent = strlen(command_list_arr[1].info) + INFO_INDENTATION;

    command_list_arr[2].name = "--list | -l";
    command_list_arr[2].info = "lists all aliases";
    command_list_arr[2].indent = strlen(command_list_arr[2].info) + INFO_INDENTATION;

    command_list_arr[3].name = "--delete <alias> | -d <alias> | --delete | -d";
    command_list_arr[3].info = "deletes the given alias or no alias to delete all";
    command_list_arr[3].indent = strlen(command_list_arr[3].info) + INFO_INDENTATION;

    command_list_arr[4].name = "--populate <count> <key_size> <value_size> | -p <count> <key_size> <value_size>";
    command_list_arr[4].info = "populate with data <count> amount of data with key being of <key_size> and value of <value_size>";
    command_list_arr[4].indent = strlen(command_list_arr[4].info) + INFO_INDENTATION;

    command_list_arr[5].name = "--help | -h";
    command_list_arr[5].info = "lists all available commands";
    command_list_arr[5].indent = strlen(command_list_arr[5].info) + INFO_INDENTATION;

    for (int i = 0; i < COMMAND_LIST_ARR_SIZE; i++)
    {
        printf
        (
            "%s\n%*s\n\n",
            command_list_arr[i].name,
            command_list_arr[i].indent,
            command_list_arr[i].info
        );
    }
}
