#include <stdio.h>
#include <string.h>

#include "../include/commands.h"
#include "../include/structs.h"

#define COMMAND_LIST_ARR_SIZE 5


void print_help()
{
    const int INDENTATION = 5;

    Command_list command_list_arr[COMMAND_LIST_ARR_SIZE];

    command_list_arr[0].name = "--add | -a <alias> <repository_link>";
    command_list_arr[0].info = "adds a new repository";
    command_list_arr[0].indent = strlen(command_list_arr[0].info) + INDENTATION;

    command_list_arr[1].name = "--show | -s <alias | all>";
    command_list_arr[1].info = "show the repository link of a given alias or show all links";
    command_list_arr[1].indent = strlen(command_list_arr[1].info) + INDENTATION;;

    command_list_arr[2].name = "--list | -l";
    command_list_arr[2].info = "lists all aliases";
    command_list_arr[2].indent = strlen(command_list_arr[2].info) + INDENTATION;;

    command_list_arr[3].name = "--delete | -d <alias>";
    command_list_arr[3].info = "deletes the given entry";
    command_list_arr[3].indent = strlen(command_list_arr[3].info) + INDENTATION;;

    command_list_arr[4].name = "--help | -h";
    command_list_arr[4].info = "lists all available commands";
    command_list_arr[4].indent = strlen(command_list_arr[4].info) + INDENTATION;;

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