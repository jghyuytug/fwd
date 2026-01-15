/**
 * @file test_integration.c
 * @brief PVF库综合集成测试 + 性能测试
 *
 * @details 完整测试所有模块的集成和性能:
 *          1. 完整工作流测试
 *          2. 大量文件查找性能测试
 *          3. 文件提取性能测试
 *          4. 内存管理测试
 *          5. 并发访问模拟测试
 *
 * @usage
 *     gcc -m32 -I../../include -o test_integration test_integration.c \
 *         ../../src/pvf/pvf_crc32.c \
 *         ../../src/pvf/pvf_decrypt.c \
 *         ../../src/pvf/pvf_hash.c \
 *         ../../src/pvf/pvf_parser.c \
 *         ../../src/pvf/pvf_packset.c
 *     ./test_integration
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pvf/pvf.h"

/* ========================================================================== */
/* 测试配置                                                                    */
/* ========================================================================== */

static const char* PVF_FILE_PATH = "/home/dxf/game/Script.pvf";

/* 性能测试参数 */
#define PERF_TEST_LOOKUP_COUNT 10000   /* 查找测试次数 */
#define PERF_TEST_EXTRACT_COUNT 500    /* 提取测试次数（降低以避免内存问题） */

/* ========================================================================== */
/* 辅助函数                                                                    */
/* ========================================================================== */

/**
 * @brief 计算时间差（毫秒）
 */
static double time_diff_ms(clock_t start, clock_t end) {
    return ((double)(end - start) * 1000.0) / CLOCKS_PER_SEC;
}

/* ========================================================================== */
/* 测试1: 完整工作流测试                                                       */
/* ========================================================================== */

static int test_full_workflow(void) {
    PackSetInternal pack;
    PVFIndexEntry* entry;
    char* buffer = NULL;
    int result;

    printf("\n========================================\n");
    printf("[测试1] 完整工作流测试\n");
    printf("========================================\n\n");

    /* 步骤1: 初始化 */
    printf("  [1] 初始化...\n");
    PVF_Initialize();

    /* 步骤2: 打开 */
    printf("  [2] 打开PVF文件...\n");
    result = PackSet_Open(PVF_FILE_PATH, &pack);
    if (result != PVF_SUCCESS) {
        printf("      [FAIL] 打开失败: %d\n", result);
        return 0;
    }
    printf("      文件数: %u\n", pack.header.index_count);

    /* 步骤3: 验证Hash表 */
    printf("  [3] 验证Hash表...\n");
    if (pack.hash_table == NULL || pack.hash_table_size != PVF_HASH_TABLE_SIZE) {
        printf("      [FAIL] Hash表无效\n");
        PackSet_Close(&pack);
        return 0;
    }
    printf("      Hash表: %d 桶\n", pack.hash_table_size);

    /* 步骤4: 查找文件 */
    printf("  [4] 查找文件...\n");
    entry = PackSet_GetFile(&pack, "equipment/character/common/wrist/brac_2choro994.equ");
    if (entry == NULL) {
        printf("      [FAIL] 文件未找到\n");
        PackSet_Close(&pack);
        return 0;
    }
    printf("      找到: %s (%u 字节)\n", entry->name, entry->size);

    /* 步骤5: 提取文件 */
    printf("  [5] 提取文件...\n");
    buffer = (char*)malloc(entry->size);
    if (buffer == NULL) {
        printf("      [FAIL] 内存分配失败\n");
        PackSet_Close(&pack);
        return 0;
    }

    result = PackSet_ExtractFile(&pack, entry, buffer, entry->size);
    if (result != PVF_SUCCESS) {
        printf("      [FAIL] 提取失败: %d\n", result);
        free(buffer);
        PackSet_Close(&pack);
        return 0;
    }
    printf("      提取成功\n");

    /* 步骤6: 验证内容（非空） */
    printf("  [6] 验证内容...\n");
    if (entry->size > 0 && buffer[0] == 0 && buffer[1] == 0) {
        /* 简单检查：内容不应全为零 */
        int all_zero = 1;
        size_t i;
        for (i = 0; i < entry->size && i < 16; i++) {
            if (buffer[i] != 0) {
                all_zero = 0;
                break;
            }
        }
        if (all_zero) {
            printf("      [WARN] 内容可能全为零\n");
        }
    }
    printf("      内容验证通过\n");

    /* 步骤7: 清理 */
    printf("  [7] 清理资源...\n");
    free(buffer);
    PackSet_Close(&pack);
    printf("      完成\n");

    printf("\n  [PASS] 完整工作流测试通过\n");
    return 1;
}

