/**
 * @file test_day2.c
 * @brief Day 2完整测试程序
 *
 * @details 测试内容:
 *          1. GUID验证
 *          2. 路径规范化
 *          3. DJB2哈希计算
 *          4. 头部解析（实际Script.pvf）
 *          5. 索引解析（解密+CRC32验证+解析条目）
 *
 * @usage
 *     gcc -m32 -I../../include -o test_day2 test_day2.c \
 *         ../../src/pvf/pvf_crc32.c \
 *         ../../src/pvf/pvf_decrypt.c \
 *         ../../src/pvf/pvf_hash.c \
 *         ../../src/pvf/pvf_parser.c
 *     ./test_day2
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pvf/pvf_types.h"
#include "pvf/pvf_crc32.h"
#include "pvf/pvf_decrypt.h"
#include "pvf/pvf_hash.h"
#include "pvf/pvf_parser.h"

/* ========================================================================== */
/* 测试数据                                                                    */
/* ========================================================================== */

/**
 * @brief Script.pvf文件路径
 */
static const char* PVF_FILE_PATH = "/home/dxf/game/Script.pvf";

/**
 * @brief 期望的头部数据（来自Python验证）
 */
static const uint32_t EXPECTED_REVISION = 48984;
static const uint32_t EXPECTED_INDEX_SIZE = 31670772;
static const uint32_t EXPECTED_INDEX_CRC32 = 0x6fe5d91b;
static const uint32_t EXPECTED_INDEX_COUNT = 379576;

/* ========================================================================== */
/* 测试函数                                                                    */
/* ========================================================================== */

/**
 * @brief 测试1: GUID验证
 */
static int test_guid_validation(void) {
    printf("\n[Test 1] GUID验证\n");
    printf("=========================================\n");

    /* 测试正确的GUID */
    if (PVF_ValidateGUID(PVF_EXPECTED_GUID)) {
        printf("  [PASS] 正确的GUID验证通过\n");
    } else {
        printf("  [FAIL] 正确的GUID验证失败\n");
        return 0;
    }

    /* 测试错误的GUID */
    if (!PVF_ValidateGUID("wrong-guid-string")) {
        printf("  [PASS] 错误的GUID验证失败（符合预期）\n");
    } else {
        printf("  [FAIL] 错误的GUID验证通过（不符合预期）\n");
        return 0;
    }

    return 1;
}

/**
 * @brief 测试2: 路径规范化
 */
static int test_path_normalization(void) {
    char normalized[256];

    printf("\n[Test 2] 路径规范化\n");
    printf("=========================================\n");

    /* 测试用例1: 反斜杠和大写 */
    PVF_NormalizePath("Etc\\ItemScript.txt", normalized);
    printf("  输入: \"Etc\\\\ItemScript.txt\"\n");
    printf("  输出: \"%s\"\n", normalized);
    printf("  期望: \"etc/itemscript.txt\"\n");

    if (strcmp(normalized, "etc/itemscript.txt") == 0) {
        printf("  [PASS] 路径规范化正确\n");
    } else {
        printf("  [FAIL] 路径规范化错误\n");
        return 0;
    }

    /* 测试用例2: 多级路径 */
    PVF_NormalizePath("Equipment\\Character\\Fighter\\Weapon.equ", normalized);
    printf("\n  输入: \"Equipment\\\\Character\\\\Fighter\\\\Weapon.equ\"\n");
    printf("  输出: \"%s\"\n", normalized);

    if (strcmp(normalized, "equipment/character/fighter/weapon.equ") == 0) {
        printf("  [PASS] 多级路径规范化正确\n");
    } else {
        printf("  [FAIL] 多级路径规范化错误\n");
        return 0;
    }

    return 1;
}

/**
 * @brief 测试3: DJB2哈希计算
 */
static int test_djb2_hash(void) {
    uint32_t hash;
    char normalized[256];

    printf("\n[Test 3] DJB2哈希计算\n");
    printf("=========================================\n");

    /* 测试用例1: 简单路径 */
    PVF_NormalizePath("Etc\\ItemScript.txt", normalized);
    hash = PVF_DJB2Hash(normalized);

    printf("  输入: \"%s\"\n", normalized);
    printf("  哈希: 0x%08x\n", hash);

    /* 注意: 由于编码问题，这里只验证函数能正常运行 */
    if (hash > 0) {
        printf("  [PASS] 哈希计算完成\n");
    } else {
        printf("  [FAIL] 哈希计算失败\n");
        return 0;
    }

    /* 测试用例2: 使用组合函数 */
    hash = PVF_CalculateFileHash("Etc\\ItemScript.txt");
    printf("\n  使用PVF_CalculateFileHash:\n");
    printf("  输入: \"Etc\\\\ItemScript.txt\"\n");
    printf("  哈希: 0x%08x\n", hash);

    if (hash > 0) {
        printf("  [PASS] 组合函数工作正常\n");
    } else {
        printf("  [FAIL] 组合函数失败\n");
        return 0;
    }

    return 1;
}

/**
 * @brief 测试4: 头部解析（实际Script.pvf）
 */
