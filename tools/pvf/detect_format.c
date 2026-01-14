/**
 * @file detect_format.c
 * @brief 检测装备文件格式（文本 vs 二进制）
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "pvf/pvf.h"

static int is_text_file(const unsigned char* data, size_t size) {
    size_t i;
    size_t text_chars = 0;
    size_t check_size = (size < 512) ? size : 512;

    for (i = 0; i < check_size; i++) {
        unsigned char c = data[i];
        if (isprint(c) || isspace(c) || c >= 0x80) {  /* 包含UTF-8 */
            text_chars++;
        }
    }

    return (text_chars * 100 / check_size) > 80;
}

int main(int argc, char* argv[]) {
    PackSetInternal pack;
    int i;
    int text_count = 0;
    int binary_count = 0;
    int checked = 0;

    if (argc < 2) {
        printf("用法: %s <pvf_file>\n", argv[0]);
        return 1;
    }

    PVF_Initialize();

    if (PackSet_Open(argv[1], &pack) != PVF_SUCCESS) {
        printf("错误: 无法打开PVF\n");
        return 1;
    }

    printf("扫描 .equ 文件格式...\n\n");

    for (i = 0; i < (int)pack.header.index_count && checked < 100; i++) {
        if (strstr(pack.indexes[i].name, ".equ") == NULL) {
            continue;
        }

        PVFIndexEntry* entry = &pack.indexes[i];
        unsigned char* buffer = (unsigned char*)malloc(entry->size);

        if (PackSet_ExtractFile(&pack, entry, buffer, entry->size) == PVF_SUCCESS) {
            if (is_text_file(buffer, entry->size)) {
                text_count++;
                if (text_count <= 3) {  /* 显示前3个 */
                    printf("[文本] %s (%u 字节)\n", entry->name, entry->size);
                    printf("  前100字节: ");
                    int j;
                    for (j = 0; j < 100 && j < (int)entry->size; j++) {
                        if (isprint(buffer[j])) {
                            putchar(buffer[j]);
                        } else if (buffer[j] == '\n') {
                            printf("\\n");
                        } else {
                            putchar('.');
                        }
                    }
                    printf("\n\n");
                }
            } else {
                binary_count++;
                if (binary_count <= 3) {  /* 显示前3个 */
                    printf("[二进制] %s (%u 字节)\n", entry->name, entry->size);
                    printf("  魔数: 0x%02x%02x\n", buffer[0], buffer[1]);
                    printf("\n");
                }
            }
        }

        free(buffer);
        checked++;
    }

    printf("========================================\n");
    printf("统计（扫描%d个文件）:\n", checked);
    printf("  文本格式: %d\n", text_count);
    printf("  二进制格式: %d\n", binary_count);
    printf("========================================\n");

    PackSet_Close(&pack);
    return 0;
}
