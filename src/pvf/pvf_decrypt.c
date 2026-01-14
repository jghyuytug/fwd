/**
 * @file pvf_decrypt.c
 * @brief PVF加密/解密算法实现
 *
 * @details 核心算法:
 *          dword ^= g_AdditionalKey (0x81A79011)
 *          dword = ROR32(key ^ dword, 6)
 *
 * @source tools/pvf_parser_py2.py:92-119, 155-189
 * @verified 2025-11-25: 索引解密成功，CRC32验证通过 (0x6fe5d91b)
 */

#include "pvf/pvf_decrypt.h"
#include "pvf/pvf_crc32.h"
#include <string.h>

/* ========================================================================== */
/* 辅助函数                                                                    */
/* ========================================================================== */

/**
 * @brief 32位右旋转
 *
 * @param value     待旋转的值
 * @param shift     旋转位数
 *
 * @return uint32_t 旋转后的值
 *
 * @details 算法: ROR(value, shift) = (value >> shift) | (value << (32 - shift))
 *
 * @example
 *     uint32_t result = PVF_ROR32(0x12345678, 6);
 *     printf("0x%08x\n", result);  // 0xe048d159
 *
 * @source tools/pvf_parser_py2.py:92-94
 */
uint32_t PVF_ROR32(uint32_t value, int shift) {
    return ((value >> shift) | (value << (32 - shift))) & 0xFFFFFFFF;
}

/* ========================================================================== */
/* 索引解密                                                                    */
/* ========================================================================== */

/**
 * @brief 解密PVF索引数据（带CRC32验证）
 *
 * @param data          加密的索引数据（将被原地解密）
 * @param length        数据长度（字节，必须是4的倍数）
 * @param crc_init      CRC初始值（通常为文件数量）
 * @param checksum      期望的CRC32校验和
 * @param key           解密密钥
 *
 * @return PVFResult    PVF_SUCCESS=成功, 其他=失败
 *
 * @details 解密过程:
 *          1. 检查4字节对齐
 *          2. 初始化CRC32表和CRC值
 *          3. 按4字节解密:
 *             a) dword ^= G_ADDITIONAL_KEY
 *             b) dword = ROR32(key ^ dword, 6)
 *             c) 更新CRC32
 *          4. 验证最终CRC32
 *
 * @example
 *     int result = PVF_DecryptIndex(index_data, 31670772, 379576,
 *                                   0x6fe5d91b, 0x6fe5d91b);
 *     if (result == PVF_SUCCESS) {
 *         printf("索引解密成功\n");
 *     }
 *
 * @source tools/pvf_parser_py2.py:96-154
 * @verified 2025-11-25: CRC32验证通过 (0x6fe5d91b)
 */
int PVF_DecryptIndex(void* data, size_t length, uint32_t crc_init,
                     uint32_t checksum, uint32_t key) {
    uint32_t crc;
    uint32_t* dwords;
    size_t dword_count;
    size_t i, j;
    uint32_t dword;
    uint8_t* bytes;
    extern uint32_t g_crc32_table[256];
    extern int g_crc32_table_initialized;

    /* 检查4字节对齐 */
    if ((length & 3) != 0) {
        return PVF_ERROR_INVALID_INDEX;
    }

    /* 确保CRC32表已初始化 */
    if (!g_crc32_table_initialized) {
        PVF_InitCRC32Table();
    }

    /* 初始化CRC32 */
    crc = (~crc_init) & 0xFFFFFFFF;

    /* 修改密钥（如果需要） */
    if (XOR_KEY) {
        key ^= XOR_KEY;
    }

    /* 按4字节解密 */
    dwords = (uint32_t*)data;
    dword_count = length / 4;

    for (i = 0; i < dword_count; i++) {
        dword = dwords[i];

        /* 步骤1: XOR g_AdditionalKey */
        if (G_ADDITIONAL_KEY) {
            dword ^= G_ADDITIONAL_KEY;
        }

        /* 步骤2: ROR(key ^ data, 6) */
        dword = PVF_ROR32(key ^ dword, 6);

        /* 写回解密后的数据 */
        dwords[i] = dword;

        /* 更新CRC32 (按字节) */
        bytes = (uint8_t*)&dwords[i];
        for (j = 0; j < 4; j++) {
            crc = (crc >> 8) ^ g_crc32_table[(crc ^ bytes[j]) & 0xFF];
        }
    }

    /* 验证CRC32 */
    crc = (~crc) & 0xFFFFFFFF;

    if (crc != checksum) {
        return PVF_ERROR_CRC_MISMATCH;
    }

    return PVF_SUCCESS;
}

/* ========================================================================== */
/* 文件数据解密                                                                */
/* ========================================================================== */

/**
 * @brief 解密PVF文件数据
 *
 * @param data          加密的文件数据（将被原地解密）
 * @param length        数据长度（字节）
 * @param decrypt_key   文件解密密钥
 *
 * @return PVFResult    PVF_SUCCESS=成功
 *
 * @details 解密过程（与索引解密相同，但无CRC32验证）:
 *          for each 4-byte chunk:
 *              dword ^= G_ADDITIONAL_KEY
 *              dword = ROR32(decrypt_key ^ dword, 6)
 *
 * @example
 *     PVF_DecryptFileData(file_data, 272, 0x9deef494);
 *     // file_data 现在包含解密后的文件内容
 *
 * @source tools/pvf_parser_py2.py:155-189
 * @note 文件数据可能不是4字节对齐，只解密完整的4字节块
 */
int PVF_DecryptFileData(void* data, size_t length, uint32_t decrypt_key) {
    uint32_t* dwords;
    size_t dword_count;
    size_t i;
    uint32_t dword;

    /* 按4字节解密（忽略不足4字节的尾部） */
    dwords = (uint32_t*)data;
    dword_count = length / 4;

    for (i = 0; i < dword_count; i++) {
        dword = dwords[i];

        /* 步骤1: XOR g_AdditionalKey */
        if (G_ADDITIONAL_KEY) {
            dword ^= G_ADDITIONAL_KEY;
        }

        /* 步骤2: ROR(decrypt_key ^ data, 6) */
        dword = PVF_ROR32(decrypt_key ^ dword, 6);

        /* 写回解密后的数据 */
        dwords[i] = dword;
    }

    return PVF_SUCCESS;
}
