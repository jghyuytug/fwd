/**
 * @file pvf_packset.c
 * @brief PackSet实现 - PVF文件管理器
 *
 * @details 实现PVF文件的打开、查找、提取功能
 *
 * @date 2025-11-25 (Day 3)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pvf/pvf_packset.h"
#include "pvf/pvf_parser.h"
#include "pvf/pvf_hash.h"
#include "pvf/pvf_decrypt.h"

/* ========================================================================== */
/* Hash表实现                                                                  */
/* ========================================================================== */

/**
 * @brief 构建Hash表（用于O(1)文件查找）
 */
int PackSet_BuildHashTable(PackSetInternal* pack) {
    uint32_t i;
    uint32_t bucket_index;
    PVFHashNode* node;

    /* 参数验证 */
    if (pack == NULL || pack->indexes == NULL) {
        return PVF_ERROR_INVALID_PARAM;
    }

    /* ====================================================================== */
    /* Step 1: 分配Hash表数组（256桶）                                        */
    /* ====================================================================== */

    pack->hash_table_size = PVF_HASH_TABLE_SIZE;
    pack->hash_table = (PVFHashNode**)malloc(
        sizeof(PVFHashNode*) * pack->hash_table_size
    );

    if (pack->hash_table == NULL) {
        return PVF_ERROR_MEMORY;
    }

    /* ====================================================================== */
    /* Step 2: 初始化所有桶为NULL                                             */
    /* ====================================================================== */

    for (i = 0; i < pack->hash_table_size; i++) {
        pack->hash_table[i] = NULL;
    }

    /* ====================================================================== */
    /* Step 3: 遍历所有索引条目，插入Hash表                                   */
    /* ====================================================================== */

    for (i = 0; i < pack->header.index_count; i++) {
        /* 计算桶索引: hash_value % 256 */
        bucket_index = pack->indexes[i].hash_value % pack->hash_table_size;

        /* 分配节点 */
        node = (PVFHashNode*)malloc(sizeof(PVFHashNode));
        if (node == NULL) {
            /* 分配失败，清理已分配的节点 */
            PackSet_DestroyHashTable(pack);
            return PVF_ERROR_MEMORY;
        }

        /* 填充节点 */
        node->index_id = i;  /* 索引数组中的位置 */
        node->next = pack->hash_table[bucket_index];  /* 链表头插入 */

        /* 插入到桶 */
        pack->hash_table[bucket_index] = node;
    }

    return PVF_SUCCESS;
}

/**
 * @brief 销毁Hash表并释放所有节点
 */
void PackSet_DestroyHashTable(PackSetInternal* pack) {
    uint32_t i;
    PVFHashNode* node;
    PVFHashNode* next;

    if (pack == NULL || pack->hash_table == NULL) {
        return;
    }

    /* 遍历所有桶 */
    for (i = 0; i < pack->hash_table_size; i++) {
        node = pack->hash_table[i];

        /* 释放链表 */
        while (node != NULL) {
            next = node->next;
            free(node);
            node = next;
        }
    }

    /* 释放Hash表数组 */
    free(pack->hash_table);
    pack->hash_table = NULL;
    pack->hash_table_size = 0;
}

/* ========================================================================== */
/* PackSet生命周期                                                             */
/* ========================================================================== */

/**
 * @brief 打开PVF文件并初始化PackSet
 */
int PackSet_Open(const char* filepath, PackSetInternal* pack) {
    int result;

    /* 参数验证 */
    if (filepath == NULL || pack == NULL) {
        return PVF_ERROR_INVALID_PARAM;
    }

    /* 初始化结构体 */
    memset(pack, 0, sizeof(PackSetInternal));

    /* ====================================================================== */
    /* Step 1: 打开文件                                                       */
    /* ====================================================================== */

    pack->fp = fopen(filepath, "rb");
    if (pack->fp == NULL) {
        return PVF_ERROR_FILE_NOT_FOUND;
    }

    /* ====================================================================== */
    /* Step 2: 解析头部                                                       */
    /* ====================================================================== */

    result = PVF_ParseHeader(pack->fp, &pack->header, &pack->header_end_offset);
    if (result != PVF_SUCCESS) {
        fclose(pack->fp);
        return result;
    }

    /* ====================================================================== */
    /* Step 3: 解析索引                                                       */
    /* ====================================================================== */

    result = PVF_ParseIndex(pack->fp, pack);
    if (result != PVF_SUCCESS) {
        fclose(pack->fp);
        return result;
    }

    /* ====================================================================== */
    /* Step 4: 构建Hash表                                                     */
    /* ====================================================================== */

    result = PackSet_BuildHashTable(pack);
    if (result != PVF_SUCCESS) {
        /* 清理索引 */
        uint32_t i;
        for (i = 0; i < pack->header.index_count; i++) {
            free(pack->indexes[i].name);
        }
        free(pack->indexes);
        fclose(pack->fp);
        return result;
    }

    return PVF_SUCCESS;
}

/**
 * @brief 关闭PackSet并释放所有资源
 */
