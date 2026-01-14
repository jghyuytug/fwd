#ifndef PVF_PARSER_COMMON_H
#define PVF_PARSER_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file parser_common.h
 * @brief PVF数据解析器通用接口和基础结构
 *
 * @details 本模块定义了所有PVF数据解析器的统一接口，
 *          包括17种数据类型的通用属性和标准操作。
 *
 * @date 2025-11-25
 */

#include <stdint.h>
#include <stddef.h>
#include "pvf/pvf_types.h"
#include "pvf/pvf_data_parser.h"
#include "pvf/pvf_packset.h"

/* ========================================================================== */
/* 常量定义                                                                    */
/* ========================================================================== */

/* 字符串缓冲区大小 */
#define PVF_NAME_MAX_LENGTH       256
#define PVF_TYPE_MAX_LENGTH       64
#define PVF_EXPLAIN_MAX_LENGTH    1024

/* 数据类型枚举 */
typedef enum {
    PVF_DATA_TYPE_UNKNOWN = 0,
    PVF_DATA_TYPE_EQUIPMENT,       /* 装备 .equ */
    PVF_DATA_TYPE_STACKABLE,       /* 消耗品 .stk */
    PVF_DATA_TYPE_SKILL,           /* 技能 .skl */
    PVF_DATA_TYPE_MONSTER,         /* 怪物 .mob */
    PVF_DATA_TYPE_DUNGEON,         /* 地下城 .dgn */
    PVF_DATA_TYPE_NPC,             /* NPC .npc */
    PVF_DATA_TYPE_QUEST,           /* 任务 .qst */
    PVF_DATA_TYPE_SHOP,            /* 商店 .shp */
    PVF_DATA_TYPE_ANIMATION,       /* 动画 .ani */
    PVF_DATA_TYPE_ATTACK,          /* 攻击信息 .atk */
    PVF_DATA_TYPE_ACTION,          /* 动作 .act */
    PVF_DATA_TYPE_PASSIVE_OBJECT,  /* 被动物体 .obj */
    PVF_DATA_TYPE_CHARACTER,       /* 角色 .chr */
    PVF_DATA_TYPE_UI,              /* UI .ui */
    PVF_DATA_TYPE_AURA,            /* 光环 .ora */
    PVF_DATA_TYPE_PARTICLE,        /* 粒子 .ptl */
    PVF_DATA_TYPE_PET,             /* 宠物 .pet */
    PVF_DATA_TYPE_MAX
} PVFDataType;

/* ========================================================================== */
/* 通用数据结构                                                                */
/* ========================================================================== */

/**
 * @brief PVF数据通用结构
 *
 * @details 所有PVF数据类型的基础结构，包含通用属性。
 *          专用解析器通过组合方式扩展此结构。
 *
 * @note 内存布局：
 *       - 所有字符串使用固定长度数组（避免动态分配）
 *       - 整数类型使用int32_t（与PVF格式一致）
 *       - raw_data保存原始容器（用于未识别字段）
 */
typedef struct {
    /* ============ 元数据 ============ */
    PVFDataType data_type;            /* 数据类型枚举 */
    char type_name[PVF_TYPE_MAX_LENGTH]; /* 类型名称（如"equipment"） */
    char file_path[PVF_NAME_MAX_LENGTH]; /* 文件路径（用于调试） */

    /* ============ 基础信息 ============ */
    char name[PVF_NAME_MAX_LENGTH];   /* 数据名称（如"疾风之刃"） */
    uint32_t item_id;                 /* 数据ID（如果适用） */

    /* ============ 通用属性 ============ */
    int32_t grade;                    /* 品级 (0-100) */
    int32_t rarity;                   /* 稀有度 (0=普通, 1=高级, 2=稀有, 3=神器, 4=传说, 5=史诗) */
    int32_t min_level;                /* 最低等级要求 */
    int32_t price;                    /* 价格（金币） */
    int32_t weight;                   /* 重量（单位：g） */

    /* ============ 说明文本 ============ */
    char basic_explain[PVF_EXPLAIN_MAX_LENGTH];  /* 基本说明 */
    char detail_explain[PVF_EXPLAIN_MAX_LENGTH]; /* 详细说明 */

    /* ============ 原始数据 ============ */
    PVFDataContainer raw_data;        /* 原始PVF容器（保留未解析字段） */

    /* ============ 扩展数据指针 ============ */
    void* type_specific_data;         /* 类型特定数据（由子类分配） */
    size_t type_specific_size;        /* 类型特定数据大小 */

} PVFDataGeneric;

/* ========================================================================== */
/* 标准接口函数                                                                */
/* ========================================================================== */

/**
 * @brief 初始化通用数据结构
 *
 * @param data          PVFDataGeneric结构体指针
 * @param data_type     数据类型枚举
 *
 * @details 设置默认值，清空所有字段
 */
void PVFData_Init(PVFDataGeneric* data, PVFDataType data_type);

/**
 * @brief 从内存缓冲区解析数据
 *
 * @param data          输出：解析后的数据
 * @param buffer        输入：文件内容
 * @param size          缓冲区大小
 *
 * @return int PVF_SUCCESS=成功, 其他=错误码
 *
 * @details 通用解析流程:
 *          1. 解析PVFDataContainer
 *          2. 提取通用字段（name, grade, rarity等）
 *          3. 调用类型特定解析器（如果已注册）
 *
 * @note 此函数只解析通用字段，子类需实现类型特定解析
 */
int PVFData_Parse(PVFDataGeneric* data, const void* buffer, size_t size);

