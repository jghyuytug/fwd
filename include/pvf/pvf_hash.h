#ifndef PVF_HASH_H
#define PVF_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file pvf_hash.h
 * @brief PVF文件路径哈希算法 (DJB2变体)
 *
 * @details 实现DNF Script.pvf使用的文件名哈希算法
 *          基于DJB2，但有关键变化：
 *          1. 文件名预处理：转小写，\ 替换为 /
 *          2. 使用cp949编码转为字节
 *          3. 标准DJB2: hash = hash * 33 + byte
 *          4. **关键**：额外的 * 33 运算！
 *
 * @source docs/pvf/PVF_FORMAT_SPECIFICATION.md:130-179
 * @source docs/Hash_Field_Investigation_COMPLETED.md
 * @verified 2025-11-10: 100% 匹配（20/20 测试文件）
 *
 * @example
 *     const char* path = "Etc\\ItemScript.txt";
 *     char normalized[256];
 *     uint32_t hash;
 *
 *     PVF_NormalizePath(path, normalized);
 *     // normalized = "etc/itemscript.txt"
 *
 *     hash = PVF_DJB2Hash(normalized);
 *     // hash = 文件名哈希值
 */

#include <stdint.h>

/* ========================================================================== */
/* 常量定义                                                                    */
/* ========================================================================== */

/**
 * @brief DJB2哈希初始值
 */
#define PVF_DJB2_INIT 5381

/**
 * @brief DJB2哈希乘数
 */
#define PVF_DJB2_MULTIPLIER 33

/* ========================================================================== */
/* 函数声明                                                                    */
/* ========================================================================== */

/**
 * @brief 规范化文件路径（PVF格式）
 *
 * @param input     原始路径（可能包含大写字母和反斜杠）
 * @param output    输出：规范化后的路径（必须预分配足够空间）
 *
 * @details 规范化步骤:
 *          1. 转换所有字符为小写
 *          2. 替换所有 '\' 为 '/'
 *
 * @note output缓冲区必须至少有 strlen(input)+1 字节
 *
 * @example
 *     char normalized[256];
 *     PVF_NormalizePath("Etc\\ItemScript.txt", normalized);
 *     // normalized = "etc/itemscript.txt"
 */
void PVF_NormalizePath(const char* input, char* output);

/**
 * @brief 计算文件名的DJB2哈希值（PVF变体）
 *
 * @param str       规范化后的文件路径（小写，正斜杠）
 *
 * @return uint32_t 哈希值
 *
 * @details 算法（PVF变体）:
 *          1. hash = 5381
 *          2. 对每个字节: hash = (hash * 33 + byte) & 0xFFFFFFFF
 *          3. **关键**: hash = (hash * 33) & 0xFFFFFFFF
 *
 * @note **必须使用cp949编码**，但在ASCII范围内（0-127）与UTF-8相同
 *       对于纯ASCII文件名，可直接使用
 *
 * @warning 文件名必须已规范化（PVF_NormalizePath）
 *
 * @example
 *     char normalized[256];
 *     uint32_t hash;
 *
 *     PVF_NormalizePath("Etc\\ItemScript.txt", normalized);
 *     hash = PVF_DJB2Hash(normalized);
 *
 *     printf("Hash: 0x%08x\n", hash);
 */
uint32_t PVF_DJB2Hash(const char* str);

/**
 * @brief 计算文件名哈希值（一步完成：规范化+哈希）
 *
 * @param path      原始文件路径
 *
 * @return uint32_t 哈希值
 *
 * @details 内部调用:
 *          1. PVF_NormalizePath()
 *          2. PVF_DJB2Hash()
 *
 * @example
 *     uint32_t hash = PVF_CalculateFileHash("Etc\\ItemScript.txt");
 */
uint32_t PVF_CalculateFileHash(const char* path);

#ifdef __cplusplus
}
#endif

#endif /* PVF_HASH_H */
