#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/structs.h"
#include "../include/repos.h"
#include "../include/utils.h"

Repo *get_repos(const char *file_path, int *count)
{
    int repos_size = 2;
    Repo *repos = (Repo *)malloc(repos_size * sizeof(Repo));

    if (is_file_extension(file_path, ".txt") != 1) {
        repos[0].status = 1;
        return repos;
    }

    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        repos[0].status = 0;
        return repos;
    }

    int repos_count = 0;


    char line[MAX_LINE_SIZE];
    while (fgets(line, sizeof(line), file))
    {
        if (repos_count == repos_size) {
            repos_size *= 2;
            repos = (Repo *)realloc(repos, repos_size * sizeof(Repo));
        }

        char *token = strtok(line, ";");
        int token_count = 0;

        while (token != NULL)
        {
            remove_new_line(token);

            if (token_count == 0) {
                sprintf(repos[repos_count].alias, token, sizeof(repos[repos_count].alias));
            }
            if (token_count == 1) {
                sprintf(repos[repos_count].link, token, sizeof(repos[repos_count].link));
            }

            token = strtok(NULL, ";");
            token_count++;
        }
        repos[repos_count].status = 0;

        repos_count++;
    }

    *count = repos_count;
    // repos_ex = repos;
    fclose(file);

    return repos;
}
