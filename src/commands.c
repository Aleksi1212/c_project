#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "../include/commands.h"
#include "../include/structs.h"
#include "../include/repos.h"
#include "../include/utils.h"
#include "../include/constants.h"

void add(const char *repos_path, const Repo *repos, int *repos_count, const char *alias, const char *link)
{
    if (strchr(alias, ';') != NULL || strchr(link, ';') != NULL) {
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

    FILE *file = fopen(repos_path, "a+");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }

    fprintf(file, "%s;%s\n", alias, link);
    fclose(file);

    printf("Succesfully added alias\n");
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
    if (alias == NULL) {
        remove(repos_path);
        printf("Succesfully removed\n");
        return;
    }

    if (repos != NULL) {
        FILE *file = fopen(repos_path, "w");

        for (int i = 0; i < *repos_count; i++) {
            if (strcmp(repos[i].alias, alias) != 0) {
                fprintf(file, "%s;%s\n", repos[i].alias, repos[i].link);
            }
        }

        printf("Successfully removed.\n");
        return;
    }
    printf("File is empty\n");
}

void populate(const char* repos_path, const Repo *repos, int *repos_count, int count, size_t key_size, size_t value_size)
{
    if (key_size > MAX_KEY_SIZE || value_size > MAX_VALUE_SIZE) {
        printf("Key or value size too large.\n");
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
    const int INDENTATION = 5;

    Command_list command_list_arr[COMMAND_LIST_ARR_SIZE];

    command_list_arr[0].name = "--add <alias> <repository_link> | -a <alias> <repository_link>";
    command_list_arr[0].info = "adds a new repository";
    command_list_arr[0].indent = strlen(command_list_arr[0].info) + INDENTATION;

    command_list_arr[1].name = "--get <alias> | -g <alias> | --get | -g";
    command_list_arr[1].info = "get the repository link of a given alias or get all links";
    command_list_arr[1].indent = strlen(command_list_arr[1].info) + INDENTATION;

    command_list_arr[2].name = "--list | -l";
    command_list_arr[2].info = "lists all aliases";
    command_list_arr[2].indent = strlen(command_list_arr[2].info) + INDENTATION;

    command_list_arr[3].name = "--delete <alias> | -d <alias>";
    command_list_arr[3].info = "deletes the given alias";
    command_list_arr[3].indent = strlen(command_list_arr[3].info) + INDENTATION;

    command_list_arr[4].name = "--populate <count> <key_size> <value_size> | -p <count> <key_size> <value_size>";
    command_list_arr[4].info = "populate with data <count> amount of data with key being of <key_size> and value of <value_size>";
    command_list_arr[4].indent = strlen(command_list_arr[4].info) + INDENTATION;

    command_list_arr[5].name = "--help | -h";
    command_list_arr[5].info = "lists all available commands";
    command_list_arr[5].indent = strlen(command_list_arr[5].info) + INDENTATION;

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
