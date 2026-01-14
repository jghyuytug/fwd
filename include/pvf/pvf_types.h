#ifndef PVF_TYPES_H
#define PVF_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file pvf_types.h
 * @brief PVF文件格式核心数据结构定义
 *
 * @details 基于PVF逆向工程成果 (2025-11-10) 和验证结果 (2025-11-25)
 *          Script.pvf: DNF游戏服务器打包文件格式
 *
 * @source docs/pvf/PVF_FORMAT_SPECIFICATION.md
 * @source docs/generated/PVF_PARSER_VALIDATION_2025-11-25.md
 * @source tools/pvf_parser_py2.py (Python参考实现)
 *
 * @verified 2025-11-25 on vm131 (379,576 files, 30.2MB index)
 */

#include <stdint.h>
#include <stdio.h>

/* ========================================================================== */
/* 返回值枚举                                                                  */
/* ========================================================================== */

/**
 * @brief PVF操作返回值
 */
typedef enum {
    PVF_SUCCESS = 0,                     /**< 操作成功 */
    PVF_ERROR_FILE_NOT_FOUND = -1,       /**< 文件未找到 */
    PVF_ERROR_INVALID_HEADER = -2,       /**< 头部格式错误 */
    PVF_ERROR_DECRYPT_FAILED = -3,       /**< 解密失败 */
    PVF_ERROR_CRC_MISMATCH = -4,         /**< CRC32校验失败 */
    PVF_ERROR_OUT_OF_MEMORY = -5,        /**< 内存分配失败 */
    PVF_ERROR_MEMORY = -5,               /**< 内存分配失败（别名） */
    PVF_ERROR_INVALID_INDEX = -6,        /**< 索引格式错误 */
    PVF_ERROR_FILE_NOT_IN_PVF = -7,      /**< 文件不在PVF中 */
    PVF_ERROR_INVALID_PARAM = -8,        /**< 无效参数 */
    PVF_ERROR_INVALID_PARAMETER = -8,    /**< 无效参数（别名） */
    PVF_ERROR_INVALID_ARGUMENT = -8,     /**< 无效参数（别名2） */
    PVF_ERROR_READ_FAILED = -9,          /**< 文件读取失败 */
    PVF_ERROR_INVALID_FORMAT = -10,      /**< 数据格式错误 */
    PVF_ERROR_PARSE_FAILED = -11,        /**< 数据解析失败 */
    PVF_ERROR_EOF = -12                  /**< 到达文件末尾 */
} PVFResult;

/* ========================================================================== */
/* PVF文件头部结构                                                             */
/* ========================================================================== */

/**
 * @brief PVF文件头部 (56字节固定)
 *
 * @details 头部结构（明文，未加密）:
 *          Offset  Size  Field
 *          0x00    4     tag_length (GUID字符串长度, 固定36)
 *          0x04    36    tag_string (GUID)
 *          0x28    4     revision (版本号)
 *          0x2C    4     index_header_size (索引区大小)
 *          0x30    4     index_crc32 (索引CRC32校验和/解密密钥)
 *          0x34    4     index_count (文件数量)
 *          0x38    -     头部结束
 *
 * @example Script.pvf头部:
 *          tag_length = 36
 *          tag_string = "fa08bf71-4395-6a4b-a3e3-2617c9fee116"
 *          revision = 48984
 *          index_header_size = 31670772 (30.2 MB)
 *          index_crc32 = 0x6fe5d91b
 *          index_count = 379576
 */
typedef struct {
    uint32_t tag_length;           /**< GUID字符串长度 (固定36) */
    char tag_string[64];           /**< GUID字符串 (最大64字节，实际36) */
    uint32_t revision;             /**< 版本号 */
    uint32_t index_header_size;    /**< 索引区大小（字节） */
    uint32_t index_crc32;          /**< 索引CRC32校验和（也用作解密密钥） */
    uint32_t index_count;          /**< 文件数量 */
} PVFHeader;

/* ========================================================================== */
/* PVF索引条目结构                                                             */
/* ========================================================================== */

