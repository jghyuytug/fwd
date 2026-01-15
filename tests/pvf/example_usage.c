/**
 * @file example_usage.c
 * @brief PVF库完整使用示例
 *
 * @details 演示如何使用PVF库的所有主要功能:
 *          1. 初始化和打开PVF文件
 *          2. 查找文件
 *          3. 提取文件内容
 *          4. 批量处理
 *          5. 错误处理
 *          6. 资源清理
 *
 * @usage
 *     gcc -m32 -I../../include -o example_usage example_usage.c \
 *         ../../src/pvf/pvf_crc32.c \
 *         ../../src/pvf/pvf_decrypt.c \
 *         ../../src/pvf/pvf_hash.c \
 *         ../../src/pvf/pvf_parser.c \
 *         ../../src/pvf/pvf_packset.c
 *     ./example_usage
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pvf/pvf.h"

/* ========================================================================== */
/* 示例1: 基本使用 - 打开、查找、提取、关闭                                    */
/* ========================================================================== */

void example_basic_usage(void) {
    PackSetInternal pack;
    PVFIndexEntry* entry;
    char* buffer = NULL;
    int result;

    printf("\n========================================\n");
    printf("示例1: 基本使用\n");
    printf("========================================\n\n");

    /* 步骤1: 初始化PVF库 */
    printf("[1] 初始化PVF库...\n");
    PVF_Initialize();

    /* 步骤2: 打开PVF文件 */
    printf("[2] 打开PVF文件...\n");
    result = PackSet_Open("/home/dxf/game/Script.pvf", &pack);

    if (result != PVF_SUCCESS) {
        printf("    错误: 无法打开PVF文件 (错误码: %d)\n", result);
        return;
    }

    printf("    成功! 文件数量: %u\n", pack.header.index_count);

    /* 步骤3: 查找文件 */
    printf("[3] 查找文件: \"equipment/character/common/wrist/brac_2choro994.equ\"\n");
    entry = PackSet_GetFile(&pack, "equipment/character/common/wrist/brac_2choro994.equ");

    if (entry == NULL) {
        printf("    错误: 文件未找到\n");
        PackSet_Close(&pack);
        return;
    }

    printf("    找到! 文件大小: %u 字节\n", entry->size);

    /* 步骤4: 分配缓冲区 */
    printf("[4] 分配缓冲区...\n");
    buffer = (char*)malloc(entry->size);
    if (buffer == NULL) {
        printf("    错误: 内存分配失败\n");
        PackSet_Close(&pack);
        return;
    }

    /* 步骤5: 提取文件 */
    printf("[5] 提取文件内容...\n");
    result = PackSet_ExtractFile(&pack, entry, buffer, entry->size);

    if (result != PVF_SUCCESS) {
        printf("    错误: 文件提取失败 (错误码: %d)\n", result);
        free(buffer);
        PackSet_Close(&pack);
        return;
    }

    printf("    成功! 前16字节 (hex): ");
    {
        int i;
        int display_size = (entry->size < 16) ? entry->size : 16;
        for (i = 0; i < display_size; i++) {
            printf("%02x ", (unsigned char)buffer[i]);
        }
        printf("\n");
    }

    /* 步骤6: 清理资源 */
    printf("[6] 清理资源...\n");
    free(buffer);
    PackSet_Close(&pack);

    printf("    完成!\n");
}

/* ========================================================================== */
/* 示例2: 一步提取 - 使用便捷API                                               */
/* ========================================================================== */

void example_quick_extract(void) {
    PackSetInternal pack;
    char buffer[65536];
    uint32_t size;
    int result;

    printf("\n========================================\n");
    printf("示例2: 一步提取（便捷API）\n");
    printf("========================================\n\n");

    /* 初始化和打开 */
    PVF_Initialize();
    if (PackSet_Open("/home/dxf/game/Script.pvf", &pack) != PVF_SUCCESS) {
        printf("错误: 无法打开PVF文件\n");
        return;
    }

    /* 一步提取 */
    printf("提取文件: \"monster/newmonsters/shonan/kaizen/action/teleport.act\"\n");

    result = PackSet_ExtractFileByPath(
        &pack,
        "monster/newmonsters/shonan/kaizen/action/teleport.act",
        buffer,
        sizeof(buffer),
        &size
    );

    if (result == PVF_SUCCESS) {
        printf("成功! 文件大小: %u 字节\n", size);
    } else {
        printf("失败! 错误码: %d\n", result);
    }

    PackSet_Close(&pack);
}

/* ========================================================================== */
/* 示例3: 批量提取 - 提取多个文件                                              */
/* ========================================================================== */

