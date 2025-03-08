#include <stdio.h>
#include <string.h>

#include "../include/commands.h"
#include "../include/structs.h"
#include "../include/repos.h"

#define COMMAND_LIST_ARR_SIZE 5

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
    if (repos != NULL) {
        for (int i = 0; i < *repos_count; i++)
        {
            if (alias != NULL) {
                if (strcmp(repos[i].alias, alias) == 0) {
                    printf("%s\n", repos[i].link);
                    return;
                }
            } else {
                printf("%s\n", repos[i].link);
            }
        }
        return;
    }

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

    command_list_arr[4].name = "--help | -h";
    command_list_arr[4].info = "lists all available commands";
    command_list_arr[4].indent = strlen(command_list_arr[4].info) + INDENTATION;

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
