#include "structs.h"

#ifndef COMMANDS_H
#define COMMANDS_H

void add(const char *repos_path, const Repo *repos, int *repos_count, const char *alias, const char *link);
void get(const Repo *repos, int *repos_count, const char *alias);
void list(const Repo *repos, int *repos_count);
void delete(const char *repos_path, Repo *repos, int *repos_count, const char *alias);

void help();

#endif