/**
 * @file test_day3.c
 * @brief Day 3完整测试程序 - PackSet功能测试
 *
 * @details 测试内容:
 *          1. PackSet_Open() - 打开PVF文件
 *          2. Hash表构建验证
 *          3. PackSet_GetFile() - 文件查找
 *          4. PackSet_ExtractFile() - 文件提取和解密
 *          5. PackSet_Close() - 资源清理
 *
 * @usage
 *     gcc -m32 -I../../include -o test_day3 test_day3.c \
 *         ../../src/pvf/pvf_crc32.c \
 *         ../../src/pvf/pvf_decrypt.c \
 *         ../../src/pvf/pvf_hash.c \
 *         ../../src/pvf/pvf_parser.c \
 *         ../../src/pvf/pvf_packset.c
 *     ./test_day3
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pvf/pvf_types.h"
#include "pvf/pvf_crc32.h"
#include "pvf/pvf_packset.h"

/* ========================================================================== */
/* 测试数据                                                                    */
/* ========================================================================== */

/**
 * @brief Script.pvf文件路径
 */
static const char* PVF_FILE_PATH = "/home/dxf/game/Script.pvf";

/**
 * @brief 测试文件列表（来自Day 2验证的前10个文件）
 */
static const char* TEST_FILES[] = {
    "creature/2wu_gunner/prime/animation/run/02_prime_run_01_dodge.ani",
    "equipment/character/common/wrist/brac_2choro994.equ",
    "passiveobject/character/gunner/animation/stingerex/subdodgenotarget.ani",
    "passiveobject/monster/ghoul/animation/skullbombsub.ani",
    "monster/newmonsters/shonan/kaizen/action/teleport.act",
    "passiveobject/actionobject/monster/lizard_man/animation/event_thunder_all6.ani",
    "equipment/character/mage/weapon/spear/beamspeard1/damage1.ani",
    "equipment/character/mage/avatar/face/chn_equ_avatar_mage_face_2687860.equ",
    "equipment/character/mage/avatar/hair/hair_a/rest.ani",
    "monster/newmonsters/dimensiongate/bakal/gold_dragon_shadow/particle/throws.ptl"
};

static const int TEST_FILE_COUNT = 10;

/**
 * @brief 期望的文件大小（来自Day 2验证）
 */
static const uint32_t EXPECTED_SIZES[] = {
    272, 317, 120, 563, 192, 420, 96, 412, 158, 112
};

/* ========================================================================== */
/* 测试函数                                                                    */
/* ========================================================================== */

/**
 * @brief 测试1: PackSet_Open() - 打开PVF文件
 */
static int test_packset_open(PackSetInternal* pack) {
    int result;

    printf("\n[Test 1] PackSet_Open() - 打开PVF文件\n");
    printf("=========================================\n");

    printf("  打开文件: %s\n", PVF_FILE_PATH);

    result = PackSet_Open(PVF_FILE_PATH, pack);

    if (result != PVF_SUCCESS) {
        printf("  [FAIL] PackSet_Open失败，错误码: %d\n", result);
        return 0;
    }

    printf("  [PASS] PackSet_Open成功\n");
    printf("  文件数量: %u\n", pack->header.index_count);
    printf("  Hash表大小: %d 桶\n", pack->hash_table_size);

    return 1;
}

/**
 * @brief 测试2: Hash表验证
 */
static int test_hash_table(PackSetInternal* pack) {
    uint32_t i;
    uint32_t total_nodes = 0;
    uint32_t empty_buckets = 0;
    uint32_t max_chain_length = 0;
    PVFHashNode* node;

    printf("\n[Test 2] Hash表验证\n");
    printf("=========================================\n");

    /* 统计Hash表信息 */
    for (i = 0; i < pack->hash_table_size; i++) {
        uint32_t chain_length = 0;

        node = pack->hash_table[i];

        if (node == NULL) {
            empty_buckets++;
        }

        while (node != NULL) {
            total_nodes++;
            chain_length++;
            node = node->next;
        }

        if (chain_length > max_chain_length) {
            max_chain_length = chain_length;
        }
    }

    printf("  总节点数: %u\n", total_nodes);
    printf("  期望节点数: %u\n", pack->header.index_count);
    printf("  空桶数量: %u / %d (%.1f%%)\n",
           empty_buckets, pack->hash_table_size,
           (empty_buckets * 100.0) / pack->hash_table_size);
    printf("  最长链: %u\n", max_chain_length);
    printf("  平均链长: %.2f\n",
           (double)total_nodes / (pack->hash_table_size - empty_buckets));

    /* 验证节点总数 */
    if (total_nodes != pack->header.index_count) {
        printf("  [FAIL] Hash表节点数量不匹配\n");
        return 0;
    }

    printf("  [PASS] Hash表构建正确\n");
    return 1;
}

/**
 * @brief 测试3: PackSet_GetFile() - 文件查找
 */
