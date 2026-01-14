/**
 * @file list_directory.c
 * @brief 列出PVF中特定目录的文件
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pvf/pvf.h"

int main(int argc, char* argv[]) {
    PackSetInternal pack;
    uint32_t i;
    const char* pvf_path;
    const char* dir_prefix;
    int count = 0;
    int max_display = 50;

    if (argc < 3) {
        printf("用法: %s <pvf_file> <directory_prefix>\n", argv[0]);
        printf("示例: %s Script.pvf etc/\n", argv[0]);
        return 1;
    }

    pvf_path = argv[1];
    dir_prefix = argv[2];

    PVF_Initialize();

    if (PackSet_Open(pvf_path, &pack) != PVF_SUCCESS) {
        printf("错误: 无法打开PVF文件\n");
        return 1;
    }

    printf("查找目录: %s\n\n", dir_prefix);
    printf("%-60s %10s\n", "文件名", "大小");
    printf("--------------------------------------------------------------------\n");

    size_t prefix_len = strlen(dir_prefix);
    for (i = 0; i < pack.header.index_count; i++) {
        if (strncmp(pack.indexes[i].name, dir_prefix, prefix_len) == 0) {
            printf("%-60s %10u\n", pack.indexes[i].name, pack.indexes[i].size);
            count++;
            if (count >= max_display) {
                printf("... (还有更多文件)\n");
                break;
            }
        }
    }

    printf("\n共找到 %d 个文件\n", count);

    PackSet_Close(&pack);
    return 0;
}
