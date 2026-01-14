/**
 * @file pvf_parser.c
 * @brief PVF文件解析实现 (头部解析、索引解析)
 *
 * @details 实现PVF格式文件的头部解析和索引解析
 *          基于Python参考实现移植
 *
 * @source tools/pvf_parser_py2.py:50-106 (Python参考实现)
 * @date 2025-11-25 (Day 2 - 任务2.1)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pvf/pvf_parser.h"
#include "pvf/pvf_decrypt.h"
#include "pvf/pvf_crc32.h"

/* ========================================================================== */
/* 辅助函数                                                                    */
/* ========================================================================== */

/**
 * @brief 验证PVF头部GUID是否有效
 */
int PVF_ValidateGUID(const char* guid_string) {
    if (guid_string == NULL) {
        return 0;
    }

    /* 比较字符串是否匹配 */
    return (strcmp(guid_string, PVF_EXPECTED_GUID) == 0) ? 1 : 0;
}

/* ========================================================================== */
/* 头部解析                                                                    */
/* ========================================================================== */

/**
 * @brief 解析PVF文件头部
 *
 * @details 读取并解析PVF文件头部的所有字段
 *          Python参考: tools/pvf_parser_py2.py:50-106
 */
int PVF_ParseHeader(FILE* fp, PVFHeader* header, uint32_t* header_end) {
    uint32_t tag_length;
    char tag_string_buffer[128];  /* 足够大的缓冲区用于读取GUID */

    /* 参数验证 */
    if (fp == NULL || header == NULL) {
        return PVF_ERROR_INVALID_PARAM;
    }

    /* 初始化头部结构体 */
    memset(header, 0, sizeof(PVFHeader));

    /* ====================================================================== */
    /* Step 1: 读取GUID长度 (4字节)                                           */
    /* ====================================================================== */

    if (fread(&tag_length, sizeof(uint32_t), 1, fp) != 1) {
        return PVF_ERROR_READ_FAILED;
    }

    /* 验证GUID长度 */
    if (tag_length != PVF_GUID_LENGTH) {
        return PVF_ERROR_INVALID_FORMAT;
    }

    header->tag_length = tag_length;

    /* ====================================================================== */
    /* Step 2: 读取GUID字符串 (36字节)                                        */
    /* ====================================================================== */

    if (fread(tag_string_buffer, 1, tag_length, fp) != tag_length) {
        return PVF_ERROR_READ_FAILED;
    }

    /* Null终止字符串 */
    tag_string_buffer[tag_length] = '\0';

    /* 复制到头部结构体 */
    memcpy(header->tag_string, tag_string_buffer, tag_length + 1);

    /* 验证GUID */
    if (!PVF_ValidateGUID(header->tag_string)) {
        /* GUID不匹配，但不视为致命错误 */
        /* 打印警告信息后继续 */
        fprintf(stderr, "[!] 警告: GUID不匹配\n");
        fprintf(stderr, "   期望: %s\n", PVF_EXPECTED_GUID);
        fprintf(stderr, "   实际: %s\n", header->tag_string);
    }

    /* ====================================================================== */
    /* Step 3: 读取版本号 (4字节)                                             */
    /* ====================================================================== */

    if (fread(&header->revision, sizeof(uint32_t), 1, fp) != 1) {
        return PVF_ERROR_READ_FAILED;
    }

    /* ====================================================================== */
    /* Step 4: 读取索引区大小 (4字节)                                         */
    /* ====================================================================== */

    if (fread(&header->index_header_size, sizeof(uint32_t), 1, fp) != 1) {
        return PVF_ERROR_READ_FAILED;
    }

    /* 验证索引区大小合理性 */
    if (header->index_header_size == 0 || header->index_header_size > 100*1024*1024) {
        /* 索引区大小应该在0-100MB之间 */
        return PVF_ERROR_INVALID_FORMAT;
    }

    /* ====================================================================== */
    /* Step 5: 读取索引CRC32 (4字节)                                          */
    /* ====================================================================== */

    if (fread(&header->index_crc32, sizeof(uint32_t), 1, fp) != 1) {
        return PVF_ERROR_READ_FAILED;
    }

    /* ====================================================================== */
    /* Step 6: 读取文件数量 (4字节)                                           */
    /* ====================================================================== */

    if (fread(&header->index_count, sizeof(uint32_t), 1, fp) != 1) {
        return PVF_ERROR_READ_FAILED;
    }

    /* 验证文件数量合理性 */
    if (header->index_count == 0 || header->index_count > 1000000) {
        /* 文件数量应该在0-100万之间 */
        return PVF_ERROR_INVALID_FORMAT;
    }

    /* ====================================================================== */
    /* Step 7: 计算头部结束位置                                               */
    /* ====================================================================== */

    /*
     * 头部大小 = 4 (tag_length) + tag_length + 4 (revision) + 4 (index_size)
     *          + 4 (index_crc32) + 4 (index_count)
     *          = 4 + 36 + 4 + 4 + 4 + 4 = 56 字节
     */
    if (header_end != NULL) {
        *header_end = 4 + header->tag_length + 4 + 4 + 4 + 4;
    }

    return PVF_SUCCESS;
}

/* ========================================================================== */
/* 索引解析                                                                    */
/* ========================================================================== */

/**
 * @brief 解析单个索引条目
 *
 * @details 从解密后的索引缓冲区中解析单个条目
 *          Python参考: tools/pvf_parser_py2.py:195-220
 */