static int test_get_file(PackSetInternal* pack) {
    int i;
    PVFIndexEntry* entry;
    int found_count = 0;

    printf("\n[Test 3] PackSet_GetFile() - 文件查找\n");
    printf("=========================================\n");

    for (i = 0; i < TEST_FILE_COUNT; i++) {
        entry = PackSet_GetFile(pack, TEST_FILES[i]);

        if (entry == NULL) {
            printf("  [FAIL] 未找到文件: %s\n", TEST_FILES[i]);
            continue;
        }

        /* 验证文件大小 */
        if (entry->size != EXPECTED_SIZES[i]) {
            printf("  [FAIL] 文件大小不匹配: %s\n", TEST_FILES[i]);
            printf("        期望: %u, 实际: %u\n", EXPECTED_SIZES[i], entry->size);
            continue;
        }

        found_count++;
        printf("  [%d] 找到: %s (大小: %u 字节)\n",
               i + 1, TEST_FILES[i], entry->size);
    }

    printf("\n  查找成功: %d/%d\n", found_count, TEST_FILE_COUNT);

    if (found_count != TEST_FILE_COUNT) {
        printf("  [FAIL] 部分文件未找到\n");
        return 0;
    }

    printf("  [PASS] 所有文件查找成功\n");
    return 1;
}

/**
 * @brief 测试4: PackSet_ExtractFile() - 文件提取
 */
static int test_extract_file(PackSetInternal* pack) {
    PVFIndexEntry* entry;
    char* buffer = NULL;
    int result;

    printf("\n[Test 4] PackSet_ExtractFile() - 文件提取\n");
    printf("=========================================\n");

    /* 选择第一个测试文件 */
    entry = PackSet_GetFile(pack, TEST_FILES[0]);

    if (entry == NULL) {
        printf("  [FAIL] 测试文件未找到\n");
        return 0;
    }

    printf("  提取文件: %s\n", TEST_FILES[0]);
    printf("  文件大小: %u 字节\n", entry->size);

    /* 分配缓冲区 */
    buffer = (char*)malloc(entry->size);
    if (buffer == NULL) {
        printf("  [FAIL] 内存分配失败\n");
        return 0;
    }

    /* 提取文件 */
    result = PackSet_ExtractFile(pack, entry, buffer, entry->size);

    if (result != PVF_SUCCESS) {
        printf("  [FAIL] 文件提取失败，错误码: %d\n", result);
        free(buffer);
        return 0;
    }

    printf("  [PASS] 文件提取成功\n");
    printf("  前16字节 (hex): ");
    {
        int i;
        int display_size = (entry->size < 16) ? entry->size : 16;
        for (i = 0; i < display_size; i++) {
            printf("%02x ", (unsigned char)buffer[i]);
        }
        printf("\n");
    }

    free(buffer);
    return 1;
}

/**
 * @brief 测试5: PackSet_ExtractFileByPath() - 组合查找+提取
 */
static int test_extract_by_path(PackSetInternal* pack) {
    char buffer[65536];
    uint32_t actual_size;
    int result;

    printf("\n[Test 5] PackSet_ExtractFileByPath() - 组合提取\n");
    printf("=========================================\n");

    printf("  提取文件: %s\n", TEST_FILES[1]);

    result = PackSet_ExtractFileByPath(
        pack,
        TEST_FILES[1],
        buffer,
        sizeof(buffer),
        &actual_size
    );

    if (result != PVF_SUCCESS) {
        printf("  [FAIL] 文件提取失败，错误码: %d\n", result);
        return 0;
    }

    printf("  [PASS] 文件提取成功\n");
    printf("  文件大小: %u 字节\n", actual_size);
    printf("  期望大小: %u 字节\n", EXPECTED_SIZES[1]);

    /* 验证大小 */
    if (actual_size != EXPECTED_SIZES[1]) {
        printf("  [FAIL] 文件大小不匹配\n");
        return 0;
    }

    printf("  [PASS] 文件大小匹配\n");
    return 1;
}

/**
 * @brief 测试6: 查找不存在的文件
 */
static int test_file_not_found(PackSetInternal* pack) {
    PVFIndexEntry* entry;

    printf("\n[Test 6] 查找不存在的文件\n");
    printf("=========================================\n");

    entry = PackSet_GetFile(pack, "nonexistent/file.txt");

    if (entry != NULL) {
        printf("  [FAIL] 错误：找到了不存在的文件\n");
        return 0;
    }

    printf("  [PASS] 正确返回NULL（文件不存在）\n");
    return 1;
}

/* ========================================================================== */
/* 主函数                                                                      */
/* ========================================================================== */

int main(void) {
    PackSetInternal pack;
    int passed = 0;
    int total = 6;

    printf("========================================\n");
    printf("PVF Day 3 完整测试 - PackSet\n");
    printf("========================================\n");

    /* 初始化CRC32表 */
    PVF_InitCRC32Table();

    /* 测试1: 打开PVF */
    if (!test_packset_open(&pack)) {
        printf("\n[FATAL] 无法打开PVF文件，测试中止\n");
        return 1;
    }
    passed++;

    /* 运行其他测试 */
    if (test_hash_table(&pack)) passed++;
    if (test_get_file(&pack)) passed++;
    if (test_extract_file(&pack)) passed++;
    if (test_extract_by_path(&pack)) passed++;
    if (test_file_not_found(&pack)) passed++;

    /* 关闭PackSet */
    printf("\n[Info] 关闭PackSet...\n");
    PackSet_Close(&pack);
    printf("[Info] PackSet已关闭\n");

    /* 汇总结果 */
    printf("\n========================================\n");
    printf("测试结果: %d/%d 通过\n", passed, total);
    printf("========================================\n");

    if (passed == total) {
        printf("\n[SUCCESS] Day 3 测试全部通过！✓\n\n");
        return 0;
    } else {
        printf("\n[FAILED] 部分测试失败\n\n");
        return 1;
    }
}
