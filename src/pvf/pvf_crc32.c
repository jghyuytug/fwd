/**
 * @file pvf_crc32.c
 * @brief CRC32算法实现
 *
 * @details 标准CRC32算法 (IEEE 802.3多项式: 0xEDB88320)
 *          用于PVF索引解密验证
 *
 * @source tools/pvf_parser_py2.py:121-145 (Python参考实现)
 * @verified 2025-11-25: 索引CRC32验证通过 (0x6fe5d91b)
 */

#include "pvf/pvf_crc32.h"

/* ========================================================================== */
/* 全局变量                                                                    */
/* ========================================================================== */

/**
 * @brief CRC32查找表 (256个条目)
 * @note 非静态，供pvf_decrypt.c使用
 */
uint32_t g_crc32_table[256];

/**
 * @brief CRC32表初始化标志
 * @note 非静态，供pvf_decrypt.c使用
 */
int g_crc32_table_initialized = 0;

/* ========================================================================== */
/* CRC32查找表生成                                                             */
/* ========================================================================== */

/**
 * @brief 生成CRC32查找表
 *
 * @details 使用标准多项式: 0xEDB88320
 *          算法：
 *          for i in 0..255:
 *              c = i
 *              for _ in 0..7:
 *                  if c & 1:
 *                      c = 0xEDB88320 ^ (c >> 1)
 *                  else:
 *                      c = c >> 1
 *              table[i] = c
 *
 * @source tools/pvf_parser_py2.py:121-134
 */
void PVF_InitCRC32Table(void) {
    int i, j;
    uint32_t c;

    /* 避免重复初始化 */
    if (g_crc32_table_initialized) {
        return;
    }

    /* 生成256个条目 */
    for (i = 0; i < 256; i++) {
        c = (uint32_t)i;

        /* 计算8位 */
        for (j = 0; j < 8; j++) {
            if (c & 1) {
                c = 0xEDB88320 ^ (c >> 1);
            } else {
                c = c >> 1;
            }
        }

        g_crc32_table[i] = c;
    }

    g_crc32_table_initialized = 1;
}

/* ========================================================================== */
/* CRC32计算                                                                   */
/* ========================================================================== */

/**
 * @brief 计算CRC32校验和（内部函数）
 *
 * @param data      待计算的数据
 * @param length    数据长度（字节）
 * @param init      CRC初始值
 *
 * @return uint32_t CRC32校验和
 */
static uint32_t _calc_crc32(const void* data, size_t length, uint32_t init) {
    uint32_t crc;
    const uint8_t* bytes;
    size_t i;

    /* 确保查找表已初始化 */
    if (!g_crc32_table_initialized) {
        PVF_InitCRC32Table();
    }

    /* 初始化CRC */
    crc = init;

    /* 逐字节计算 */
    bytes = (const uint8_t*)data;
    for (i = 0; i < length; i++) {
        crc = (crc >> 8) ^ g_crc32_table[(crc ^ bytes[i]) & 0xFF];
    }

    return crc;
}

/**
 * @brief 计算数据的CRC32校验和
 *
 * @param data      待计算的数据
 * @param length    数据长度（字节）
 *
 * @return uint32_t CRC32校验和
 *
 * @details 标准CRC32算法:
 *          1. 初始值: 0xFFFFFFFF
 *          2. 对每个字节: crc = (crc >> 8) ^ table[(crc ^ byte) & 0xFF]
 *          3. 最终值: ~crc
 *
 * @example
 *     const char* data = "Hello World";
 *     uint32_t crc = PVF_CalcCRC32(data, strlen(data));
 *     printf("CRC32: 0x%08x\n", crc);
 */
uint32_t PVF_CalcCRC32(const void* data, size_t length) {
    uint32_t crc = _calc_crc32(data, length, 0xFFFFFFFF);
    return ~crc;
}

/**
 * @brief 计算数据的CRC32校验和（带自定义初始值）
 *
 * @param data      待计算的数据
 * @param length    数据长度（字节）
 * @param init      CRC初始值（已经按位取反）
 *
 * @return uint32_t CRC32校验和
 *
 * @details 用于PVF索引解密，初始值为 ~index_count
 *
 * @example
 *     // PVF索引解密：初始值为文件数量的按位取反
 *     uint32_t crc = PVF_CalcCRC32_Init(data, length, ~file_count);
 */
uint32_t PVF_CalcCRC32_Init(const void* data, size_t length, uint32_t init) {
    uint32_t crc = _calc_crc32(data, length, init);
    return ~crc;
}

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
 *     }
 */
int PVF_VerifyCRC32(const void* data, size_t length, uint32_t expected_crc) {
    uint32_t calculated_crc = PVF_CalcCRC32(data, length);
    return (calculated_crc == expected_crc) ? 1 : 0;
}