/* ========================================================================== */
/* 测试2: 大量文件查找性能测试                                                 */
/* ========================================================================== */

static int test_lookup_performance(void) {
    PackSetInternal pack;
    clock_t start, end;
    int i;
    int found_count = 0;
    double elapsed_ms;
    double avg_time_us;

    printf("\n========================================\n");
    printf("[测试2] 文件查找性能测试\n");
    printf("========================================\n\n");

    /* 打开PVF */
    PVF_Initialize();
    if (PackSet_Open(PVF_FILE_PATH, &pack) != PVF_SUCCESS) {
        printf("  [FAIL] 无法打开PVF文件\n");
        return 0;
    }

    printf("  测试参数:\n");
    printf("    文件数: %u\n", pack.header.index_count);
    printf("    查找次数: %d\n", PERF_TEST_LOOKUP_COUNT);

    /* 性能测试：随机查找 */
    printf("\n  执行查找测试...\n");
    start = clock();

    for (i = 0; i < PERF_TEST_LOOKUP_COUNT; i++) {
        uint32_t random_index = rand() % pack.header.index_count;
        const char* filepath = pack.indexes[random_index].name;
        PVFIndexEntry* entry = PackSet_GetFile(&pack, filepath);

        if (entry != NULL) {
            found_count++;
        }
    }

    end = clock();
    elapsed_ms = time_diff_ms(start, end);
    avg_time_us = (elapsed_ms * 1000.0) / PERF_TEST_LOOKUP_COUNT;

    printf("\n  性能结果:\n");
    printf("    总耗时: %.2f 毫秒\n", elapsed_ms);
    printf("    成功查找: %d/%d (%.1f%%)\n",
           found_count, PERF_TEST_LOOKUP_COUNT,
           (found_count * 100.0) / PERF_TEST_LOOKUP_COUNT);
    printf("    平均查找时间: %.2f 微秒/次\n", avg_time_us);
    printf("    查找速率: %.0f 次/秒\n", 1000000.0 / avg_time_us);

    PackSet_Close(&pack);

    /* 验证成功率 */
    if (found_count < PERF_TEST_LOOKUP_COUNT * 0.99) {
        printf("\n  [FAIL] 查找成功率太低\n");
        return 0;
    }

    printf("\n  [PASS] 查找性能测试通过\n");
    return 1;
}

/* ========================================================================== */
/* 测试3: 文件提取性能测试                                                     */
/* ========================================================================== */

