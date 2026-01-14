#ifndef PVF_H
#define PVF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file pvf.h
 * @brief PVF库统一接口 - 一站式包含所有PVF功能
 *
 * @details 这是PVF库的主要入口点，包含所有必需的头文件
 *          用户只需包含此文件即可使用完整的PVF功能
 *
 * @version 1.0.0
 * @date 2025-11-25
 *
 * @example 基本使用:
 *     #include "pvf/pvf.h"
 *
 *     int main(void) {
 *         PackSetInternal pack;
 *
 *         // 初始化
 *         PVF_InitCRC32Table();
 *
 *         // 打开PVF文件
 *         if (PackSet_Open("/path/to/Script.pvf", &pack) == PVF_SUCCESS) {
 *             // 查找文件
 *             PVFIndexEntry* entry = PackSet_GetFile(&pack, "Etc\\ItemScript.txt");
 *             if (entry != NULL) {
 *                 // 提取文件
 *                 char* buffer = malloc(entry->size);
 *                 PackSet_ExtractFile(&pack, entry, buffer, entry->size);
 *                 // 使用文件内容...
 *                 free(buffer);
 *             }
 *
 *             // 关闭
 *             PackSet_Close(&pack);
 *         }
 *
 *         return 0;
 *     }
 */

/* ========================================================================== */
/* 核心数据结构和类型                                                          */
/* ========================================================================== */

#include "pvf/pvf_types.h"

/* ========================================================================== */
/* CRC32算法                                                                   */
/* ========================================================================== */

#include "pvf/pvf_crc32.h"

/* ========================================================================== */
/* 加密/解密算法                                                               */
/* ========================================================================== */

#include "pvf/pvf_decrypt.h"

/* ========================================================================== */
/* 哈希算法（DJB2变体）                                                        */
/* ========================================================================== */

#include "pvf/pvf_hash.h"

/* ========================================================================== */
/* 头部和索引解析                                                              */
/* ========================================================================== */

#include "pvf/pvf_parser.h"

/* ========================================================================== */
/* PackSet（PVF文件管理器）                                                    */
/* ========================================================================== */

#include "pvf/pvf_packset.h"

/* ========================================================================== */
/* 版本信息                                                                    */
/* ========================================================================== */

#define PVF_VERSION_MAJOR 1
#define PVF_VERSION_MINOR 0
#define PVF_VERSION_PATCH 0
#define PVF_VERSION_STRING "1.0.0"

/* ========================================================================== */
/* 快速API（常用操作的便捷函数）                                               */
/* ========================================================================== */

/**
 * @brief 获取PVF库版本字符串
 *
 * @return const char* 版本字符串（例如："1.0.0"）
 */
static inline const char* PVF_GetVersion(void) {
    return PVF_VERSION_STRING;
}

/**
 * @brief 初始化PVF库（推荐在使用前调用）
 *
 * @details 执行必要的初始化:
 *          - 初始化CRC32查找表
 *
 * @note 此函数是线程安全的，可以多次调用
 */
static inline void PVF_Initialize(void) {
    PVF_InitCRC32Table();
}

/* ========================================================================== */
/* 使用指南                                                                    */
/* ========================================================================== */

/**
 * @page usage_guide PVF库使用指南
 *
 * @section quick_start 快速开始
 *
 * @subsection step1 步骤1: 包含头文件
 * @code
 * #include "pvf/pvf.h"
 * @endcode
 *
 * @subsection step2 步骤2: 初始化
 * @code
 * PVF_Initialize();  // 或者直接调用 PVF_InitCRC32Table();
 * @endcode
 *
 * @subsection step3 步骤3: 打开PVF文件
 * @code
 * PackSetInternal pack;
 * int result = PackSet_Open("/path/to/Script.pvf", &pack);
 * if (result != PVF_SUCCESS) {
 *     fprintf(stderr, "打开失败: %d\n", result);
 *     return 1;
 * }
 * @endcode
 *
 * @subsection step4 步骤4: 查找和提取文件
 * @code
 * // 方法1: 分步操作
 * PVFIndexEntry* entry = PackSet_GetFile(&pack, "Etc\\ItemScript.txt");
 * if (entry != NULL) {
 *     char* buffer = malloc(entry->size);
 *     if (PackSet_ExtractFile(&pack, entry, buffer, entry->size) == PVF_SUCCESS) {
 *         // 使用文件内容...
 *         printf("文件大小: %u 字节\n", entry->size);
 *     }
 *     free(buffer);
 * }
 *
 * // 方法2: 一步提取
 * char buffer[65536];
 * uint32_t size;
 * if (PackSet_ExtractFileByPath(&pack, "Etc\\ItemScript.txt",
 *                               buffer, sizeof(buffer), &size) == PVF_SUCCESS) {
 *     printf("提取成功: %u 字节\n", size);
 * }
 * @endcode
 *
 * @subsection step5 步骤5: 关闭
 * @code
 * PackSet_Close(&pack);
 * @endcode
 *
 * @section error_handling 错误处理
 *
 * 所有返回int的函数使用以下返回值:
 * - PVF_SUCCESS (0): 操作成功
 * - PVF_ERROR_FILE_NOT_FOUND: 文件未找到
 * - PVF_ERROR_INVALID_HEADER: 头部格式错误
 * - PVF_ERROR_CRC_MISMATCH: CRC32校验失败
 * - PVF_ERROR_MEMORY: 内存分配失败
 * - 其他负数: 各种错误（参见pvf_types.h）
 *
 * @section performance 性能考虑
 *
 * - **文件查找**: O(1) 平均时间（使用Hash表）
 * - **内存使用**: 约 37.7 MB（379,576个文件）
 * - **打开时间**: 约 2-3 秒（解析+构建Hash表）
 * - **提取速度**: 取决于文件大小和磁盘I/O
 *
 * @section thread_safety 线程安全
 *
 * - PVF_InitCRC32Table(): 线程安全（使用全局标志）
 * - PackSet实例: 不是线程安全的
 * - 建议: 每个线程使用独立的PackSet实例
 *
 * @section memory_management 内存管理
 *
 * - PackSet_Open(): 分配内存（索引+Hash表）
 * - PackSet_Close(): 释放所有内存
 * - 用户负责: 提取文件的缓冲区分配/释放
 */

#ifdef __cplusplus
}
#endif

#endif /* PVF_H */