void PackSet_Close(PackSetInternal* pack) {
    uint32_t i;

    if (pack == NULL) {
        return;
    }

    /* ====================================================================== */
    /* Step 1: 销毁Hash表                                                     */
    /* ====================================================================== */

    PackSet_DestroyHashTable(pack);

    /* ====================================================================== */
    /* Step 2: 释放索引数组                                                   */
    /* ====================================================================== */

    if (pack->indexes != NULL) {
        /* 释放每个文件名 */
        for (i = 0; i < pack->header.index_count; i++) {
            if (pack->indexes[i].name != NULL) {
                free(pack->indexes[i].name);
            }
        }

        /* 释放索引数组 */
        free(pack->indexes);
        pack->indexes = NULL;
    }

    /* ====================================================================== */
    /* Step 3: 关闭文件句柄                                                   */
    /* ====================================================================== */

    if (pack->fp != NULL) {
        fclose(pack->fp);
        pack->fp = NULL;
    }

    /* 清零结构体 */
    memset(pack, 0, sizeof(PackSetInternal));
}

/* ========================================================================== */
/* 文件查找                                                                    */
/* ========================================================================== */

/**
 * @brief 按路径查找文件
 */
PVFIndexEntry* PackSet_GetFile(PackSetInternal* pack, const char* filepath) {
    char normalized[1024];
    uint32_t hash;
    uint32_t bucket_index;
    PVFHashNode* node;
    PVFIndexEntry* entry;

    /* 参数验证 */
    if (pack == NULL || filepath == NULL || pack->hash_table == NULL) {
        return NULL;
    }

    /* ====================================================================== */
    /* Step 1: 规范化路径                                                     */
    /* ====================================================================== */

    PVF_NormalizePath(filepath, normalized);

    /* ====================================================================== */
    /* Step 2: 计算哈希                                                       */
    /* ====================================================================== */

    hash = PVF_DJB2Hash(normalized);

    /* ====================================================================== */
    /* Step 3: 计算桶索引                                                     */
    /* ====================================================================== */

    bucket_index = hash % pack->hash_table_size;

    /* ====================================================================== */
    /* Step 4: 在桶的链表中查找                                               */
    /* ====================================================================== */

    node = pack->hash_table[bucket_index];

    while (node != NULL) {
        /* 获取索引条目 */
        entry = &pack->indexes[node->index_id];

        /* 比较哈希值（快速过滤） */
        if (entry->hash_value == hash) {
            /* 比较文件名（确认匹配） */
            if (strcmp(entry->name, normalized) == 0) {
                return entry;  /* 找到！ */
            }
        }

        /* 继续下一个节点 */
        node = node->next;
    }

    /* 未找到 */
    return NULL;
}

/* ========================================================================== */
/* 文件提取                                                                    */
/* ========================================================================== */

/**
 * @brief 提取并解密文件内容
 */
int PackSet_ExtractFile(PackSetInternal* pack, PVFIndexEntry* entry,
                        void* output_buffer, size_t buffer_size) {
    uint32_t file_data_offset;
    size_t bytes_read;
    int decrypt_result;

    /* 参数验证 */
    if (pack == NULL || entry == NULL || output_buffer == NULL) {
        return PVF_ERROR_INVALID_PARAM;
    }

    /* 验证缓冲区大小 */
    if (buffer_size < entry->size) {
        return PVF_ERROR_MEMORY;
    }

    /* ====================================================================== */
    /* Step 1: 计算文件数据偏移                                               */
    /* ====================================================================== */

    /*
     * 文件数据偏移 = 头部大小 + 索引大小 + entry->offset
     *              = 56 + index_header_size + entry->offset
     */
    file_data_offset = pack->header_end_offset +
                       pack->header.index_header_size +
                       entry->offset;

    /* ====================================================================== */
    /* Step 2: 定位到文件偏移                                                 */
    /* ====================================================================== */

    if (fseek(pack->fp, file_data_offset, SEEK_SET) != 0) {
        return PVF_ERROR_READ_FAILED;
    }

    /* ====================================================================== */
    /* Step 3: 读取加密数据                                                   */
    /* ====================================================================== */

    bytes_read = fread(output_buffer, 1, entry->size, pack->fp);
    if (bytes_read != entry->size) {
        return PVF_ERROR_READ_FAILED;
    }

    /* ====================================================================== */
    /* Step 4: 解密数据                                                       */
    /* ====================================================================== */

    decrypt_result = PVF_DecryptFileData(
        output_buffer,
        entry->size,
        entry->decrypt_key
    );

    if (decrypt_result != PVF_SUCCESS) {
        return decrypt_result;
    }

    return PVF_SUCCESS;
}

/**
 * @brief 按路径提取文件（组合查找+提取）
 */
int PackSet_ExtractFileByPath(PackSetInternal* pack, const char* filepath,
                               void* output_buffer, size_t buffer_size,
                               uint32_t* actual_size) {
    PVFIndexEntry* entry;
    int result;

    /* ====================================================================== */
    /* Step 1: 查找文件                                                       */
    /* ====================================================================== */

    entry = PackSet_GetFile(pack, filepath);
    if (entry == NULL) {
        return PVF_ERROR_FILE_NOT_IN_PVF;
    }

    /* 返回实际大小 */
    if (actual_size != NULL) {
        *actual_size = entry->size;
    }

    /* ====================================================================== */
    /* Step 2: 提取文件                                                       */
    /* ====================================================================== */

    result = PackSet_ExtractFile(pack, entry, output_buffer, buffer_size);

    return result;
}
