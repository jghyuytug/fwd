/**
 * @file test_data_parser.c
 * @brief PVF数据解析器测试
 *
 * @usage
 *     gcc -m32 -I../../include -o test_data_parser test_data_parser.c \
 *         ../../src/pvf/pvf_crc32.c \
 *         ../../src/pvf/pvf_decrypt.c \
 *         ../../src/pvf/pvf_hash.c \
 *         ../../src/pvf/pvf_parser.c \
 *         ../../src/pvf/pvf_packset.c \
 *         ../../src/pvf/pvf_data_parser.c
 *     ./test_data_parser
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pvf/pvf.h"
#include "pvf/pvf_data_parser.h"

static const char* PVF_FILE_PATH = "/home/dxf/game/Script.pvf";

/* ========================================================================== */
/* 测试1: 迭代器基本功能                                                      */
/* ========================================================================== */

static int test_iterator_basic(void) {
    /* 模拟数据：魔数 + 两个键值对 */
    unsigned char test_data[] = {
        0xB0, 0xD0,                    /* 魔数 */
        0x05, 0x01, 0x00, 0x00, 0x00,  /* 键=1 */
        0x02, 0x0A, 0x00, 0x00, 0x00,  /* 整数值=10 */
        0x05, 0x02, 0x00, 0x00, 0x00,  /* 键=2 */
        0x07, 0x14, 0x00, 0x00, 0x00   /* 引用值=20 */
    };

    PVFDataIterator iter;
    PVFDataEntry entry;
    int result;
    int count = 0;

    printf("\n[测试1] 迭代器基本功能\n");
    printf("=========================================\n");

    /* 初始化 */
    result = PVFDataIterator_Init(&iter, test_data, sizeof(test_data));
    if (result != PVF_SUCCESS) {
        printf("  [FAIL] 初始化失败\n");
        return 0;
    }

    printf("  初始化成功, 魔数: %s\n", iter.has_magic ? "是" : "否");
    printf("  起始位置: %zu\n\n", iter.position);

    /* 迭代 */
    while (PVFDataIterator_HasNext(&iter)) {
        result = PVFDataIterator_Next(&iter, &entry);
        if (result != PVF_SUCCESS) {
            printf("  [FAIL] 读取条目失败\n");
            return 0;
        }

        count++;
        printf("  条目#%d: 键=0x%08x, 类型=%s, ",
               count, entry.key, PVFData_TypeName(entry.type));

        if (entry.type == PVF_VALUE_TYPE_INTEGER) {
            printf("值=%d\n", entry.value.int_value);
        } else if (entry.type == PVF_VALUE_TYPE_REFERENCE) {
            printf("引用=0x%08x\n", entry.value.ref_value);
        } else {
            printf("\n");
        }
    }

    if (count != 2) {
        printf("  [FAIL] 条目数不正确: %d (期望2)\n", count);
        return 0;
    }

    printf("\n  [PASS]\n");
    return 1;
}

/* ========================================================================== */
/* 测试2: 容器功能                                                            */
/* ========================================================================== */

static int test_container(void) {
    unsigned char test_data[] = {
        0xB0, 0xD0,
        0x05, 0x10, 0x00, 0x00, 0x00, 0x02, 0x64, 0x00, 0x00, 0x00,  /* 键=16, 值=100 */
        0x05, 0x20, 0x00, 0x00, 0x00, 0x02, 0xC8, 0x00, 0x00, 0x00   /* 键=32, 值=200 */
    };

    PVFDataContainer container = {0};
    int result;

    printf("\n[测试2] 容器功能\n");
    printf("=========================================\n");

    /* 解析 */
    result = PVFDataContainer_Parse(&container, test_data, sizeof(test_data));
    if (result != PVF_SUCCESS) {
        printf("  [FAIL] 解析失败\n");
        return 0;
    }

    printf("  解析成功: %d 个条目\n", container.count);

    /* 查找 */
    int32_t val1 = PVFDataContainer_GetInt(&container, 0x10, -1);
    int32_t val2 = PVFDataContainer_GetInt(&container, 0x20, -1);
    int32_t val3 = PVFDataContainer_GetInt(&container, 0x99, -999);

    printf("  键0x10的值: %d (期望100)\n", val1);
    printf("  键0x20的值: %d (期望200)\n", val2);
    printf("  键0x99的值: %d (期望-999, 不存在)\n", val3);

    PVFDataContainer_Free(&container);

    if (val1 != 100 || val2 != 200 || val3 != -999) {
        printf("  [FAIL] 值不正确\n");
        return 0;
    }

    printf("  [PASS]\n");
    return 1;
}

