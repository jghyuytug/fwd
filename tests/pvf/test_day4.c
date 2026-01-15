/**
 * @file test_day4.c
 * @brief Day 4综合测试 - 简化版（稳定）
 *
 * @details 测试所有核心功能，避免大量性能测试
 *
 * @usage
 *     gcc -m32 -I../../include -o test_day4 test_day4.c \
 *         ../../src/pvf/pvf_crc32.c \
 *         ../../src/pvf/pvf_decrypt.c \
 *         ../../src/pvf/pvf_hash.c \
 *         ../../src/pvf/pvf_parser.c \
 *         ../../src/pvf/pvf_packset.c
 *     ./test_day4
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pvf/pvf.h"

static const char* PVF_FILE_PATH = "/home/dxf/game/Script.pvf";

/* ========================================================================== */
/* 测试1: API版本和初始化                                                      */
/* ========================================================================== */

static int test_api_version(void) {
    const char* version;

    printf("\n[测试1] API版本和初始化\n");
    printf("=========================================\n");

    version = PVF_GetVersion();
    printf("  PVF库版本: %s\n", version);

    if (strcmp(version, "1.0.0") != 0) {
        printf("  [FAIL] 版本号不匹配\n");
        return 0;
    }

    PVF_Initialize();
    printf("  初始化成功\n");

    printf("  [PASS]\n");
    return 1;
}

/* ========================================================================== */
/* 测试2: 完整工作流                                                           */
/* ========================================================================== */

static int test_full_workflow(void) {
    PackSetInternal pack;
    PVFIndexEntry* entry;
    char* buffer;
    int result;

    printf("\n[测试2] 完整工作流\n");
    printf("=========================================\n");

    /* 打开 */
    result = PackSet_Open(PVF_FILE_PATH, &pack);
    if (result != PVF_SUCCESS) {
        printf("  [FAIL] 打开失败\n");
        return 0;
    }
    printf("  打开成功: %u 个文件\n", pack.header.index_count);

    /* 查找 */
    entry = PackSet_GetFile(&pack, "equipment/character/common/wrist/brac_2choro994.equ");
    if (entry == NULL) {
        printf("  [FAIL] 文件未找到\n");
        PackSet_Close(&pack);
        return 0;
    }
    printf("  查找成功: %s (%u 字节)\n", entry->name, entry->size);

    /* 提取 */
    buffer = (char*)malloc(entry->size);
    result = PackSet_ExtractFile(&pack, entry, buffer, entry->size);
    if (result != PVF_SUCCESS) {
        printf("  [FAIL] 提取失败\n");
        free(buffer);
        PackSet_Close(&pack);
        return 0;
    }
    printf("  提取成功\n");

    free(buffer);
    PackSet_Close(&pack);

    printf("  [PASS]\n");
    return 1;
}

/* ========================================================================== */
/* 测试3: 批量文件操作（少量）                                                 */
/* ========================================================================== */

static int test_batch_operations(void) {
    PackSetInternal pack;
    const char* files[] = {
        "creature/2wu_gunner/prime/animation/run/02_prime_run_01_dodge.ani",
        "equipment/character/common/wrist/brac_2choro994.equ",
        "passiveobject/character/gunner/animation/stingerex/subdodgenotarget.ani",
        "monster/newmonsters/shonan/kaizen/action/teleport.act",
        "equipment/character/mage/weapon/spear/beamspeard1/damage1.ani"
    };
    int count = 5;
    int i, success = 0;

    printf("\n[测试3] 批量文件操作（5个文件）\n");
    printf("=========================================\n");

    if (PackSet_Open(PVF_FILE_PATH, &pack) != PVF_SUCCESS) {
        printf("  [FAIL] 打开失败\n");
        return 0;
    }

    for (i = 0; i < count; i++) {
        PVFIndexEntry* entry = PackSet_GetFile(&pack, files[i]);
        if (entry != NULL) {
            char* buf = (char*)malloc(entry->size);
            if (buf && PackSet_ExtractFile(&pack, entry, buf, entry->size) == PVF_SUCCESS) {
                success++;
            }
            if (buf) free(buf);
        }
    }

    printf("  成功: %d/%d\n", success, count);

    PackSet_Close(&pack);

    if (success != count) {
        printf("  [FAIL] 未全部成功\n");
        return 0;
    }

    printf("  [PASS]\n");
    return 1;
}

/* ========================================================================== */
/* 测试4: 错误处理                                                             */
/* ========================================================================== */

static int test_error_handling(void) {
    PackSetInternal pack;
    PVFIndexEntry* entry;
    char small_buf[10];
    int result;

    printf("\n[测试4] 错误处理\n");
    printf("=========================================\n");

    /* 测试1: 打开不存在的文件 */
    result = PackSet_Open("/nonexistent.pvf", &pack);
    if (result != PVF_ERROR_FILE_NOT_FOUND) {
        printf("  [FAIL] 错误码不正确\n");
        return 0;
    }
    printf("  不存在的文件: 正确返回错误\n");

    /* 测试2: 查找不存在的文件 */
    if (PackSet_Open(PVF_FILE_PATH, &pack) != PVF_SUCCESS) {
        printf("  [FAIL] 打开失败\n");
        return 0;
    }

    entry = PackSet_GetFile(&pack, "nonexistent/file.txt");
    if (entry != NULL) {
        printf("  [FAIL] 不存在的文件返回非NULL\n");
        PackSet_Close(&pack);
        return 0;
    }
    printf("  不存在的文件: 正确返回NULL\n");

    /* 测试3: 缓冲区太小 */
    entry = PackSet_GetFile(&pack, "equipment/character/common/wrist/brac_2choro994.equ");
    if (entry != NULL && entry->size > sizeof(small_buf)) {
        result = PackSet_ExtractFile(&pack, entry, small_buf, sizeof(small_buf));
        if (result != PVF_ERROR_MEMORY) {
            printf("  [FAIL] 缓冲区太小未返回错误\n");
            PackSet_Close(&pack);
            return 0;
        }
        printf("  缓冲区太小: 正确返回错误\n");
    }

    PackSet_Close(&pack);

    printf("  [PASS]\n");
    return 1;
}

