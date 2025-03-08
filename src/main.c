#include <stdio.h>

#include "../include/commands.h"
#include "../include/utils.h"

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        printf("No arguments were provided. Available commands:\n");
        print_help();
        return 1;
    }

    switch (get_command_code(argv[1]))
    {
    case 0:
        /* code */
        break;
    case 1:
        /* code */
        break;
    case 2:
        /* code */
        break;
    case 3:
        /* code */
        break;
    case 4:
        print_help();
        break;
    
    default:
        printf("Invalid command %s. Available commands:\n", argv[1]);
        print_help();
        return 1;
    }

    return 0;
}