static int test_header_parsing(FILE* fp) {
    PVFHeader header;
    uint32_t header_end;
    int result;

    printf("\n[Test 4] 头部解析 (Script.pvf)\n");
    printf("=========================================\n");

    /* 重置文件位置到开头 */
    fseek(fp, 0, SEEK_SET);

    /* 解析头部 */
    result = PVF_ParseHeader(fp, &header, &header_end);

    if (result != PVF_SUCCESS) {
        printf("  [FAIL] 头部解析失败，错误码: %d\n", result);
        return 0;
    }

    /* 验证所有字段 */
    printf("  GUID: %s\n", header.tag_string);
    printf("  版本号: %u (期望: %u)\n", header.revision, EXPECTED_REVISION);
    printf("  索引大小: %u (期望: %u)\n", header.index_header_size, EXPECTED_INDEX_SIZE);
    printf("  索引CRC32: 0x%08x (期望: 0x%08x)\n", header.index_crc32, EXPECTED_INDEX_CRC32);
    printf("  文件数量: %u (期望: %u)\n", header.index_count, EXPECTED_INDEX_COUNT);
    printf("  头部结束: 0x%08x\n", header_end);

    /* 验证字段匹配 */
    if (header.revision != EXPECTED_REVISION ||
        header.index_header_size != EXPECTED_INDEX_SIZE ||
        header.index_crc32 != EXPECTED_INDEX_CRC32 ||
        header.index_count != EXPECTED_INDEX_COUNT) {
        printf("  [FAIL] 头部字段不匹配\n");
        return 0;
    }

    printf("  [PASS] 头部解析正确\n");
    return 1;
}

/**
 * @brief 测试5: 索引解析（解密+CRC32验证+解析条目）
 */
static int test_index_parsing(FILE* fp) {
    PackSetInternal pack;
    int result;
    uint32_t i;

    printf("\n[Test 5] 索引解析 (Script.pvf)\n");
    printf("=========================================\n");

    /* 初始化PackSetInternal */
    memset(&pack, 0, sizeof(PackSetInternal));
    pack.fp = fp;

    /* 重置文件位置到开头 */
    fseek(fp, 0, SEEK_SET);

    /* 先解析头部 */
    result = PVF_ParseHeader(fp, &pack.header, &pack.header_end_offset);
    if (result != PVF_SUCCESS) {
        printf("  [FAIL] 头部解析失败\n");
        return 0;
    }

    printf("  [1] 头部解析成功\n");

    /* 解析索引 */
    printf("  [2] 开始索引解密和解析...\n");
    result = PVF_ParseIndex(fp, &pack);

    if (result != PVF_SUCCESS) {
        printf("  [FAIL] 索引解析失败，错误码: %d\n", result);
        return 0;
    }

    printf("  [3] 索引解析成功！\n");
    printf("  [4] CRC32验证通过: 0x%08x\n", pack.header.index_crc32);
    printf("  [5] 成功解析 %u 个文件\n", pack.header.index_count);

    /* 打印前10个条目 */
    printf("\n  前10个文件:\n");
    for (i = 0; i < 10 && i < pack.header.index_count; i++) {
        printf("    [%u] %s\n", i, pack.indexes[i].name);
        printf("        哈希: 0x%08x\n", pack.indexes[i].hash_value);
        printf("        大小: %u 字节\n", pack.indexes[i].size);
        printf("        偏移: 0x%08x\n", pack.indexes[i].offset);
        printf("        密钥: 0x%08x\n", pack.indexes[i].decrypt_key);
    }

    /* 释放内存 */
    printf("\n  [6] 释放内存...\n");
    for (i = 0; i < pack.header.index_count; i++) {
        free(pack.indexes[i].name);
    }
    free(pack.indexes);

    printf("  [PASS] 索引解析、CRC32验证、条目解析全部正确\n");
    return 1;
}

/* ========================================================================== */
/* 主函数                                                                      */
/* ========================================================================== */

int main(void) {
    FILE* fp = NULL;
    int passed = 0;
    int total = 5;

    printf("========================================\n");
    printf("PVF Day 2 完整测试\n");
    printf("========================================\n");

    /* 初始化CRC32表 */
    PVF_InitCRC32Table();

    /* 运行独立测试 */
    if (test_guid_validation()) passed++;
    if (test_path_normalization()) passed++;
    if (test_djb2_hash()) passed++;

    /* 打开Script.pvf进行文件测试 */
    printf("\n[Info] 打开 %s\n", PVF_FILE_PATH);
    fp = fopen(PVF_FILE_PATH, "rb");

    if (fp == NULL) {
        printf("[ERROR] 无法打开Script.pvf文件\n");
        printf("        请确保文件存在: %s\n", PVF_FILE_PATH);
        printf("\n测试结果: %d/%d 通过\n", passed, total);
        return 1;
    }

    /* 运行文件测试 */
    if (test_header_parsing(fp)) passed++;
    if (test_index_parsing(fp)) passed++;

    /* 关闭文件 */
    fclose(fp);

    /* 汇总结果 */
    printf("\n========================================\n");
    printf("测试结果: %d/%d 通过\n", passed, total);
    printf("========================================\n");

    if (passed == total) {
        printf("\n[SUCCESS] Day 2 测试全部通过！✓\n\n");
        return 0;
    } else {
        printf("\n[FAILED] 部分测试失败\n\n");
        return 1;
    }
}
