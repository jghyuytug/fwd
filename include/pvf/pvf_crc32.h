#ifndef PVF_CRC32_H
#define PVF_CRC32_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file pvf_crc32.h
 * @brief CRC32算法接口 (用于PVF索引解密和验证)
 *
 * @details 使用标准CRC32算法 (IEEE 802.3多项式: 0xEDB88320)
 *          用途:
 *          1. 索引数据解密时的CRC32验证
 *          2. 文件数据的完整性校验
 *
 * @source tools/pvf_parser_py2.py:121-145 (Python参考实现)
 * @verified 2025-11-25: CRC32验证通过 (0x6fe5d91b)
 */

#include <stdint.h>
#include <stddef.h>

/* ========================================================================== */
/* 全局变量声明                                                                */
/* ========================================================================== */

/**
 * @brief CRC32查找表 (供外部访问)
 * @note 由 pvf_crc32.c 定义，pvf_decrypt.c 使用
 */
extern uint32_t g_crc32_table[256];

/**
 * @brief CRC32表初始化标志 (供外部访问)
 */
extern int g_crc32_table_initialized;

/* ========================================================================== */
/* 函数声明                                                                    */
/* ========================================================================== */

/**
 * @brief 初始化CRC32查找表
 *
 * @details 生成256个条目的CRC32查找表，用于加速计算
 *          使用标准多项式: 0xEDB88320
 *
 * @note 第一次调用CRC32相关函数时会自动初始化，
 *       也可以手动调用此函数提前初始化
 *
 * @example
 *     PVF_InitCRC32Table();  // 可选：提前初始化
 */
void PVF_InitCRC32Table(void);

/**
 * @brief 计算数据的CRC32校验和
 *
 * @param data      待计算的数据
 * @param length    数据长度（字节）
 *
 * @return uint32_t CRC32校验和
 *
 * @details 计算过程:
 *          1. 初始值: 0xFFFFFFFF
 *          2. 对每个字节: crc = (crc >> 8) ^ table[(crc ^ byte) & 0xFF]
 *          3. 最终值: ~crc
 *
 * @example
 *     const char* data = "Hello World";
 *     uint32_t crc = PVF_CalcCRC32(data, strlen(data));
 *     printf("CRC32: 0x%08x\n", crc);
 */
uint32_t PVF_CalcCRC32(const void* data, size_t length);

/**
 * @brief 计算数据的CRC32校验和（带自定义初始值）
 *
 * @param data      待计算的数据
 * @param length    数据长度（字节）
 * @param init      CRC初始值
 *
 * @return uint32_t CRC32校验和
 *
 * @details 用于PVF索引解密，初始值为 ~index_count
 *
 * @example
 *     // PVF索引解密：初始值为文件数量的按位取反
 *     uint32_t crc = PVF_CalcCRC32_Init(data, length, ~file_count);
 */
uint32_t PVF_CalcCRC32_Init(const void* data, size_t length, uint32_t init);

/**
 * @brief 验证数据的CRC32校验和
 *
 * @param data          待验证的数据
 * @param length        数据长度（字节）
 * @param expected_crc  期望的CRC32值
 *
 * @return int 1=验证通过, 0=验证失败
 *
 * @example
 *     if (PVF_VerifyCRC32(data, length, 0x6fe5d91b)) {
 *         printf("CRC32验证通过\n");
 *     } else {
 *         printf("CRC32验证失败\n");
 *     }
 */
int PVF_VerifyCRC32(const void* data, size_t length, uint32_t expected_crc);

#ifdef __cplusplus
}
#endif

#endif /* PVF_CRC32_H */
