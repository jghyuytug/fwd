#ifndef PVF_DECRYPT_H
#define PVF_DECRYPT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file pvf_decrypt.h
 * @brief PVF加密/解密算法接口
 *
 * @details PVF使用两种解密算法:
 *          1. 索引解密: 解密索引数据（带CRC32验证）
 *          2. 文件解密: 解密文件数据
 *
 *          核心算法:
 *          dword ^= g_AdditionalKey  (0x81A79011)
 *          dword = ROR32(key ^ dword, 6)
 *
 * @source tools/pvf_parser_py2.py:92-119, 155-189
 * @source docs/pvf/PVF_FORMAT_SPECIFICATION.md
 * @verified 2025-11-25: 索引解密成功，CRC32验证通过 (0x6fe5d91b)
 */

#include "pvf/pvf_types.h"

/* ========================================================================== */
/* 加密常量                                                                    */
/* ========================================================================== */

/**
 * @brief g_AdditionalKey (关键参数)
 *
 * @details 从conf目录反汇编代码获取的正确值
 *          此参数对解密至关重要，不能修改！
 *
 * @verified 2025-11-25: 使用此值解密索引，CRC32验证通过
 */
#define G_ADDITIONAL_KEY 0x81A79011

/**
 * @brief XOR_KEY (当前版本未使用)
 *
 * @details 生产环境 Script.pvf 使用 XOR_KEY = 0
 *          保留此定义以便将来支持其他版本
 */
#define XOR_KEY 0

/* ========================================================================== */
/* 解密函数                                                                    */
/* ========================================================================== */

/**
 * @brief 解密PVF索引数据（带CRC32验证）
 *
 * @param data          加密的索引数据（将被原地解密）
 * @param length        数据长度（字节，必须是4的倍数）
 * @param crc_init      CRC初始值（通常为文件数量 index_count）
 * @param checksum      期望的CRC32校验和（通常为 index_crc32）
 * @param key           解密密钥（通常为 index_crc32）
 *
 * @return PVFResult    PVF_SUCCESS=成功, 其他=失败
 *
 * @details 解密过程:
 *          1. 检查4字节对齐
 *          2. 初始化CRC32 (crc = ~crc_init)
 *          3. 对每4字节:
 *             a) dword ^= G_ADDITIONAL_KEY
 *             b) dword = ROR32(key ^ dword, 6)
 *             c) 更新CRC32
 *          4. 验证CRC32 (~crc == checksum)
 *
 * @example Script.pvf索引解密:
 *     uint8_t* index_data = malloc(31670772);
 *     fread(index_data, 1, 31670772, fp);
 *
 *     int result = PVF_DecryptIndex(
 *         index_data,
 *         31670772,           // 索引大小
 *         379576,             // 文件数量 (CRC初始值)
 *         0x6fe5d91b,         // 期望CRC32
 *         0x6fe5d91b          // 解密密钥
 *     );
 *
 *     if (result == PVF_SUCCESS) {
 *         printf("索引解密成功\n");
 *         // index_data 现在包含解密后的索引
 *     }
 *
 * @note 数据将被原地解密，不会分配新内存
 * @warning 如果CRC32验证失败，返回 PVF_ERROR_CRC_MISMATCH
 */
int PVF_DecryptIndex(void* data, size_t length, uint32_t crc_init,
                     uint32_t checksum, uint32_t key);

/**
 * @brief 解密PVF文件数据
 *
 * @param data          加密的文件数据（将被原地解密）
 * @param length        数据长度（字节）
 * @param decrypt_key   文件解密密钥（来自索引条目）
 *
 * @return PVFResult    PVF_SUCCESS=成功, 其他=失败
 *
 * @details 解密过程（与索引解密相同，但无CRC32验证）:
 *          for each 4-byte chunk:
 *              dword ^= G_ADDITIONAL_KEY
 *              dword = ROR32(decrypt_key ^ dword, 6)
 *
 * @example 提取文件:
 *     // 从索引获取文件信息
 *     PVFIndexEntry* entry = &indexes[0];
 *
 *     // 读取加密数据
 *     uint8_t* file_data = malloc(entry->size);
 *     fseek(fp, file_offset, SEEK_SET);
 *     fread(file_data, 1, entry->size, fp);
 *
 *     // 解密文件数据
 *     PVF_DecryptFileData(file_data, entry->size, entry->decrypt_key);
 *
 *     // file_data 现在包含解密后的文件内容
 *
 * @note 数据将被原地解密，不会分配新内存
 * @note 文件数据解密不进行CRC32验证
 */
int PVF_DecryptFileData(void* data, size_t length, uint32_t decrypt_key);

/**
 * @brief 32位右旋转 (ROR32)
 *
 * @param value     待旋转的值
 * @param shift     旋转位数（通常为6）
 *
 * @return uint32_t 旋转后的值
 *
 * @details 算法: ROR(value, shift) = (value >> shift) | (value << (32 - shift))
 *
 * @example
 *     uint32_t result = PVF_ROR32(0x12345678, 6);
 *     // result = 0xe048d159
 */
uint32_t PVF_ROR32(uint32_t value, int shift);

#ifdef __cplusplus
}
#endif

#endif /* PVF_DECRYPT_H */
