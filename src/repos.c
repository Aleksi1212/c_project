#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/structs.h"
#include "../include/repos.h"
#include "../include/utils.h"

int get_repos(const char *file_path, Repo **repos, int *count)
{
    if (is_file_extension(file_path, ".txt") != 1) {
        return -1;
    }

    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        return 1;
    }

    int repos_count = 0;
    int repos_size = 2;

    *repos = (Repo *)malloc(repos_size * sizeof(Repo));
    if (repos == NULL) {
        return -2;
    }

    char line[MAX_LINE_SIZE];
    while (fgets(line, sizeof(line), file))
    {
        if (repos_count == repos_size) {
            repos_size *= 2;
            Repo *temp = (Repo *)realloc(*repos, repos_size * sizeof(Repo));
            if (temp == NULL) {
                free(*repos);
                return -2;
            }
            
            *repos = temp;
        }

        char *token = strtok(line, "|");
        int token_count = 0;

        while (token != NULL)
        {
            remove_new_line(token);

            if (token_count == 0) {
                sprintf((*repos)[repos_count].alias, token, sizeof((*repos)[repos_count].alias));
            }
            if (token_count == 1) {
                sprintf((*repos)[repos_count].link, token, sizeof((*repos)[repos_count].link));
            }

            token = strtok(NULL, "|");
            token_count++;
        }

        repos_count++;
    }

    *count = repos_count;
    fclose(file);

    return 1;
}
