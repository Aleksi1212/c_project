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

    DiskStore *store = disk_store_new("data/repos.db");

    const char *test = disk_store_put(store, "key", "val");
    const char *test1 = disk_store_put(store, "key1", "val1");
    const char *test2 = disk_store_put(store, "key2", "val2");
    const char *test3 = disk_store_put(store, "key3", "val3");

    const char *test_get = disk_store_get(store, "key");
    const char *test_get1 = disk_store_get(store, "key1");
    const char *test_get2 = disk_store_get(store, "key2");
    const char *test_get3 = disk_store_get(store, "key3");

    printf("%s\n", test);
    printf("%s\n", test1);
    printf("%s\n", test2);
    printf("%s\n", test3);

    printf("%s\n", test_get);
    printf("%s\n", test_get1);
    printf("%s\n", test_get2);
    printf("%s\n", test_get3);

    const char *keys = disk_store_keys(store);
    printf("%s\n", keys);

    const char *test_del = disk_store_delete(store, "key");
    const char *test_del1 = disk_store_delete(store, "key1");
    const char *test_del2 = disk_store_delete(store, "key2");
    const char *test_del3 = disk_store_delete(store, "key3");

    printf("%s\n", test_del);
    printf("%s\n", test_del1);
    printf("%s\n", test_del2);
    printf("%s\n", test_del3);

    const char *keys1 = disk_store_keys(store);
    printf("%s\n", keys1);



    // uint32_t crc32_table[256];

    // DiskStore *store = DiskStore_new("data/repos.db", crc32_table);
    // // DiskStore_get(store, "testkey3", crc32_table);
    // printf("%ld\n", store->key_dir->ks.write_pos);

    // // const char *put_result = DiskStore_put(store, "testkey3", "testval3", crc32_table);
    // // printf("%s\n", put_result);


    // const char *test = DiskStore_get(store, "testkey2", crc32_table);
    // // printf("%s\n", test);
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