/* ========================================================================== */
/* 测试3: 真实PVF文件解析                                                     */
/* ========================================================================== */

static int test_real_pvf_file(void) {
    PackSetInternal pack;
    PVFIndexEntry* entry;
    unsigned char* buffer;
    PVFDataContainer container = {0};
    int result;

    printf("\n[测试3] 真实PVF文件解析\n");
    printf("=========================================\n");

    /* 打开PVF */
    PVF_Initialize();
    result = PackSet_Open(PVF_FILE_PATH, &pack);
    if (result != PVF_SUCCESS) {
        printf("  [SKIP] PVF文件不存在\n");
        return 1;  /* 不算失败 */
    }

    /* 提取装备文件 */
    entry = PackSet_GetFile(&pack, "equipment/character/common/wrist/brac_2choro994.equ");
    if (entry == NULL) {
        printf("  [SKIP] 测试文件未找到\n");
        PackSet_Close(&pack);
        return 1;
    }

    printf("  文件: brac_2choro994.equ\n");
    printf("  大小: %u 字节\n", entry->size);

    buffer = (unsigned char*)malloc(entry->size);
    result = PackSet_ExtractFile(&pack, entry, buffer, entry->size);
    if (result != PVF_SUCCESS) {
        printf("  [FAIL] 提取失败\n");
        free(buffer);
        PackSet_Close(&pack);
        return 0;
    }

    /* 解析 */
    result = PVFDataContainer_Parse(&container, buffer, entry->size);
    if (result != PVF_SUCCESS) {
        printf("  [FAIL] 解析失败\n");
        free(buffer);
        PackSet_Close(&pack);
        return 0;
    }

    printf("  解析成功: %d 个键值对\n", container.count);

    /* 显示部分数据 */
    printf("\n  前10个键值对:\n");
    int i;
    for (i = 0; i < 10 && i < container.count; i++) {
        PVFDataEntry* e = &container.entries[i];
        printf("    [%d] 键=0x%08x, 类型=%s, ",
               i+1, e->key, PVFData_TypeName(e->type));

        if (e->type == PVF_VALUE_TYPE_INTEGER) {
            printf("值=%d\n", e->value.int_value);
        } else if (e->type == PVF_VALUE_TYPE_REFERENCE) {
            printf("引用=0x%08x\n", e->value.ref_value);
        } else {
            printf("\n");
        }
    }

    /* 清理 */
    PVFDataContainer_Free(&container);
    free(buffer);
    PackSet_Close(&pack);

    printf("\n  [PASS]\n");
    return 1;
}

/* ========================================================================== */
/* 测试4: 辅助函数                                                            */
/* ========================================================================== */

static int test_utility_functions(void) {
    unsigned char test_data[] = {0xB0, 0xD0, 0x05, 0x01, 0x00, 0x00, 0x00};

    printf("\n[测试4] 辅助函数\n");
    printf("=========================================\n");

    /* 魔数检测 */
    int has_magic = PVFData_HasMagic(test_data, sizeof(test_data));
    printf("  魔数检测: %s (期望:是)\n", has_magic ? "是" : "否");

    /* 估算数量 */
    int estimated = PVFData_EstimateCount(test_data, sizeof(test_data));
    printf("  估算数量: %d\n", estimated);

    /* 验证 */
    int valid = PVFData_Validate(test_data, sizeof(test_data));
    printf("  数据验证: %s\n", valid ? "有效" : "无效");

    if (!has_magic || !valid) {
        printf("  [FAIL]\n");
        return 0;
    }

    printf("  [PASS]\n");
    return 1;
}

/* ========================================================================== */
/* 主函数                                                                      */
/* ========================================================================== */

int main(int argc, char* argv[]) {
    int passed = 0;
    int total = 4;

    /* 允许自定义PVF路径 */
    if (argc >= 2) {
        PVF_FILE_PATH = argv[1];
    }

    printf("========================================\n");
    printf("PVF数据解析器测试\n");
    printf("版本: %s\n", PVF_GetVersion());
    printf("========================================\n");

    if (test_iterator_basic()) passed++;
    if (test_container()) passed++;
    if (test_real_pvf_file()) passed++;
    if (test_utility_functions()) passed++;

    printf("\n========================================\n");
    printf("测试结果: %d/%d 通过\n", passed, total);
    printf("========================================\n\n");

    return (passed == total) ? 0 : 1;
}