void example_batch_extract(void) {
    PackSetInternal pack;
    const char* files[] = {
        "creature/2wu_gunner/prime/animation/run/02_prime_run_01_dodge.ani",
        "equipment/character/common/wrist/brac_2choro994.equ",
        "passiveobject/character/gunner/animation/stingerex/subdodgenotarget.ani"
    };
    int file_count = 3;
    int i;
    int success_count = 0;

    printf("\n========================================\n");
    printf("示例3: 批量提取\n");
    printf("========================================\n\n");

    /* 初始化和打开 */
    PVF_Initialize();
    if (PackSet_Open("/home/dxf/game/Script.pvf", &pack) != PVF_SUCCESS) {
        printf("错误: 无法打开PVF文件\n");
        return;
    }

    printf("提取 %d 个文件:\n\n", file_count);

    /* 批量提取 */
    for (i = 0; i < file_count; i++) {
        PVFIndexEntry* entry;
        char* buffer;
        int result;

        printf("[%d/%d] %s\n", i + 1, file_count, files[i]);

        /* 查找 */
        entry = PackSet_GetFile(&pack, files[i]);
        if (entry == NULL) {
            printf("      [失败] 文件未找到\n");
            continue;
        }

        /* 分配缓冲区 */
        buffer = (char*)malloc(entry->size);
        if (buffer == NULL) {
            printf("      [失败] 内存分配失败\n");
            continue;
        }

        /* 提取 */
        result = PackSet_ExtractFile(&pack, entry, buffer, entry->size);
        if (result != PVF_SUCCESS) {
            printf("      [失败] 提取失败 (错误码: %d)\n", result);
            free(buffer);
            continue;
        }

        printf("      [成功] %u 字节\n", entry->size);
        success_count++;

        /* 这里可以处理文件内容... */

        free(buffer);
    }

    printf("\n批量提取完成: %d/%d 成功\n", success_count, file_count);

    PackSet_Close(&pack);
}

/* ========================================================================== */
/* 示例4: 错误处理 - 处理各种错误情况                                          */
/* ========================================================================== */

void example_error_handling(void) {
    PackSetInternal pack;
    PVFIndexEntry* entry;
    char buffer[1024];
    int result;

    printf("\n========================================\n");
    printf("示例4: 错误处理\n");
    printf("========================================\n\n");

    PVF_Initialize();

    /* 测试1: 打开不存在的文件 */
    printf("[测试1] 打开不存在的文件\n");
    result = PackSet_Open("/nonexistent/file.pvf", &pack);
    if (result == PVF_ERROR_FILE_NOT_FOUND) {
        printf("        正确处理: 文件未找到\n");
    }

    /* 测试2: 查找不存在的文件 */
    printf("[测试2] 查找不存在的文件\n");
    if (PackSet_Open("/home/dxf/game/Script.pvf", &pack) == PVF_SUCCESS) {
        entry = PackSet_GetFile(&pack, "nonexistent/file.txt");
        if (entry == NULL) {
            printf("        正确处理: 返回NULL\n");
        }

        /* 测试3: 缓冲区太小 */
        printf("[测试3] 缓冲区太小\n");
        entry = PackSet_GetFile(&pack, "equipment/character/common/wrist/brac_2choro994.equ");
        if (entry != NULL) {
            result = PackSet_ExtractFile(&pack, entry, buffer, 10);  /* 缓冲区只有10字节 */
            if (result == PVF_ERROR_MEMORY) {
                printf("        正确处理: 缓冲区太小\n");
            }
        }

        PackSet_Close(&pack);
    }

    printf("\n错误处理测试完成!\n");
}

/* ========================================================================== */
/* 示例5: 遍历所有文件 - 列出PVF中的所有文件                                   */
/* ========================================================================== */

void example_list_all_files(void) {
    PackSetInternal pack;
    uint32_t i;
    uint32_t display_count = 20;  /* 只显示前20个 */

    printf("\n========================================\n");
    printf("示例5: 列出文件（前20个）\n");
    printf("========================================\n\n");

    PVF_Initialize();

    if (PackSet_Open("/home/dxf/game/Script.pvf", &pack) != PVF_SUCCESS) {
        printf("错误: 无法打开PVF文件\n");
        return;
    }

    printf("总文件数: %u\n\n", pack.header.index_count);

    for (i = 0; i < display_count && i < pack.header.index_count; i++) {
        PVFIndexEntry* entry = &pack.indexes[i];
        printf("[%5u] %s\n", i + 1, entry->name);
        printf("        大小: %u 字节, 哈希: 0x%08x\n",
               entry->size, entry->hash_value);
    }

    if (pack.header.index_count > display_count) {
        printf("\n... 还有 %u 个文件未显示\n",
               pack.header.index_count - display_count);
    }

    PackSet_Close(&pack);
}

/* ========================================================================== */
/* 主函数                                                                      */
/* ========================================================================== */

int main(void) {
    printf("========================================\n");
    printf("PVF库使用示例程序\n");
    printf("版本: %s\n", PVF_GetVersion());
    printf("========================================\n");

    /* 运行所有示例 */
    example_basic_usage();
    example_quick_extract();
    example_batch_extract();
    example_error_handling();
    example_list_all_files();

    printf("\n========================================\n");
    printf("所有示例运行完成!\n");
    printf("========================================\n\n");

    return 0;
}