/**
 * @brief 从PVF文件中解析数据
 *
 * @param data          输出：解析后的数据
 * @param pack          PackSetInternal结构体指针
 * @param filepath      PVF内部文件路径
 *
 * @return int PVF_SUCCESS=成功, 其他=错误码
 *
 * @details 内部调用:
 *          1. PackSet_GetFile() 查找文件
 *          2. PackSet_ExtractFile() 提取内容
 *          3. PVFData_Parse() 解析数据
 */
int PVFData_ParseFromPVF(PVFDataGeneric* data, PackSetInternal* pack, const char* filepath);

/**
 * @brief 打印数据内容（调试用）
 *
 * @param data          PVFDataGeneric结构体指针
 *
 * @details 输出格式:
 *          ===== [type_name] =====
 *          Name: xxx
 *          Grade: xxx
 *          ...
 */
void PVFData_Print(const PVFDataGeneric* data);

/**
 * @brief 释放数据结构占用的资源
 *
 * @param data          PVFDataGeneric结构体指针
 *
 * @details 清理:
 *          1. 释放raw_data容器
 *          2. 释放type_specific_data（如果存在）
 *          3. 清空所有字段
 */
void PVFData_Free(PVFDataGeneric* data);

/**
 * @brief 将数据导出为JSON格式
 *
 * @param data          PVFDataGeneric结构体指针
 * @param json_buffer   输出缓冲区
 * @param buffer_size   缓冲区大小
 *
 * @return int 实际写入的字节数，-1=失败
 *
 * @details JSON格式:
 *          {
 *            "type": "equipment",
 *            "name": "疾风之刃",
 *            "grade": 65,
 *            "rarity": 5,
 *            ...
 *          }
 */
int PVFData_ToJSON(const PVFDataGeneric* data, char* json_buffer, size_t buffer_size);

/* ========================================================================== */
/* 工具函数                                                                    */
/* ========================================================================== */

/**
 * @brief 根据文件扩展名检测数据类型
 *
 * @param filepath      文件路径
 *
 * @return PVFDataType 数据类型枚举
 *
 * @example
 *     PVFDataType type = PVFData_DetectType("equipment/sword.equ");
 *     // 返回: PVF_DATA_TYPE_EQUIPMENT
 */
PVFDataType PVFData_DetectType(const char* filepath);

/**
 * @brief 获取数据类型名称
 *
 * @param data_type     数据类型枚举
 *
 * @return const char* 类型名称字符串
 *
 * @example
 *     const char* name = PVFData_GetTypeName(PVF_DATA_TYPE_EQUIPMENT);
 *     // 返回: "equipment"
 */
const char* PVFData_GetTypeName(PVFDataType data_type);

/**
 * @brief 获取稀有度名称（中文）
 *
 * @param rarity        稀有度值 (0-5)
 *
 * @return const char* 稀有度名称
 *
 * @example
 *     const char* name = PVFData_GetRarityName(5);
 *     // 返回: "史诗"
 */
const char* PVFData_GetRarityName(int32_t rarity);

/**
 * @brief 验证数据完整性
 *
 * @param data          PVFDataGeneric结构体指针
 *
 * @return int 1=有效, 0=无效
 *
 * @details 检查:
 *          - 必填字段是否存在
 *          - 数值范围是否合理
 *          - 字符串是否有效
 */
int PVFData_Validate(const PVFDataGeneric* data);

/* ========================================================================== */
/* 类型特定解析器注册机制                                                      */
/* ========================================================================== */

/**
 * @brief 类型特定解析器函数指针
 *
 * @param data          PVFDataGeneric结构体指针（已解析通用字段）
 *
 * @return int PVF_SUCCESS=成功, 其他=错误码
 */
typedef int (*TypeSpecificParser)(PVFDataGeneric* data);

/**
 * @brief 注册类型特定解析器
 *
 * @param data_type     数据类型枚举
 * @param parser        解析器函数指针
 *
 * @details 注册后，PVFData_Parse()会自动调用对应解析器
 *
 * @example
 *     PVFData_RegisterParser(PVF_DATA_TYPE_EQUIPMENT, Equipment_ParseSpecific);
 */
void PVFData_RegisterParser(PVFDataType data_type, TypeSpecificParser parser);

/* ========================================================================== */
/* StringTable 集成接口                                                        */
/* ========================================================================== */

/**
 * @brief 初始化 StringTable（从文件加载）
 *
 * @param filepath  stringtable.bin 文件路径
 *
 * @return int 0=成功, 其他=错误码
 *
 * @note 必须在解析任何 PVF 数据之前调用
 *
 * @example
 *     PVFData_InitStringTable("stringtable.bin");
 */
int PVFData_InitStringTable(const char* filepath);

/**
 * @brief 释放 StringTable
 *
 * @note 在程序退出前调用
 */
void PVFData_FreeStringTable(void);

/**
 * @brief 检查 StringTable 是否已加载
 *
 * @return int 1=已加载, 0=未加载
 */
int PVFData_IsStringTableLoaded(void);

/**
 * @brief 根据键获取标签名
 *
 * @param key       键值（stringtable 索引）
 * @param tag_buf   输出缓冲区
 * @param buf_size  缓冲区大小
 *
 * @return int 成功返回标签长度，失败返回负数
 *
 * @example
 *     char tag[128];
 *     PVFData_GetTagName(0x00004817, tag, sizeof(tag));
 *     // tag = "[grade]"
 */
int PVFData_GetTagName(uint32_t key, char* tag_buf, size_t buf_size);

/**
 * @brief 打印 PVF 数据的详细内容（带标签名解析）
 *
 * @param data  PVFDataGeneric 结构体指针
 *
 * @note 需要先调用 PVFData_InitStringTable() 加载 StringTable
 */
void PVFData_PrintDetailed(const PVFDataGeneric* data);

#ifdef __cplusplus
}
#endif

#endif /* PVF_PARSER_COMMON_H */
