#ifndef PVF_DATA_PARSER_H
#define PVF_DATA_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file pvf_data_parser.h
 * @brief PVF数据解析器 - 通用键值对解析
 *
 * @details 提供统一的接口来解析PVF中的二进制键值对数据
 *          支持.equ、.stk、.ani、.act等多种文件格式
 *
 * @version 1.0.0
 * @date 2025-11-25
 */

#include <stdint.h>
#include <stddef.h>

/* ========================================================================== */
/* 常量定义                                                                   */
/* ========================================================================== */

/**
 * @brief 文件头魔数
 */
#define PVF_DATA_MAGIC_NUMBER 0xD0B0  /* 小端序: 0xB0 0xD0 */

/**
 * @brief 值类型标记
 */
#define PVF_TYPE_KEY_VALUE_PAIR 0x05  /* 键值对 */
#define PVF_TYPE_INTEGER        0x02  /* 整数值 */
#define PVF_TYPE_REFERENCE      0x07  /* 引用/特殊值 */

/* ========================================================================== */
/* 数据结构                                                                   */
/* ========================================================================== */

/**
 * @brief 值类型枚举
 */
typedef enum {
    PVF_VALUE_TYPE_KV        = 0x05,  /* 键值对（嵌套） */
    PVF_VALUE_TYPE_INTEGER   = 0x02,  /* 整数 */
    PVF_VALUE_TYPE_REFERENCE = 0x07,  /* 引用 */
    PVF_VALUE_TYPE_UNKNOWN   = 0xFF   /* 未知类型 */
} PVFValueType;

/**
 * @brief 数据条目（单个键值对）
 */
typedef struct {
    uint32_t key;                     /* 键 (4字节) */
    PVFValueType type;                /* 值类型 */

    union {
        int32_t int_value;            /* 整数值 (type == INTEGER) */
        uint32_t ref_value;           /* 引用值 (type == REFERENCE) */
        struct {
            uint32_t sub_key;         /* 子键 (type == KV) */
            uint32_t sub_value;       /* 子值 */
        } kv;
    } value;
} PVFDataEntry;

/**
 * @brief 数据迭代器
 *
 * @details 用于顺序遍历文件中的所有键值对
 */
typedef struct {
    const unsigned char* data;        /* 原始数据指针 */
    size_t size;                      /* 数据总大小 */
    size_t position;                  /* 当前读取位置 */
    int has_magic;                    /* 是否检测到魔数 */
    int error;                        /* 错误标志 */
} PVFDataIterator;

/**
 * @brief 数据容器（解析结果）
 *
 * @details 存储解析后的所有键值对
 */
typedef struct {
    PVFDataEntry* entries;            /* 条目数组 */
    int count;                        /* 条目数量 */
    int capacity;                     /* 数组容量 */
} PVFDataContainer;

/* ========================================================================== */
/* 迭代器API                                                                  */
/* ========================================================================== */

/**
 * @brief 初始化迭代器
 *
 * @param iter 迭代器指针
 * @param data 数据缓冲区
 * @param size 数据大小
 * @return int PVF_SUCCESS / PVF_ERROR_*
 *
 * @note 自动检测并跳过魔数 (0xb0d0)
 */
int PVFDataIterator_Init(PVFDataIterator* iter, const void* data, size_t size);

/**
 * @brief 读取下一个条目
 *
 * @param iter 迭代器指针
 * @param entry 输出：条目数据
 * @return int PVF_SUCCESS / PVF_ERROR_*
 *
 * @note 调用前应先用 HasNext() 检查
 */
int PVFDataIterator_Next(PVFDataIterator* iter, PVFDataEntry* entry);

/**
 * @brief 检查是否还有数据
 *
 * @param iter 迭代器指针
 * @return int 1=有, 0=无
 */
int PVFDataIterator_HasNext(const PVFDataIterator* iter);

/**
 * @brief 重置迭代器到起始位置
 *
 * @param iter 迭代器指针
 */
void PVFDataIterator_Reset(PVFDataIterator* iter);

/**
 * @brief 获取当前位置
 *
 * @param iter 迭代器指针
 * @return size_t 当前字节偏移量
 */
size_t PVFDataIterator_GetPosition(const PVFDataIterator* iter);

/* ========================================================================== */
/* 容器API                                                                    */
/* ========================================================================== */

/**
 * @brief 初始化容器
 *
 * @param container 容器指针
 * @param initial_capacity 初始容量
 * @return int PVF_SUCCESS / PVF_ERROR_MEMORY
 */
int PVFDataContainer_Init(PVFDataContainer* container, int initial_capacity);

/**
 * @brief 从数据解析并填充容器
 *
 * @param container 容器指针
 * @param data 数据缓冲区
 * @param size 数据大小
 * @return int PVF_SUCCESS / PVF_ERROR_*
 */
int PVFDataContainer_Parse(PVFDataContainer* container,
                            const void* data, size_t size);

/**
 * @brief 添加条目到容器
 *
 * @param container 容器指针
 * @param entry 条目数据
 * @return int PVF_SUCCESS / PVF_ERROR_MEMORY
 */
int PVFDataContainer_Add(PVFDataContainer* container,
                         const PVFDataEntry* entry);

/**
 * @brief 根据键查找条目
 *
 * @param container 容器指针
 * @param key 键值
 * @return PVFDataEntry* 找到的条目，NULL表示未找到
 */
PVFDataEntry* PVFDataContainer_Find(const PVFDataContainer* container,
                                     uint32_t key);

/**
 * @brief 获取整数值（便捷函数）
 *
 * @param container 容器指针
 * @param key 键值
 * @param default_value 默认值（未找到时返回）
 * @return int32_t 整数值
 */
int32_t PVFDataContainer_GetInt(const PVFDataContainer* container,
                                 uint32_t key, int32_t default_value);

/**
 * @brief 获取引用值（便捷函数）
 *
 * @param container 容器指针
 * @param key 键值
 * @param default_value 默认值（未找到时返回）
 * @return uint32_t 引用值
 */
uint32_t PVFDataContainer_GetRef(const PVFDataContainer* container,
                                  uint32_t key, uint32_t default_value);

/**
 * @brief 释放容器
 *
 * @param container 容器指针
 */
void PVFDataContainer_Free(PVFDataContainer* container);

/* ========================================================================== */
/* 实用函数                                                                   */
/* ========================================================================== */

/**
 * @brief 验证原始数据格式
 *
 * @param data 数据缓冲区
 * @param size 数据大小
 * @return int 1=有效, 0=无效
 */
int PVFDataRaw_Validate(const void* data, size_t size);

/**
 * @brief 检查魔数
 *
 * @param data 数据缓冲区
 * @param size 数据大小
 * @return int 1=有魔数, 0=无
 */
int PVFData_HasMagic(const void* data, size_t size);

/**
 * @brief 统计键值对数量（快速估算）
 *
 * @param data 数据缓冲区
 * @param size 数据大小
 * @return int 估计的条目数量
 */
int PVFData_EstimateCount(const void* data, size_t size);

/**
 * @brief 类型名称（用于调试）
 *
 * @param type 类型值
 * @return const char* 类型名称字符串
 */
const char* PVFData_TypeName(PVFValueType type);

#ifdef __cplusplus
}
#endif

#endif /* PVF_DATA_PARSER_H */