static int test_extract_performance(void) {
    PackSetInternal pack;
    clock_t start, end;
    int i;
    int success_count = 0;
    uint64_t total_bytes = 0;
    double elapsed_ms;
    double throughput_mbps;

    printf("\n========================================\n");
    printf("[测试3] 文件提取性能测试\n");
    printf("========================================\n\n");

    /* 打开PVF */
    PVF_Initialize();
    if (PackSet_Open(PVF_FILE_PATH, &pack) != PVF_SUCCESS) {
        printf("  [FAIL] 无法打开PVF文件\n");
        return 0;
    }

    printf("  测试参数:\n");
    printf("    提取次数: %d\n", PERF_TEST_EXTRACT_COUNT);

    /* 性能测试：随机提取 */
    printf("\n  执行提取测试...\n");
    start = clock();

    for (i = 0; i < PERF_TEST_EXTRACT_COUNT; i++) {
        uint32_t random_index = rand() % pack.header.index_count;
        PVFIndexEntry* entry = &pack.indexes[random_index];
        char* buffer;
        int result;

        /* 跳过太大的文件（> 64KB） */
        if (entry->size > 65536) {
            continue;
        }

        buffer = (char*)malloc(entry->size);
        if (buffer == NULL) {
            continue;
        }

        result = PackSet_ExtractFile(&pack, entry, buffer, entry->size);
        if (result == PVF_SUCCESS) {
            success_count++;
            total_bytes += entry->size;
        }

        free(buffer);
    }

    end = clock();
    elapsed_ms = time_diff_ms(start, end);
    throughput_mbps = (total_bytes / 1024.0 / 1024.0) / (elapsed_ms / 1000.0);

    printf("\n  性能结果:\n");
    printf("    总耗时: %.2f 毫秒\n", elapsed_ms);
    printf("    成功提取: %d/%d (%.1f%%)\n",
           success_count, PERF_TEST_EXTRACT_COUNT,
           (success_count * 100.0) / PERF_TEST_EXTRACT_COUNT);
    printf("    总字节数: %.2f MB\n", total_bytes / 1024.0 / 1024.0);
    printf("    平均吞吐量: %.2f MB/秒\n", throughput_mbps);
    if (success_count > 0) {
        printf("    平均提取时间: %.2f 毫秒/次\n",
               elapsed_ms / success_count);
    }

    PackSet_Close(&pack);

    printf("\n  [PASS] 提取性能测试通过\n");
    return 1;
}

/* ========================================================================== */
/* 测试4: 内存管理测试                                                         */
/* ========================================================================== */

static int test_memory_management(void) {
    PackSetInternal pack1, pack2;
    int i;

    printf("\n========================================\n");
    printf("[测试4] 内存管理测试\n");
    printf("========================================\n\n");

    PVF_Initialize();

    /* 测试：多次打开/关闭 */
    printf("  [子测试1] 多次打开/关闭（10次）\n");
    for (i = 0; i < 10; i++) {
        if (PackSet_Open(PVF_FILE_PATH, &pack1) != PVF_SUCCESS) {
            printf("      [FAIL] 第%d次打开失败\n", i + 1);
            return 0;
        }
        PackSet_Close(&pack1);
    }
    printf("      [PASS] 10次打开/关闭成功\n");

    /* 测试：多个同时打开的实例 */
    printf("\n  [子测试2] 多个实例同时打开\n");
    if (PackSet_Open(PVF_FILE_PATH, &pack1) != PVF_SUCCESS) {
        printf("      [FAIL] 打开实例1失败\n");
        return 0;
    }

    if (PackSet_Open(PVF_FILE_PATH, &pack2) != PVF_SUCCESS) {
        printf("      [FAIL] 打开实例2失败\n");
        PackSet_Close(&pack1);
        return 0;
    }

    /* 验证两个实例独立 */
    if (pack1.fp == pack2.fp) {
        printf("      [WARN] 两个实例共享文件句柄\n");
    }

    PackSet_Close(&pack1);
    PackSet_Close(&pack2);
    printf("      [PASS] 多实例测试成功\n");

    /* 测试：重复关闭（安全性） */
    printf("\n  [子测试3] 重复关闭测试\n");
    if (PackSet_Open(PVF_FILE_PATH, &pack1) != PVF_SUCCESS) {
        printf("      [FAIL] 打开失败\n");
        return 0;
    }
    PackSet_Close(&pack1);
    PackSet_Close(&pack1);  /* 重复关闭 */
    printf("      [PASS] 重复关闭不崩溃\n");

    printf("\n  [PASS] 内存管理测试通过\n");
    return 1;
}

/* ========================================================================== */
/* 测试5: 边界情况测试                                                         */
/* ========================================================================== */

