#ifndef PVF_PACKSET_H
#define PVF_PACKSET_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file pvf_packset.h
 * @brief PackSet接口 - PVF文件管理器
 *
 * @details PackSet封装了PVF文件的所有访问逻辑:
 *          - 打开/关闭PVF文件
 *          - 按路径查找文件（O(1)哈希表查找）
 *          - 提取并解密文件内容
 *
 * @date 2025-11-25 (Day 3)
 */

#include <stdint.h>
#include "pvf/pvf_types.h"

/* ========================================================================== */
/* 常量定义                                                                    */
/* ========================================================================== */

/**
 * @brief Hash表大小（桶数量）
 * @note 256是一个合理的桶数量，提供良好的性能/内存平衡
 */
#define PVF_HASH_TABLE_SIZE 256

/* ========================================================================== */
/* 函数声明 - PackSet生命周期                                                  */
/* ========================================================================== */

/**
 * @brief 打开PVF文件并初始化PackSet
 *
 * @param filepath      PVF文件路径
 * @param pack          输出：PackSetInternal结构体指针（必须预分配）
 *
 * @return int PVF_SUCCESS=成功, 其他=错误码
 *
 * @details 操作步骤:
 *          1. 打开文件
 *          2. 解析头部 (PVF_ParseHeader)
 *          3. 解析索引 (PVF_ParseIndex)
 *          4. 构建Hash表 (PackSet_BuildHashTable)
 *
 * @note 调用者负责分配pack内存
 * @note 成功后，pack->fp、pack->indexes、pack->hash_table均已初始化
 * @note 使用完毕后必须调用PackSet_Close()释放资源
 *
 * @example
 *     PackSetInternal pack;
 *     if (PackSet_Open("/home/dxf/game/Script.pvf", &pack) == PVF_SUCCESS) {
 *         // 使用pack...
 *         PackSet_Close(&pack);
 *     }
 */
int PackSet_Open(const char* filepath, PackSetInternal* pack);

/**
 * @brief 关闭PackSet并释放所有资源
 *
 * @param pack          PackSetInternal结构体指针
 *
 * @details 清理步骤:
 *          1. 释放Hash表节点
 *          2. 释放Hash表数组
 *          3. 释放每个索引条目的文件名
 *          4. 释放索引数组
 *          5. 关闭文件句柄
 *
 * @note 此函数总是成功（不返回错误码）
 * @note 调用后pack中的所有指针均失效
 *
 * @example
 *     PackSetInternal pack;
 *     PackSet_Open("Script.pvf", &pack);
 *     // ... 使用 ...
 *     PackSet_Close(&pack);
 */
void PackSet_Close(PackSetInternal* pack);

/* ========================================================================== */
/* 函数声明 - Hash表                                                           */
/* ========================================================================== */

/**
 * @brief 构建Hash表（用于O(1)文件查找）
 *
 * @param pack          PackSetInternal结构体指针（索引已解析）
 *
 * @return int PVF_SUCCESS=成功, 其他=错误码
 *
 * @details 构建步骤:
 *          1. 分配Hash表数组（256桶）
 *          2. 初始化所有桶为NULL
 *          3. 遍历所有索引条目
 *          4. 计算每个文件的hash_value % 256
 *          5. 插入到对应桶的链表头
 *
 * @note 冲突使用链表法解决
 * @note 此函数在PackSet_Open()中自动调用
 *
 * @complexity 时间: O(n), 空间: O(n)
 */
int PackSet_BuildHashTable(PackSetInternal* pack);

/**
 * @brief 销毁Hash表并释放所有节点
 *
 * @param pack          PackSetInternal结构体指针
 *
 * @details 清理步骤:
 *          1. 遍历所有桶
 *          2. 释放每个桶的链表节点
 *          3. 释放Hash表数组
 *
 * @note 此函数在PackSet_Close()中自动调用
 */
void PackSet_DestroyHashTable(PackSetInternal* pack);