/**
 * @brief PVF索引条目 (变长结构: 20 + name_length 字节)
 *
 * @details 索引条目格式（解密后）:
 *          Offset  Size          Field
 *          0       4             hash (文件名DJB2哈希)
 *          4       4             name_length
 *          8       name_length   filename (UTF-8)
 *          8+N     4             file_size (实际大小)
 *          12+N    4             decrypt_key (文件解密密钥)
 *          16+N    4             file_offset (文件在数据区的偏移)
 *
 * @warning 字段顺序非常关键！必须是: size → decrypt_key → offset
 *          不是 offset → size → aligned_size！
 *
 * @example 索引条目示例:
 *          hash = 0x00003e43
 *          name_length = 63
 *          filename = "creature/2wu_gunner/prime/animation/run/02_prime_run_01_dodge.ani"
 *          file_size = 272
 *          decrypt_key = 0x9deef494
 *          file_offset = 0
 *
 * @verified 2025-11-25: 解析10个条目成功，偏移值连续，结构正确
 */
typedef struct {
    uint32_t hash_value;           /**< 文件名DJB2哈希值 */
    char* name;                    /**< 文件名 (UTF-8编码，动态分配) */
    uint32_t name_length;          /**< 文件名长度 */
    uint32_t size;                 /**< 文件实际大小（字节） */
    uint32_t decrypt_key;          /**< 文件解密密钥 (每个文件独立) */
    uint32_t offset;               /**< 文件在数据区的偏移（4字节对齐） */
} PVFIndexEntry;

/* ========================================================================== */
/* PackSet主结构                                                               */
/* ========================================================================== */

/**
 * @brief Hash表节点 (用于O(1)文件查找)
 */
typedef struct PVFHashNode {
    int index_id;                  /**< 索引条目ID */
    struct PVFHashNode* next;      /**< 冲突链表 */
} PVFHashNode;

/**
 * @brief PackSet实例 (PVF文件管理器)
 *
 * @details PackSet封装了PVF文件的所有访问逻辑:
 *          - 文件句柄
 *          - 头部信息
 *          - 索引数组 (379,576个条目)
 *          - Hash表 (256桶，O(1)查找)
 *
 * @note 此结构替换原有的stub实现 (packset_stub.h)
 *
 * @memory 内存占用估算 (379,576个文件):
 *         - PVFHeader: 72字节
 *         - PVFIndexEntry数组: 379576 * 32字节 = 11.6 MB
 *         - 文件名字符串: 平均60字节 * 379576 = 21.8 MB
 *         - Hash表: 256 * 4字节 = 1 KB
 *         - Hash节点: 379576 * 12字节 = 4.3 MB
 *         - 总计: ~37.7 MB
 */
typedef struct PackSetInternal {
    /* 兼容stub结构 */
    int dummy;                     /**< 占位字段（兼容packset_stub.h） */

    /* 文件句柄 */
    FILE* fp;                      /**< PVF文件句柄 */
    uint32_t header_end_offset;    /**< 头部结束位置 (固定56) */

    /* 头部信息 */
    PVFHeader header;              /**< PVF头部 */

    /* 索引数组 */
    PVFIndexEntry* indexes;        /**< 索引条目数组 */

    /* Hash表 (快速查找) */
    PVFHashNode** hash_table;      /**< Hash表 (256桶) */
    int hash_table_size;           /**< Hash表大小 (固定256) */
} PackSetInternal;

/**
 * @brief PackSet公开类型 (兼容packset_stub.h)
 */
typedef struct PackSet {
    int dummy;
} PackSet;

/* ========================================================================== */
/* 常量定义                                                                    */
/* ========================================================================== */

/**
 * @brief 预期的GUID (Script.pvf标识)
 */
#define PVF_EXPECTED_GUID "fa08bf71-4395-6a4b-a3e3-2617c9fee116"

/**
 * @brief PVF头部固定大小 (字节)
 */
#define PVF_HEADER_SIZE 56

/**
 * @brief Hash表桶数量 (2^8 = 256)
 */
#define PVF_HASH_TABLE_SIZE 256

/**
 * @brief 最大文件名长度
 */
#define PVF_MAX_FILENAME_LENGTH 512

/* ========================================================================== */
/* 辅助宏                                                                      */
/* ========================================================================== */

/**
 * @brief 将PackSet转换为内部结构
 */
#define PACKSET_INTERNAL(pack) ((PackSetInternal*)(pack))

#ifdef __cplusplus
}
#endif

#endif /* PVF_TYPES_H */
