#ifndef PVF_PARSER_H
#define PVF_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file pvf_parser.h
 * @brief PVF文件解析接口 (头部解析、索引解析)
 *
 * @details 用于解析DNF Script.pvf文件格式
 *          包含头部解析和索引解析功能
 *
 * @source tools/pvf_parser_py2.py:50-106 (Python参考实现)
 * @date 2025-11-25 (Day 2 - 任务2.1)
 */

#include <stdio.h>
#include <stdint.h>
#include "pvf/pvf_types.h"

/* ========================================================================== */
/* 常量定义                                                                    */
/* ========================================================================== */

/**
 * @brief 期望的PVF GUID字符串
 * @note 所有有效的Script.pvf文件都应该包含此GUID
 */
#define PVF_EXPECTED_GUID "fa08bf71-4395-6a4b-a3e3-2617c9fee116"

/**
 * @brief GUID字符串长度（不含null终止符）
 */
#define PVF_GUID_LENGTH 36

/* ========================================================================== */
/* 函数声明 - 头部解析                                                         */
/* ========================================================================== */

/**
 * @brief 解析PVF文件头部
 *
 * @param fp            已打开的PVF文件句柄（读取位置应在文件开头）
 * @param header        输出：头部信息结构体指针
 * @param header_end    输出：头部结束位置（可为NULL）
 *
 * @return int PVF_SUCCESS=成功, 其他=错误码
 *
 * @details 解析步骤:
 *          1. 读取GUID长度（4字节，期望36）
 *          2. 读取GUID字符串（36字节）
 *          3. 验证GUID是否匹配
 *          4. 读取版本号（4字节）
 *          5. 读取索引区大小（4字节）
 *          6. 读取索引CRC32（4字节）
 *          7. 读取文件数量（4字节）
 *          8. 计算头部结束位置
 *
 * @example
 *     FILE* fp = fopen("/mnt/sdc1/Script.pvf", "rb");
 *     PVFHeader header;
 *     uint32_t header_end;
 *
 *     if (PVF_ParseHeader(fp, &header, &header_end) == PVF_SUCCESS) {
 *         printf("版本: %u\n", header.revision);
 *         printf("文件数: %u\n", header.index_count);
 *         printf("索引CRC32: 0x%08x\n", header.index_crc32);
 *     }
 *
 * @note 此函数不会关闭文件句柄
 * @note 读取完成后，文件指针位于头部结束位置
 */
int PVF_ParseHeader(FILE* fp, PVFHeader* header, uint32_t* header_end);

/**
 * @brief 验证PVF头部GUID是否有效
 *
 * @param guid_string   待验证的GUID字符串
 *
 * @return int 1=有效, 0=无效
 *
 * @details 比较输入字符串与PVF_EXPECTED_GUID
 *
 * @example
 *     if (PVF_ValidateGUID(header.tag_string)) {
 *         printf("GUID验证通过\n");
 *     } else {
 *         printf("GUID验证失败\n");
 *     }
 */
int PVF_ValidateGUID(const char* guid_string);

/* ========================================================================== */
/* 函数声明 - 索引解析                                                         */
/* ========================================================================== */

/**
 * @brief 解析PVF索引区（包含解密）
 *
 * @param fp    已打开的PVF文件句柄（读取位置应在索引区开头）
 * @param pack  PackSetInternal结构体指针（header字段必须已填充）
 *
 * @return int PVF_SUCCESS=成功, 其他=错误码
 *
 * @details 解析步骤:
 *          1. 分配索引数据缓冲区（header.index_header_size字节）
 *          2. 读取整个索引区
 *          3. 调用PVF_DecryptIndex()解密
 *          4. 验证CRC32是否匹配
 *          5. 解析索引条目（header.index_count个）
 *          6. 为每个文件名分配内存
 *          7. 填充indexes数组
 *
 * @note 此函数会分配内存（pack->indexes和每个条目的name字段）
 * @note 调用者负责释放内存：
 *       - 先释放每个indexes[i].name
 *       - 再释放indexes数组
 *
 * @example
 *     PackSetInternal pack;
 *     pack.fp = fp;
 *
 *     // 先解析头部
 *     PVF_ParseHeader(fp, &pack.header, &pack.header_end_offset);
 *
 *     // 再解析索引
 *     if (PVF_ParseIndex(fp, &pack) == PVF_SUCCESS) {
 *         printf("成功解析 %u 个文件\n", pack.header.index_count);
 *
 *         // 使用索引...
 *
 *         // 释放内存
 *         for (uint32_t i = 0; i < pack.header.index_count; i++) {
 *             free(pack.indexes[i].name);
 *         }
 *         free(pack.indexes);
 *     }
 */
int PVF_ParseIndex(FILE* fp, PackSetInternal* pack);

/**
 * @brief 解析单个索引条目
 *
 * @param buffer        索引数据缓冲区
 * @param offset        当前读取偏移量（输入/输出）
 * @param entry         输出：索引条目结构体指针
 *
 * @return int PVF_SUCCESS=成功, 其他=错误码
 *
 * @details 解析格式:
 *          hash_value (4B)
 *          name_length (4B)
 *          filename (variable, name_length bytes)
 *          file_size (4B)
 *          decrypt_key (4B)
 *          file_offset (4B)
 *
 * @note 此函数会为entry->name分配内存
 * @note offset会被更新为下一个条目的起始位置
 */
int PVF_ParseIndexEntry(const uint8_t* buffer, size_t* offset,
                        PVFIndexEntry* entry);

#ifdef __cplusplus
}
#endif

#endif /* PVF_PARSER_H */