/* ========================================================================== */
/* 函数声明 - 文件查找                                                         */
/* ========================================================================== */

/**
 * @brief 按路径查找文件
 *
 * @param pack          PackSetInternal结构体指针
 * @param filepath      文件路径（例如："Etc\\ItemScript.txt"）
 *
 * @return PVFIndexEntry* 索引条目指针，NULL=未找到
 *
 * @details 查找步骤:
 *          1. 规范化路径 (PVF_NormalizePath)
 *          2. 计算DJB2哈希 (PVF_DJB2Hash)
 *          3. 计算桶索引: hash % 256
 *          4. 在桶的链表中查找匹配的文件名
 *
 * @note 返回的指针指向pack->indexes数组，不需要手动释放
 * @note 时间复杂度: O(1) 平均, O(n) 最坏（冲突严重）
 *
 * @example
 *     PVFIndexEntry* entry = PackSet_GetFile(&pack, "Etc\\ItemScript.txt");
 *     if (entry != NULL) {
 *         printf("找到文件: %s, 大小: %u 字节\n", entry->name, entry->size);
 *     }
 */
PVFIndexEntry* PackSet_GetFile(PackSetInternal* pack, const char* filepath);

/* ========================================================================== */
/* 函数声明 - 文件提取                                                         */
/* ========================================================================== */

/**
 * @brief 提取并解密文件内容
 *
 * @param pack          PackSetInternal结构体指针
 * @param entry         索引条目指针（由PackSet_GetFile返回）
 * @param output_buffer 输出：文件内容缓冲区（调用者负责分配）
 * @param buffer_size   缓冲区大小（必须 >= entry->size）
 *
 * @return int PVF_SUCCESS=成功, 其他=错误码
 *
 * @details 提取步骤:
 *          1. 验证缓冲区大小
 *          2. 定位到文件偏移 (header_end_offset + index_size + entry->offset)
 *          3. 读取加密数据
 *          4. 解密数据 (PVF_DecryptFileData)
 *          5. 写入输出缓冲区
 *
 * @note 调用者必须预分配output_buffer（大小至少entry->size字节）
 * @note 文件数据偏移 = 头部大小 + 索引大小 + entry->offset
 *
 * @example
 *     PVFIndexEntry* entry = PackSet_GetFile(&pack, "Etc\\ItemScript.txt");
 *     if (entry != NULL) {
 *         char* buffer = (char*)malloc(entry->size);
 *         if (PackSet_ExtractFile(&pack, entry, buffer, entry->size) == PVF_SUCCESS) {
 *             // 使用buffer...
 *         }
 *         free(buffer);
 *     }
 */
int PackSet_ExtractFile(PackSetInternal* pack, PVFIndexEntry* entry,
                        void* output_buffer, size_t buffer_size);

/**
 * @brief 按路径提取文件（组合查找+提取）
 *
 * @param pack          PackSetInternal结构体指针
 * @param filepath      文件路径
 * @param output_buffer 输出：文件内容缓冲区
 * @param buffer_size   缓冲区大小
 * @param actual_size   输出：实际文件大小（可为NULL）
 *
 * @return int PVF_SUCCESS=成功, 其他=错误码
 *
 * @details 内部调用:
 *          1. PackSet_GetFile() 查找文件
 *          2. PackSet_ExtractFile() 提取内容
 *
 * @example
 *     char buffer[65536];
 *     uint32_t size;
 *     if (PackSet_ExtractFileByPath(&pack, "Etc\\ItemScript.txt",
 *                                   buffer, sizeof(buffer), &size) == PVF_SUCCESS) {
 *         printf("提取成功，大小: %u 字节\n", size);
 *     }
 */
int PackSet_ExtractFileByPath(PackSetInternal* pack, const char* filepath,
                               void* output_buffer, size_t buffer_size,
                               uint32_t* actual_size);

#ifdef __cplusplus
}
#endif

#endif /* PVF_PACKSET_H */
