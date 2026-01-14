/**
 * @file equipment_parser.c
 * @brief 装备数据解析器实现
 */

#include "pvf/parsers/equipment_parser.h"
#include "pvf/pvf.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ========================================================================== */
/* API实现                                                                    */
/* ========================================================================== */

void Equipment_Init(EquipmentData* eq) {
    if (eq == NULL) return;

    memset(eq, 0, sizeof(EquipmentData));

    /* 初始化容器 */
    eq->raw_data.entries = NULL;
    eq->raw_data.count = 0;
    eq->raw_data.capacity = 0;
}

int Equipment_Parse(EquipmentData* eq, const void* data, size_t size) {
    int result;

    if (eq == NULL || data == NULL) {
        return PVF_ERROR_INVALID_PARAMETER;
    }

    /* 初始化 */
    Equipment_Init(eq);

    /* 解析到容器 */
    result = PVFDataContainer_Parse(&eq->raw_data, data, size);
    if (result != PVF_SUCCESS) {
        return result;
    }

    /* 提取已知字段 - 只使用INTEGER类型的值 */
    PVFDataEntry* entry;

    /* 基本信息 */
    entry = PVFDataContainer_Find(&eq->raw_data, EQU_KEY_ITEM_ID);
    if (entry && entry->type == PVF_VALUE_TYPE_REFERENCE) {
        eq->item_id = entry->value.ref_value;
    }

    entry = PVFDataContainer_Find(&eq->raw_data, EQU_KEY_NAME_REF);
    if (entry && entry->type == PVF_VALUE_TYPE_REFERENCE) {
        eq->name_ref = entry->value.ref_value;
    }

    eq->grade = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_GRADE, 0);
    eq->rarity = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_RARITY, 0);
    eq->min_level = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_MIN_LEVEL, 0);
    eq->weight = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_WEIGHT, 0);

    /* 四维属性 */
    eq->strength = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_STRENGTH, 0);
    eq->intelligence = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_INTELLIGENCE, 0);
    eq->stamina = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_STAMINA, 0);
    eq->spirit = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_SPIRIT, 0);

    /* 生命魔法值 */
    eq->hp_max = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_HP_MAX, 0);
    eq->mp_max = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_MP_MAX, 0);

    /* 防御属性 */
    eq->physical_defense = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_PHYSICAL_DEF, 0);
    eq->magical_defense = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_MAGICAL_DEF, 0);

    /* 装备攻防 */
    eq->equip_physical_def_min = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_EQUIP_PHYS_DEF_MIN, 0);
    eq->equip_physical_def_max = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_EQUIP_PHYS_DEF_MAX, 0);

    /* 恢复速度 */
    eq->mp_regen = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_MP_REGEN, 0);
    eq->hp_regen = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_HP_REGEN, 0);

    /* 特殊属性 */
    eq->anti_evil = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_ANTI_EVIL, 0);
    eq->physical_crit = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_PHYSICAL_CRIT, 0);
    eq->magical_crit = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_MAGICAL_CRIT, 0);

    /* 速度属性 */
    eq->move_speed = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_MOVE_SPEED, 0);
    eq->attack_speed = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_ATTACK_SPEED, 0);
    eq->cast_speed = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_CAST_SPEED, 0);

    /* 价格 */
    eq->repair_price = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_REPAIR_PRICE, 0);
    eq->sell_price = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_SELL_PRICE, 0);
    eq->price = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_PRICE, 0);

    /* 其他 */
    eq->cool_time = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_COOL_TIME, 0);
    eq->inventory_limit = PVFDataContainer_GetInt(&eq->raw_data, EQU_KEY_INVENTORY_LIMIT, 0);

    return PVF_SUCCESS;
}

int Equipment_ParseFromPVF(EquipmentData* eq,
                            struct PackSetInternal* pack,
                            const char* filepath) {
    PVFIndexEntry* entry;
    unsigned char* buffer;
    int result;

    if (eq == NULL || pack == NULL || filepath == NULL) {
        return PVF_ERROR_INVALID_PARAMETER;
    }

    /* 查找文件 */
    entry = PackSet_GetFile(pack, filepath);
    if (entry == NULL) {
        return PVF_ERROR_FILE_NOT_FOUND;
    }

    /* 提取文件 */
    buffer = (unsigned char*)malloc(entry->size);
    if (buffer == NULL) {
        return PVF_ERROR_MEMORY;
    }

    result = PackSet_ExtractFile(pack, entry, buffer, entry->size);
    if (result != PVF_SUCCESS) {
        free(buffer);
        return result;
    }

    /* 解析 */
    result = Equipment_Parse(eq, buffer, entry->size);
    free(buffer);

    return result;
}