int PVF_ParseIndexEntry(const uint8_t* buffer, size_t* offset,
                        PVFIndexEntry* entry) {
    size_t pos = *offset;
    uint32_t hash_value;
    uint32_t name_length;
    uint32_t file_size;
    uint32_t decrypt_key;
    uint32_t file_offset;

    /* 参数验证 */
    if (buffer == NULL || offset == NULL || entry == NULL) {
        return PVF_ERROR_INVALID_PARAM;
    }

    /* 初始化条目 */
    memset(entry, 0, sizeof(PVFIndexEntry));

    /* ====================================================================== */
    /* 读取字段                                                               */
    /* ====================================================================== */

    /* hash_value (4字节) */
    memcpy(&hash_value, &buffer[pos], 4);
    pos += 4;

    /* name_length (4字节) */
    memcpy(&name_length, &buffer[pos], 4);
    pos += 4;

    /* 验证文件名长度 */
    if (name_length == 0 || name_length > 512) {
        return PVF_ERROR_INVALID_FORMAT;
    }

    /* 分配文件名内存 */
    entry->name = (char*)malloc(name_length + 1);
    if (entry->name == NULL) {
        return PVF_ERROR_MEMORY;
    }

    /* filename (variable length) */
    memcpy(entry->name, &buffer[pos], name_length);
    entry->name[name_length] = '\0';  /* Null终止 */
    pos += name_length;

    /* file_size (4字节) */
    memcpy(&file_size, &buffer[pos], 4);
    pos += 4;

    /* decrypt_key (4字节) */
    memcpy(&decrypt_key, &buffer[pos], 4);
    pos += 4;

    /* file_offset (4字节) */
    memcpy(&file_offset, &buffer[pos], 4);
    pos += 4;

    /* ====================================================================== */
    /* 填充条目结构体                                                         */
    /* ====================================================================== */

    entry->hash_value = hash_value;
    entry->name_length = name_length;
    entry->size = file_size;
    entry->decrypt_key = decrypt_key;
    entry->offset = file_offset;

    /* 更新偏移量 */
    *offset = pos;

    return PVF_SUCCESS;
}

/**
 * @brief 解析PVF索引区（包含解密）
 *
 * @details 读取、解密、解析整个索引区
 *          Python参考: tools/pvf_parser_py2.py:174-220
 */
int PVF_ParseIndex(FILE* fp, PackSetInternal* pack) {
    uint8_t* index_buffer = NULL;
    size_t index_size;
    int decrypt_result;
    size_t offset;
    uint32_t i;
    int result;

    /* 参数验证 */
    if (fp == NULL || pack == NULL) {
        return PVF_ERROR_INVALID_PARAM;
    }

    /* 验证头部已解析 */
    if (pack->header.index_count == 0) {
        return PVF_ERROR_INVALID_PARAM;
    }

    /* ====================================================================== */
    /* Step 1: 分配索引数据缓冲区                                             */
    /* ====================================================================== */

    index_size = pack->header.index_header_size;

    index_buffer = (uint8_t*)malloc(index_size);
    if (index_buffer == NULL) {
        return PVF_ERROR_MEMORY;
    }

    /* ====================================================================== */
    /* Step 2: 读取整个索引区                                                 */
    /* ====================================================================== */

    if (fread(index_buffer, 1, index_size, fp) != index_size) {
        free(index_buffer);
        return PVF_ERROR_READ_FAILED;
    }

    /* ====================================================================== */
    /* Step 3: 解密索引数据                                                   */
    /* ====================================================================== */

    /*
     * 调用Day 1实现的解密函数
     * crc_init = index_count (文件数量)
     * checksum = index_crc32 (期望CRC32值)
     * key = index_crc32 (解密密钥 = 索引CRC32值)
     *
     * 参考: tools/pvf_parser_py2.py:202-206
     */
    decrypt_result = PVF_DecryptIndex(
        index_buffer,
        index_size,
        pack->header.index_count,   /* CRC初始值 */
        pack->header.index_crc32,   /* 期望CRC32 */
        pack->header.index_crc32    /* 解密密钥 = index_crc32 */
    );

    if (decrypt_result != PVF_SUCCESS) {
        free(index_buffer);
        return decrypt_result;
    }

    /* ====================================================================== */
    /* Step 4: 分配索引数组                                                   */
    /* ====================================================================== */

    pack->indexes = (PVFIndexEntry*)malloc(
        sizeof(PVFIndexEntry) * pack->header.index_count
    );

    if (pack->indexes == NULL) {
        free(index_buffer);
        return PVF_ERROR_MEMORY;
    }

    /* ====================================================================== */
    /* Step 5: 解析所有索引条目                                               */
    /* ====================================================================== */

    offset = 0;

    for (i = 0; i < pack->header.index_count; i++) {
        result = PVF_ParseIndexEntry(index_buffer, &offset, &pack->indexes[i]);

        if (result != PVF_SUCCESS) {
            /* 解析失败，释放已分配的内存 */
            uint32_t j;
            for (j = 0; j < i; j++) {
                free(pack->indexes[j].name);
            }
            free(pack->indexes);
            free(index_buffer);
            return result;
        }
    }

    /* ====================================================================== */
    /* Step 6: 清理                                                           */
    /* ====================================================================== */

    free(index_buffer);

    return PVF_SUCCESS;
}
