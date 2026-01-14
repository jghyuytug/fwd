#ifndef EQUIPMENT_PARSER_H
#define EQUIPMENT_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file equipment_parser.h
 * @brief 装备数据解析器 (.equ文件)
 *
 * @details 将PVF中的装备文件解析为结构化数据
 *
 * @version 1.0.0
 * @date 2025-11-25
 */

#include <stdint.h>
#include "pvf/pvf_data_parser.h"
#include "pvf/pvf_types.h"  /* For PackSetInternal */

/* ========================================================================== */
/* 装备键定义（根据分析结果）                                                */
/* ========================================================================== */

/* 基本信息 */
#define EQU_KEY_ITEM_ID          0x00002CA5  /* 物品ID (Ref) */
#define EQU_KEY_NAME_REF         0x0000482F  /* 名称引用 (Ref) */
#define EQU_KEY_GRADE            0x00004817  /* 品级/等级 */
#define EQU_KEY_RARITY           0x0000066C  /* 稀有度 */
#define EQU_KEY_MIN_LEVEL        0x0000481C  /* 最低等级 */
#define EQU_KEY_WEIGHT           0x00001D14  /* 重量 */

/* 四维属性 */
#define EQU_KEY_STRENGTH         0x00001265  /* 力量 (physical attack) */
#define EQU_KEY_INTELLIGENCE     0x00001D0C  /* 智力 (magical attack) */
#define EQU_KEY_STAMINA          0x00001D0D  /* 体力 (physical defense) */
#define EQU_KEY_SPIRIT           0x00000B2D  /* 精神 (magical defense) */

/* 生命魔法值 */
#define EQU_KEY_HP_MAX           0x0000139A  /* HP最大值 */
#define EQU_KEY_MP_MAX           0x0000139B  /* MP最大值 */

/* 防御属性 */
#define EQU_KEY_PHYSICAL_DEF     0x00001D0B  /* 物理防御 */
#define EQU_KEY_MAGICAL_DEF      0x0000139C  /* 魔法防御 */

/* 装备攻防（最小/最大双值） */
#define EQU_KEY_EQUIP_PHYS_DEF_MIN  0x00001D09  /* 装备物理防御（最小） */
#define EQU_KEY_EQUIP_PHYS_DEF_MAX  0x00001D0A  /* 装备物理防御（最大） */

/* 特殊属性 */
#define EQU_KEY_MP_REGEN         0x00001D11  /* MP恢复速度 */
#define EQU_KEY_HP_REGEN         0x00001D12  /* HP恢复速度 */
#define EQU_KEY_ANTI_EVIL        0x0000487A  /* 驱魔值 */
#define EQU_KEY_PHYSICAL_CRIT    0x00004961  /* 物理暴击 */
#define EQU_KEY_MAGICAL_CRIT     0x00004962  /* 魔法暴击 */

/* 速度属性 */
#define EQU_KEY_MOVE_SPEED       0x00004922  /* 移动速度 */
#define EQU_KEY_ATTACK_SPEED     0x0000118D  /* 攻击速度 */
#define EQU_KEY_CAST_SPEED       0x00001D13  /* 施放速度 */

/* 价格 */
#define EQU_KEY_REPAIR_PRICE     0x0000481E  /* 修理价格 */
#define EQU_KEY_SELL_PRICE       0x0000481F  /* 出售价格 */
#define EQU_KEY_PRICE            0x0000481D  /* 基础价格 */

/* 其他 */
#define EQU_KEY_COOL_TIME        0x0000482C  /* 冷却时间 */
#define EQU_KEY_INVENTORY_LIMIT  0x0000650C  /* 负重限制 */

/* ========================================================================== */
/* 装备数据结构                                                               */
/* ========================================================================== */

/**
 * @brief 装备数据
 */
typedef struct {
    /* 基本信息 */
    uint32_t item_id;                /* 物品ID (引用) */
    uint32_t name_ref;               /* 名称引用 */
    int32_t grade;                   /* 品级 */
    int32_t rarity;                  /* 稀有度 */
    int32_t min_level;               /* 最小使用等级 */
    int32_t weight;                  /* 重量 */

    /* 四维属性 */
    int32_t strength;                /* 力量 (physical attack) */
    int32_t intelligence;            /* 智力 (magical attack) */
    int32_t stamina;                 /* 体力 (physical defense) */
    int32_t spirit;                  /* 精神 (magical defense) */

    /* 生命魔法值 */
    int32_t hp_max;                  /* HP最大值 */
    int32_t mp_max;                  /* MP最大值 */

    /* 防御属性 */
    int32_t physical_defense;        /* 物理防御 */
    int32_t magical_defense;         /* 魔法防御 */

    /* 装备攻防（最小/最大） */
    int32_t equip_physical_def_min;  /* 装备物理防御（最小） */
    int32_t equip_physical_def_max;  /* 装备物理防御（最大） */

    /* 恢复速度 */
    int32_t mp_regen;                /* MP恢复速度 */
    int32_t hp_regen;                /* HP恢复速度 */

    /* 特殊属性 */
    int32_t anti_evil;               /* 驱魔值 */
    int32_t physical_crit;           /* 物理暴击 */
    int32_t magical_crit;            /* 魔法暴击 */

    /* 速度属性 */
    int32_t move_speed;              /* 移动速度 */
    int32_t attack_speed;            /* 攻击速度 */
    int32_t cast_speed;              /* 施放速度 */

    /* 价格 */
    int32_t repair_price;            /* 修理价格 */
    int32_t sell_price;              /* 出售价格 */
    int32_t price;                   /* 基础价格 */

    /* 其他 */
    int32_t cool_time;               /* 冷却时间 */
    int32_t inventory_limit;         /* 负重限制 */

    /* 原始数据 */
    PVFDataContainer raw_data;       /* 保留所有原始键值对 */
} EquipmentData;

/* ========================================================================== */
/* API函数                                                                    */
/* ========================================================================== */

/**
 * @brief 初始化装备数据结构
 *
 * @param eq 装备数据指针
 */
void Equipment_Init(EquipmentData* eq);

/**
 * @brief 从原始数据解析装备
 *
 * @param eq 输出：装备数据
 * @param data 原始数据
 * @param size 数据大小
 * @return int PVF_SUCCESS / PVF_ERROR_*
 */
int Equipment_Parse(EquipmentData* eq, const void* data, size_t size);

/**
 * @brief 从PVF直接解析装备（便捷函数）
 *
 * @param eq 输出：装备数据
 * @param pack PVF PackSet
 * @param filepath 装备文件路径
 * @return int PVF_SUCCESS / PVF_ERROR_*
 */
int Equipment_ParseFromPVF(EquipmentData* eq,
                            struct PackSetInternal* pack,
                            const char* filepath);

/**
 * @brief 释放装备数据
 *
 * @param eq 装备数据指针
 */
void Equipment_Free(EquipmentData* eq);

/**
 * @brief 打印装备信息（调试用）
 *
 * @param eq 装备数据指针
 */
void Equipment_Print(const EquipmentData* eq);

/**
 * @brief 获取原始键值
 *
 * @param eq 装备数据指针
 * @param key 键
 * @param default_value 默认值
 * @return int32_t 值
 */
int32_t Equipment_GetRawInt(const EquipmentData* eq, uint32_t key,
                             int32_t default_value);

#ifdef __cplusplus
}
#endif

#endif /* EQUIPMENT_PARSER_H */
