#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../include/commands.h"
#include "../include/utils.h"
#include "../include/repos.h"
#include "../include/diskstore.h"
#include "../include/structs.h"

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        printf("No arguments were provided. Available commands:\n");
        help();
        return 1;
    }

    uint32_t crc32_table[256];

    DiskStore *store = DiskStore_new("data/repos.db", crc32_table);
    // DiskStore_get(store, "testkey3", crc32_table);
    printf("%ld\n", store->key_dir->ks.write_pos);

    // const char *put_result = DiskStore_put(store, "testkey3", "testval3", crc32_table);
    // printf("%s\n", put_result);


    const char *test = DiskStore_get(store, "testkey2", crc32_table);
    // printf("%s\n", test);
    // printf("test\n");
    // printf("%s\n", store->key_dir->ks.log_size);
    // printf("%s\n", put_result);

    // const char *get_result = DiskStore_keys(store);
    // printf("%s\n", get_result);

    // int repos_count = 0;
    // char *repos_path = "data/repos.txt";

    // Repo *repos = get_repos(repos_path, &repos_count);

    // switch (get_command_code(argv[1]))
    // {
    // case 0:
    //     if (argc < 3) {
    //         printf("Invalid arguments\n");
    //         return 1;
    //     }
    //     add(repos_path, repos, &repos_count, argv[2], argv[3]);
    //     break;
    // case 1:
    //     if (argc < 2) {
    //         get(repos, &repos_count, NULL);
    //     } else {
    //         get(repos, &repos_count, argv[2]);
    //     }
    //     break;
    // case 2:
    //     list(repos, &repos_count);
    //     break;
    // case 3:
    //     if (argc < 2) {
    //         delete(repos_path, repos, &repos_count, NULL);
    //     } else {
    //         delete(repos_path, repos, &repos_count, argv[2]);
    //     }
    //     break;
    // case 4:
    //     help();
    //     break;
    
    // default:
    //     printf("Invalid command %s. Available commands:\n", argv[1]);
    //     help();
    //     return 1;
    // }

    return 0;
}