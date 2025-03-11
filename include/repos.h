#include <stdio.h>
#include "structs.h"

#ifndef REPO_MANAGER_H
#define REPO_MANAGER_H

int get_repos(const char *file_path, Repo **repos, int *count);

#endif