void Equipment_Free(EquipmentData* eq) {
    if (eq == NULL) return;

    PVFDataContainer_Free(&eq->raw_data);
    memset(eq, 0, sizeof(EquipmentData));
}

void Equipment_Print(const EquipmentData* eq) {
    if (eq == NULL) return;

    printf("========================================\n");
    printf("装备信息\n");
    printf("========================================\n\n");

    printf("基本信息:\n");
    printf("  物品ID:      0x%08x (%u)\n", eq->item_id, eq->item_id);
    printf("  名称引用:    0x%08x\n", eq->name_ref);
    printf("  品级:        %d\n", eq->grade);
    printf("  稀有度:      %d\n", eq->rarity);
    printf("  最小等级:    %d\n", eq->min_level);
    printf("  重量:        %d\n", eq->weight);

    printf("\n四维属性:\n");
    if (eq->strength != 0)
        printf("  力量:        %d\n", eq->strength);
    if (eq->intelligence != 0)
        printf("  智力:        %d\n", eq->intelligence);
    if (eq->stamina != 0)
        printf("  体力:        %d\n", eq->stamina);
    if (eq->spirit != 0)
        printf("  精神:        %d\n", eq->spirit);

    printf("\n生命魔法值:\n");
    if (eq->hp_max != 0)
        printf("  HP最大值:    %d\n", eq->hp_max);
    if (eq->mp_max != 0)
        printf("  MP最大值:    %d\n", eq->mp_max);

    printf("\n防御属性:\n");
    if (eq->physical_defense != 0)
        printf("  物理防御:    %d\n", eq->physical_defense);
    if (eq->magical_defense != 0)
        printf("  魔法防御:    %d\n", eq->magical_defense);

    printf("\n装备攻防:\n");
    if (eq->equip_physical_def_min != 0 || eq->equip_physical_def_max != 0)
        printf("  装备物防:    %d ~ %d\n",
               eq->equip_physical_def_min, eq->equip_physical_def_max);

    printf("\n恢复速度:\n");
    if (eq->mp_regen != 0)
        printf("  MP恢复:      %d\n", eq->mp_regen);
    if (eq->hp_regen != 0)
        printf("  HP恢复:      %d\n", eq->hp_regen);

    printf("\n特殊属性:\n");
    if (eq->anti_evil != 0)
        printf("  驱魔:        %d\n", eq->anti_evil);
    if (eq->physical_crit != 0)
        printf("  物理暴击:    %d\n", eq->physical_crit);
    if (eq->magical_crit != 0)
        printf("  魔法暴击:    %d\n", eq->magical_crit);

    printf("\n速度属性:\n");
    if (eq->move_speed != 0)
        printf("  移动速度:    %d\n", eq->move_speed);
    if (eq->attack_speed != 0)
        printf("  攻击速度:    %d\n", eq->attack_speed);
    if (eq->cast_speed != 0)
        printf("  施放速度:    %d\n", eq->cast_speed);

    printf("\n价格:\n");
    if (eq->repair_price != 0)
        printf("  修理价格:    %d 金币\n", eq->repair_price);
    if (eq->sell_price != 0)
        printf("  出售价格:    %d 金币\n", eq->sell_price);
    if (eq->price != 0)
        printf("  基础价格:    %d 金币\n", eq->price);

    printf("\n其他:\n");
    if (eq->cool_time != 0)
        printf("  冷却时间:    %d\n", eq->cool_time);
    if (eq->inventory_limit != 0)
        printf("  负重限制:    %d\n", eq->inventory_limit);

    printf("\n原始数据: %d 个键值对\n", eq->raw_data.count);
    printf("========================================\n");
}

int32_t Equipment_GetRawInt(const EquipmentData* eq, uint32_t key,
                             int32_t default_value) {
    PVFDataEntry* entry;

    if (eq == NULL) {
        return default_value;
    }

    entry = PVFDataContainer_Find(&eq->raw_data, key);
    if (entry == NULL) {
        return default_value;
    }

    switch (entry->type) {
        case PVF_VALUE_TYPE_INTEGER:
            return entry->value.int_value;

        case PVF_VALUE_TYPE_REFERENCE:
            /* 引用值作为整数返回 */
            return (int32_t)entry->value.ref_value;

        default:
            return default_value;
    }
}