/* ========================================================================== */
/* 测试5: 内存管理                                                             */
/* ========================================================================== */

static int test_memory_management(void) {
    PackSetInternal pack;
    int i;

    printf("\n[测试5] 内存管理\n");
    printf("=========================================\n");

    /* 多次打开/关闭 */
    printf("  多次打开/关闭（5次）\n");
    for (i = 0; i < 5; i++) {
        if (PackSet_Open(PVF_FILE_PATH, &pack) != PVF_SUCCESS) {
            printf("  [FAIL] 第%d次打开失败\n", i + 1);
            return 0;
        }
        PackSet_Close(&pack);
    }
    printf("  完成\n");

    /* 重复关闭 */
    printf("  重复关闭测试\n");
    if (PackSet_Open(PVF_FILE_PATH, &pack) != PVF_SUCCESS) {
        printf("  [FAIL] 打开失败\n");
        return 0;
    }
    PackSet_Close(&pack);
    PackSet_Close(&pack);  /* 重复关闭不应崩溃 */
    printf("  完成\n");

    printf("  [PASS]\n");
    return 1;
}

/* ========================================================================== */
/* 测试6: Hash表统计                                                           */
/* ========================================================================== */

static int test_hash_statistics(void) {
    PackSetInternal pack;
    uint32_t i, empty = 0, total = 0;
    uint32_t expected_count;
    PVFHashNode* node;

    printf("\n[测试6] Hash表统计\n");
    printf("=========================================\n");

    if (PackSet_Open(PVF_FILE_PATH, &pack) != PVF_SUCCESS) {
        printf("  [FAIL] 打开失败\n");
        return 0;
    }

    expected_count = pack.header.index_count;

    for (i = 0; i < pack.hash_table_size; i++) {
        if (pack.hash_table[i] == NULL) {
            empty++;
        }
        node = pack.hash_table[i];
        while (node) {
            total++;
            node = node->next;
        }
    }

    printf("  总节点数: %u\n", total);
    printf("  期望: %u\n", expected_count);
    printf("  桶数: %d\n", pack.hash_table_size);
    printf("  空桶: %u (%.1f%%)\n", empty, (empty * 100.0) / pack.hash_table_size);
    printf("  平均链长: %.2f\n", (double)total / (pack.hash_table_size - empty));

    PackSet_Close(&pack);

    if (total != expected_count) {
        printf("  [FAIL] 节点数不匹配\n");
        return 0;
    }

    printf("  [PASS]\n");
    return 1;
}

/* ========================================================================== */
/* 测试7: 性能基准（小规模）                                                   */
/* ========================================================================== */

static int test_performance_basic(void) {
    PackSetInternal pack;
    clock_t start, end;
    int i, found = 0;

    printf("\n[测试7] 性能基准（100次查找）\n");
    printf("=========================================\n");

    if (PackSet_Open(PVF_FILE_PATH, &pack) != PVF_SUCCESS) {
        printf("  [FAIL] 打开失败\n");
        return 0;
    }

    start = clock();
    for (i = 0; i < 100; i++) {
        uint32_t idx = (i * 3797) % pack.header.index_count;
        if (PackSet_GetFile(&pack, pack.indexes[idx].name) != NULL) {
            found++;
        }
    }
    end = clock();

    printf("  查找: %d/100\n", found);
    printf("  耗时: %.2f 毫秒\n",
           ((double)(end - start) * 1000.0) / CLOCKS_PER_SEC);
    printf("  平均: %.2f 微秒/次\n",
           ((double)(end - start) * 1000000.0) / CLOCKS_PER_SEC / 100);

    PackSet_Close(&pack);

    printf("  [PASS]\n");
    return 1;
}

/* ========================================================================== */
/* 主函数                                                                      */
/* ========================================================================== */

int main(void) {
    int passed = 0;
    int total = 7;

    printf("========================================\n");
    printf("PVF库 Day 4 综合测试\n");
    printf("版本: %s\n", PVF_GetVersion());
    printf("========================================\n");

    if (test_api_version()) passed++;
    if (test_full_workflow()) passed++;
    if (test_batch_operations()) passed++;
    if (test_error_handling()) passed++;
    if (test_memory_management()) passed++;
    if (test_hash_statistics()) passed++;
    if (test_performance_basic()) passed++;

    printf("\n========================================\n");
    printf("测试结果: %d/%d 通过\n", passed, total);
    printf("========================================\n");

    if (passed == total) {
        printf("\n[SUCCESS] Day 4 测试全部通过！✓\n\n");
        return 0;
    } else {
        printf("\n[FAILED] 部分测试失败\n\n");
        return 1;
    }
}
