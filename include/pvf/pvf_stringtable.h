#ifndef PVF_STRINGTABLE_H
#define PVF_STRINGTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file pvf_stringtable.h
 * @brief PVF StringTable 加载器
 *
 * @details StringTable 是 PVF 数据文件中键值对的字符串标签查找表。
 *          用于将二进制键（如 0x00004817）转换为可读标签（如 "[grade]"）。
 *
 * @note StringTable 格式：
 *       [0x00000000] count (uint32) = 933,886
 *       [0x00000004] indices[count] (uint32 × count)
 *       [0x0038fffc] string_data (null-terminated strings)
 *
 * @date 2025-11-25
 */

#include <stdint.h>
#include <stddef.h>

/* ========================================================================== */
/* 常量定义                                                                    */
/* ========================================================================== */

#define STRINGTABLE_MAX_TAG_LENGTH    128   /* 标签最大长度 */
#define STRINGTABLE_TAG_OFFSET        4     /* 标签在字符串中的偏移 */

/* 错误码 */
#define STRINGTABLE_SUCCESS           0
#define STRINGTABLE_ERROR_NULL_PTR    -1
#define STRINGTABLE_ERROR_FILE_OPEN   -2
#define STRINGTABLE_ERROR_FILE_READ   -3
#define STRINGTABLE_ERROR_OUT_OF_MEM  -4
#define STRINGTABLE_ERROR_INVALID     -5
#define STRINGTABLE_ERROR_NOT_LOADED  -6

/* ========================================================================== */
/* 数据结构                                                                    */
/* ========================================================================== */

/**
 * @brief StringTable 结构体
 */
typedef struct {
    uint32_t    count;          /* 字符串总数 */
    uint32_t*   indices;        /* 索引表（偏移数组） */
    char*       string_data;    /* 字符串数据区 */
    size_t      data_size;      /* 数据区大小 */
    int         loaded;         /* 是否已加载 */
} PVFStringTable;

/* ========================================================================== */
/* 核心API                                                                     */
/* ========================================================================== */

/**
 * @brief 初始化 StringTable 结构体
 *
 * @param st  StringTable 结构体指针
 */
void StringTable_Init(PVFStringTable* st);

/**
 * @brief 从文件加载 StringTable
 *
 * @param st        StringTable 结构体指针
 * @param filepath  stringtable.bin 文件路径
 *
 * @return int STRINGTABLE_SUCCESS=成功, 其他=错误码
 */
int StringTable_LoadFromFile(PVFStringTable* st, const char* filepath);

/**
 * @brief 从内存缓冲区加载 StringTable
 *
 * @param st      StringTable 结构体指针
 * @param buffer  内存缓冲区
 * @param size    缓冲区大小
 *
 * @return int STRINGTABLE_SUCCESS=成功, 其他=错误码
 */
int StringTable_LoadFromBuffer(PVFStringTable* st, const void* buffer, size_t size);

/**
 * @brief 释放 StringTable 占用的资源
 *
 * @param st  StringTable 结构体指针
 */
void StringTable_Free(PVFStringTable* st);

/* ========================================================================== */
/* 查询API                                                                     */
/* ========================================================================== */

/**
 * @brief 根据索引获取原始字符串
 *
 * @param st     StringTable 结构体指针
 * @param index  索引值
 *
 * @return const char* 字符串指针，失败返回 NULL
 *
 * @note 返回的字符串指针指向内部数据，不要释放
 */
const char* StringTable_GetString(const PVFStringTable* st, uint32_t index);

/**
 * @brief 根据索引获取标签名（从偏移+4处提取 [xxx] 格式）
 *
 * @param st         StringTable 结构体指针
 * @param index      索引值
 * @param tag_buf    输出缓冲区
 * @param buf_size   缓冲区大小
 *
 * @return int 成功返回标签长度，失败返回负数错误码
 *
 * @example
 *     char tag[128];
 *     StringTable_GetTag(&st, 0x00004817, tag, sizeof(tag));
 *     // tag = "[grade]"
 */
int StringTable_GetTag(const PVFStringTable* st, uint32_t index,
                       char* tag_buf, size_t buf_size);

/**
 * @brief 检查索引是否有效
 *
 * @param st     StringTable 结构体指针
 * @param index  索引值
 *
 * @return int 1=有效, 0=无效
 */
int StringTable_IsValidIndex(const PVFStringTable* st, uint32_t index);

