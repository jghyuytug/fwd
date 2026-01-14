/**
 * @file pvf_hash.c
 * @brief PVF文件路径哈希算法实现 (DJB2变体)
 *
 * @details 实现DNF Script.pvf使用的文件名哈希算法
 *          基于逆向工程结果 (2025-11-10) 验证
 *
 * @source docs/pvf/PVF_FORMAT_SPECIFICATION.md:130-179
 * @source docs/Hash_Field_Investigation_COMPLETED.md
 * @verified 2025-11-10: 100% 匹配（20/20 测试文件）
 *
 * @date 2025-11-25 (Day 2 - 任务2.2)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pvf/pvf_hash.h"

/* ========================================================================== */
/* 路径规范化                                                                  */
/* ========================================================================== */

/**
 * @brief 规范化文件路径（PVF格式）
 *
 * @details 转换为小写并替换反斜杠为正斜杠
 *          参考: docs/pvf/PVF_FORMAT_SPECIFICATION.md:136
 */
void PVF_NormalizePath(const char* input, char* output) {
    size_t i;
    size_t len;

    if (input == NULL || output == NULL) {
        return;
    }

    len = strlen(input);

    for (i = 0; i < len; i++) {
        /* 步骤1: 转换为小写 */
        char c = input[i];

        if (c >= 'A' && c <= 'Z') {
            c = c + ('a' - 'A');  /* 转小写 */
        }

        /* 步骤2: 替换反斜杠为正斜杠 */
        if (c == '\\') {
            c = '/';
        }

        output[i] = c;
    }

    /* Null终止 */
    output[len] = '\0';
}

/* ========================================================================== */
/* DJB2哈希算法（PVF变体）                                                     */
/* ========================================================================== */

/**
 * @brief 计算DJB2哈希值（PVF变体）
 *
 * @details 算法:
 *          1. hash = 5381 (PVF_DJB2_INIT)
 *          2. 对每个字节: hash = (hash * 33 + byte) & 0xFFFFFFFF
 *          3. **关键**: hash = (hash * 33) & 0xFFFFFFFF
 *
 * @note 这是DJB2的变体，循环后有额外的 * 33 运算
 *       参考: docs/pvf/PVF_FORMAT_SPECIFICATION.md:142-148
 */
uint32_t PVF_DJB2Hash(const char* str) {
    uint32_t hash;
    size_t i;
    size_t len;
    unsigned char byte;

    if (str == NULL) {
        return 0;
    }

    /* 初始化哈希值 */
    hash = PVF_DJB2_INIT;  /* 5381 */

    len = strlen(str);

    /* ====================================================================== */
    /* 步骤1: 标准DJB2循环                                                    */
    /* ====================================================================== */

    for (i = 0; i < len; i++) {
        byte = (unsigned char)str[i];

        /* hash = hash * 33 + byte */
        hash = (hash * PVF_DJB2_MULTIPLIER + byte) & 0xFFFFFFFF;
    }

    /* ====================================================================== */
    /* 步骤2: **关键** - 额外的 * 33 运算！                                  */
    /* ====================================================================== */

    /*
     * 这是PVF变体的关键特征
     * 来源: pvfUtility/Helper/DataHelper.cs
     * C#代码: return num * 0x21;  // 额外的 * 33
     */
    hash = (hash * PVF_DJB2_MULTIPLIER) & 0xFFFFFFFF;

    return hash;
}

/* ========================================================================== */
/* 组合函数                                                                    */
/* ========================================================================== */

/**
 * @brief 计算文件名哈希值（规范化+哈希）
 *
 * @details 一步完成路径规范化和哈希计算
 */
uint32_t PVF_CalculateFileHash(const char* path) {
    char normalized[1024];  /* 足够大的缓冲区 */

    if (path == NULL) {
        return 0;
    }

    /* 步骤1: 规范化路径 */
    PVF_NormalizePath(path, normalized);

    /* 步骤2: 计算哈希 */
    return PVF_DJB2Hash(normalized);
}
