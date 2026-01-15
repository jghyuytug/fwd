/**
 * @file test_day1.c
 * @brief Day 1模块测试程序
 *
 * @details 测试内容:
 *          1. CRC32算法验证
 *          2. ROR32算法验证
 *          3. 基础数据结构大小验证
 *
 * @usage
 *     gcc -m32 -I../../include -o test_day1 test_day1.c \
 *         ../../src/pvf/pvf_crc32.c ../../src/pvf/pvf_decrypt.c
 *     ./test_day1
 */

#include <stdio.h>
#include <string.h>
#include "pvf/pvf_types.h"
#include "pvf/pvf_crc32.h"
#include "pvf/pvf_decrypt.h"

/* ========================================================================== */
/* 测试数据                                                                    */
/* ========================================================================== */

/**
 * @brief 测试用的简单数据
 */
static const char* test_string = "Hello, PVF!";

/**
 * @brief 已知CRC32值（使用Python验证）
 * Python 2.4: zlib.crc32("Hello, PVF!") = 0x88f6320e
 */
static const uint32_t expected_crc32 = 0x88f6320e;

/* ========================================================================== */
/* 测试函数                                                                    */
/* ========================================================================== */

/**
 * @brief 测试1: CRC32查找表生成
 */
static int test_crc32_table_init(void) {
    printf("\n[Test 1] CRC32查找表初始化\n");
    printf("=========================================\n");

    PVF_InitCRC32Table();

    /* 验证前4个条目（已知值） */
    printf("  g_crc32_table[0] = 0x%08x (期望: 0x00000000)\n", g_crc32_table[0]);
    printf("  g_crc32_table[1] = 0x%08x (期望: 0x77073096)\n", g_crc32_table[1]);
    printf("  g_crc32_table[2] = 0x%08x (期望: 0xee0e612c)\n", g_crc32_table[2]);
    printf("  g_crc32_table[3] = 0x%08x (期望: 0x990951ba)\n", g_crc32_table[3]);

    /* 验证已知值 */
    if (g_crc32_table[0] == 0x00000000 &&
        g_crc32_table[1] == 0x77073096 &&
        g_crc32_table[2] == 0xee0e612c &&
        g_crc32_table[3] == 0x990951ba) {
        printf("  [PASS] CRC32表初始化正确\n");
        return 1;
    } else {
        printf("  [FAIL] CRC32表初始化错误\n");
        return 0;
    }
}

/**
 * @brief 测试2: CRC32计算
 */
static int test_crc32_calculation(void) {
    uint32_t calculated;

    printf("\n[Test 2] CRC32计算\n");
    printf("=========================================\n");

    printf("  输入: \"%s\"\n", test_string);

    calculated = PVF_CalcCRC32(test_string, strlen(test_string));

    printf("  计算CRC32: 0x%08x\n", calculated);
    printf("  期望CRC32: 0x%08x\n", expected_crc32);

    if (calculated == expected_crc32) {
        printf("  [PASS] CRC32计算正确\n");
        return 1;
    } else {
        printf("  [FAIL] CRC32计算错误\n");
        return 0;
    }
}

/**
 * @brief 测试3: ROR32右旋转
 */
static int test_ror32(void) {
    uint32_t value = 0x12345678;
    uint32_t result;
    uint32_t expected = 0xe048d159;  /* 已知: ROR32(0x12345678, 6) */

    printf("\n[Test 3] ROR32右旋转\n");
    printf("=========================================\n");

    printf("  输入: 0x%08x\n", value);
    printf("  旋转位数: 6\n");

    result = PVF_ROR32(value, 6);

    printf("  计算结果: 0x%08x\n", result);
    printf("  期望结果: 0x%08x\n", expected);

    if (result == expected) {
        printf("  [PASS] ROR32计算正确\n");
        return 1;
    } else {
        printf("  [FAIL] ROR32计算错误\n");
        return 0;
    }
}

/**
 * @brief 测试4: 数据结构大小
 */
static int test_structure_sizes(void) {
    printf("\n[Test 4] 数据结构大小\n");
    printf("=========================================\n");

    printf("  sizeof(PVFHeader) = %lu 字节\n",
           (unsigned long)sizeof(PVFHeader));
    printf("  sizeof(PVFIndexEntry) = %lu 字节\n",
           (unsigned long)sizeof(PVFIndexEntry));
    printf("  sizeof(PVFHashNode) = %lu 字节\n",
           (unsigned long)sizeof(PVFHashNode));
    printf("  sizeof(PackSetInternal) = %lu 字节\n",
           (unsigned long)sizeof(PackSetInternal));

    /* 基本检查：结构体大小合理 */
    if (sizeof(PVFHeader) > 0 &&
        sizeof(PVFIndexEntry) > 0 &&
        sizeof(PackSetInternal) > 0) {
        printf("  [PASS] 所有结构体大小合理\n");
        return 1;
    } else {
        printf("  [FAIL] 结构体大小异常\n");
        return 0;
    }
}

/**
 * @brief 测试5: 解密算法常量验证
 */
static int test_decrypt_constants(void) {
    printf("\n[Test 5] 解密算法常量\n");
    printf("=========================================\n");

    printf("  G_ADDITIONAL_KEY = 0x%08x\n", G_ADDITIONAL_KEY);
    printf("  XOR_KEY = 0x%08x\n", XOR_KEY);

    /* 验证关键常量 */
    if (G_ADDITIONAL_KEY == 0x81A79011 && XOR_KEY == 0) {
        printf("  [PASS] 解密常量正确\n");
        return 1;
    } else {
        printf("  [FAIL] 解密常量错误\n");
        return 0;
    }
}

/* ========================================================================== */
/* 主函数                                                                      */
/* ========================================================================== */

int main(void) {
    int passed = 0;
    int total = 5;

    printf("========================================\n");
    printf("PVF Day 1 模块测试\n");
    printf("========================================\n");

    /* 运行所有测试 */
    if (test_crc32_table_init()) passed++;
    if (test_crc32_calculation()) passed++;
    if (test_ror32()) passed++;
    if (test_structure_sizes()) passed++;
    if (test_decrypt_constants()) passed++;

    /* 汇总结果 */
    printf("\n========================================\n");
    printf("测试结果: %d/%d 通过\n", passed, total);
    printf("========================================\n");

    if (passed == total) {
        printf("\n[SUCCESS] Day 1 模块测试全部通过！✓\n\n");
        return 0;
    } else {
        printf("\n[FAILED] 部分测试失败\n\n");
        return 1;
    }
}