/* ========================================================================== */
/* 便捷宏定义                                                                  */
/* ========================================================================== */

/**
 * @brief 常用标签索引（从 stringtable.bin 提取）
 */
#define TAG_NAME                    0x00002ca5  /* [name] */
#define TAG_GRADE                   0x00004817  /* [grade] */
#define TAG_RARITY                  0x0000066c  /* [rarity] */
#define TAG_MINIMUM_LEVEL           0x0000481c  /* [minimum level] */
#define TAG_PRICE                   0x0000481d  /* [price] */
#define TAG_REPAIR_PRICE            0x0000481e  /* [repair price] */
#define TAG_VALUE                   0x0000481f  /* [value] */
#define TAG_WEIGHT                  0x00001d14  /* [weight] */

/* 物理属性 */
#define TAG_PHYSICAL_ATTACK         0x00001265  /* [physical attack] */
#define TAG_PHYSICAL_DEFENSE        0x00001d0b  /* [physical defense] */
#define TAG_PHYSICAL_CRITICAL       0x00004961  /* [physical critical hit] */
#define TAG_EQUIP_PHYS_ATTACK       0x000013ba  /* [equipment physical attack] */
#define TAG_EQUIP_PHYS_DEFENSE      0x0000139a  /* [equipment physical defense] */

/* 魔法属性 */
#define TAG_MAGICAL_ATTACK          0x00001d0c  /* [magical attack] */
#define TAG_MAGICAL_DEFENSE         0x00001d0d  /* [magical defense] */
#define TAG_MAGICAL_CRITICAL        0x00004962  /* [magical critical hit] */
#define TAG_EQUIP_MAG_ATTACK        0x000013bb  /* [equipment magical attack] */
#define TAG_EQUIP_MAG_DEFENSE       0x0000139c  /* [equipment magical defense] */

/* 元素属性 */
#define TAG_FIRE_ATTACK             0x00004821  /* [fire attack] */
#define TAG_WATER_ATTACK            0x00004964  /* [water attack] */
#define TAG_LIGHT_ATTACK            0x00004853  /* [light attack] */
#define TAG_DARK_ATTACK             0x0000490d  /* [dark attack] */
#define TAG_FIRE_RESISTANCE         0x0000495e  /* [fire resistance] */
#define TAG_WATER_RESISTANCE        0x00004927  /* [water resistance] */
#define TAG_LIGHT_RESISTANCE        0x00001d0f  /* [light resistance] */
#define TAG_DARK_RESISTANCE         0x00001d0e  /* [dark resistance] */

/* 状态属性 */
#define TAG_HIT_RECOVERY            0x0000139b  /* [hit recovery] */
#define TAG_ALL_STATUS_RESIST       0x0000139d  /* [all active status resistance] */
#define TAG_ATTACK_SPEED            0x00000b2c  /* [attack speed] */
#define TAG_JUMP_SPEED              0x00001d15  /* [jump speed] */

/* 装备类型 */
#define TAG_EQUIPMENT_TYPE          0x00004826  /* [equipment type] */
#define TAG_ATTACH_TYPE             0x0000481a  /* [attach type] */
#define TAG_USABLE_JOB              0x00004818  /* [usable job] */
#define TAG_SEALING                 0x0000481b  /* [sealing] */
#define TAG_DURABILITY              0x000062d3  /* [durability] */
#define TAG_SUB_TYPE                0x000062d1  /* [sub type] */

/* 图像/资源 */
#define TAG_ICON                    0x00004822  /* [icon] */
#define TAG_FIELD_IMAGE             0x00004824  /* [field image] */

/* 说明文本 */
#define TAG_BASIC_EXPLAIN           0x0000482f  /* [basic explain] */
#define TAG_DETAIL_EXPLAIN          0x00004831  /* [detail explain] */
#define TAG_FLAVOR_TEXT             0x00004860  /* [flavor text] */

/* 技能相关 */
#define TAG_SKILL                   0x00000222  /* [skill] */
#define TAG_SKILL_END               0x00000223  /* [/skill] */
#define TAG_SKILL_DATA_UP           0x00000ec5  /* [skill data up] */
#define TAG_COOLTIME                0x000017a8  /* [cooltime] */

/* 其他 */
#define TAG_COOL_TIME               0x0000482c  /* [cool time] */
#define TAG_CREATION_RATE           0x00004820  /* [creation rate] */

#ifdef __cplusplus
}
#endif

#endif /* PVF_STRINGTABLE_H */