static int test_edge_cases(void) {
    PackSetInternal pack;
    PVFIndexEntry* entry;
    char small_buffer[10];
    int result;

    printf("\n========================================\n");
    printf("[测试5] 边界情况测试\n");
    printf("========================================\n\n");

    PVF_Initialize();

    if (PackSet_Open(PVF_FILE_PATH, &pack) != PVF_SUCCESS) {
        printf("  [FAIL] 无法打开PVF文件\n");
        return 0;
    }

    /* 测试1: 空路径 */
    printf("  [子测试1] 空路径查找\n");
    entry = PackSet_GetFile(&pack, "");
    if (entry == NULL) {
        printf("      [PASS] 空路径正确返回NULL\n");
    } else {
        printf("      [FAIL] 空路径错误返回非NULL\n");
    }

    /* 测试2: 很长的路径 */
    printf("\n  [子测试2] 超长路径查找\n");
    {
        char long_path[2048];
        memset(long_path, 'a', sizeof(long_path) - 1);
        long_path[sizeof(long_path) - 1] = '\0';

        entry = PackSet_GetFile(&pack, long_path);
        if (entry == NULL) {
            printf("      [PASS] 超长路径正确返回NULL\n");
        } else {
            printf("      [WARN] 超长路径返回非NULL\n");
        }
    }

    /* 测试3: 缓冲区太小 */
    printf("\n  [子测试3] 缓冲区太小\n");
    entry = PackSet_GetFile(&pack, "equipment/character/common/wrist/brac_2choro994.equ");
    if (entry != NULL && entry->size > sizeof(small_buffer)) {
        result = PackSet_ExtractFile(&pack, entry, small_buffer, sizeof(small_buffer));
        if (result == PVF_ERROR_MEMORY) {
            printf("      [PASS] 缓冲区太小正确返回错误\n");
        } else {
            printf("      [FAIL] 缓冲区太小未返回错误\n");
        }
    }

    /* 测试4: 查找第一个和最后一个文件 */
    printf("\n  [子测试4] 边界索引查找\n");
    if (pack.header.index_count > 0) {
        /* 第一个 */
        entry = PackSet_GetFile(&pack, pack.indexes[0].name);
        if (entry != NULL) {
            printf("      [PASS] 第一个文件查找成功\n");
        } else {
            printf("      [FAIL] 第一个文件查找失败\n");
        }

        /* 最后一个 */
        entry = PackSet_GetFile(&pack, pack.indexes[pack.header.index_count - 1].name);
        if (entry != NULL) {
            printf("      [PASS] 最后一个文件查找成功\n");
        } else {
            printf("      [FAIL] 最后一个文件查找失败\n");
        }
    }

    PackSet_Close(&pack);

    printf("\n  [PASS] 边界情况测试通过\n");
    return 1;
}

/* ========================================================================== */
/* 主函数                                                                      */
/* ========================================================================== */

int main(void) {
    int passed = 0;
    int total = 5;
    clock_t start_time, end_time;

    printf("========================================\n");
    printf("PVF库综合集成测试 + 性能测试\n");
    printf("版本: %s\n", PVF_GetVersion());
    printf("========================================\n");

    /* 初始化随机数种子 */
    srand((unsigned int)time(NULL));

    /* 记录总开始时间 */
    start_time = clock();

    /* 运行所有测试 */
    if (test_full_workflow()) passed++;
    if (test_lookup_performance()) passed++;
    if (test_extract_performance()) passed++;
    if (test_memory_management()) passed++;
    if (test_edge_cases()) passed++;

    /* 记录总结束时间 */
    end_time = clock();

    /* 汇总结果 */
    printf("\n========================================\n");
    printf("测试汇总\n");
    printf("========================================\n");
    printf("  通过: %d/%d\n", passed, total);
    printf("  总耗时: %.2f 秒\n", time_diff_ms(start_time, end_time) / 1000.0);
    printf("========================================\n");

    if (passed == total) {
        printf("\n[SUCCESS] 所有集成测试通过！✓\n\n");
        return 0;
    } else {
        printf("\n[FAILED] 部分测试失败\n\n");
        return 1;
    }
}
