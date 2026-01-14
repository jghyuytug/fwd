/**
 * Combat Data Loader - Header
 * Purpose: Load combat-related data from database using ORM
 *
 * Phase: 6.1 - Combat System Implementation
 * Created: 2025-11-23
 */

#ifndef SRC_COMBAT_COMBAT_DATA_LOADER_H_
#define SRC_COMBAT_COMBAT_DATA_LOADER_H_

#include <common/defs.h>
#include <common/error_codes.h>
#include <database/headers/db_connection_manager.h>

/**
 * Character combat attributes (loaded from database)
 */
typedef struct {
    int character_id;
    int level;
    int job_class;

    // Basic attributes
    int strength;           // 力量
    int intelligence;       // 智力
    int vitality;           // 体力
    int spirit;             // 精神

    // Combat stats
    int physical_attack;    // 物理攻击力
    int magical_attack;     // 魔法攻击力
    int physical_defense;   // 物理防御
    int magical_defense;    // 魔法防御
    int hp;                 // 当前HP
    int mp;                 // 当前MP
    int max_hp;             // 最大HP
    int max_mp;             // 最大MP

    // Advanced stats
    int critical_rate;      // 暴击率 (%)
    int critical_damage;    // 暴击伤害倍率 (%)
    int attack_speed;       // 攻击速度
    int move_speed;         // 移动速度
    int evasion;            // 闪避率 (%)
    int accuracy;           // 命中率 (%)
} CharacterCombatData;

/**
 * Equipment combat bonus (loaded from database)
 */
typedef struct {
    int equipment_id;
    int slot_type;

    // Attribute bonuses
    int strength_bonus;
    int intelligence_bonus;
    int vitality_bonus;
    int spirit_bonus;

    // Combat stat bonuses
    int physical_attack_bonus;
    int magical_attack_bonus;
    int physical_defense_bonus;
    int magical_defense_bonus;

    // Special bonuses
    int critical_rate_bonus;
    int critical_damage_bonus;
    int element_type;       // 元素属性
    int element_bonus;      // 元素加成
} EquipmentCombatData;

/**
 * Skill combat data (loaded from database)
 */
typedef struct {
    int skill_id;
    int skill_level;
    int skill_type;         // 技能类型（物理/魔法）

    int base_damage;        // 基础伤害
    int damage_multiplier;  // 伤害倍率 (%)
    int cooldown;           // 冷却时间 (ms)
    int mp_cost;            // MP消耗
    int cast_time;          // 施法时间 (ms)

    int element_type;       // 元素属性
    int status_effect_id;   // 附加状态效果ID
    int status_effect_rate; // 状态效果触发率 (%)
} SkillCombatData;

/**
 * Initialize combat data loader
 * @param db_manager Database connection manager
 * @return 0 on success, error code on failure
 */
int CombatDataLoader_Initialize(DBConnectionManager* db_manager);

/**
 * Load character combat data from database
 * @param character_id Character ID
 * @param out_data Output: character combat data
 * @return 0 on success, error code on failure
 */
int CombatDataLoader_LoadCharacter(int character_id, CharacterCombatData* out_data);

/**
 * Load equipment combat data from database
 * @param character_id Character ID
 * @param out_equipment Array of equipment data (must be pre-allocated)
 * @param equipment_count Input: array size, Output: actual count
 * @return 0 on success, error code on failure
 */
int CombatDataLoader_LoadEquipment(int character_id, EquipmentCombatData* out_equipment, int* equipment_count);

/**
 * Load skill combat data from database
 * @param character_id Character ID
 * @param skill_id Skill ID
 * @param out_skill Output: skill combat data
 * @return 0 on success, error code on failure
 */
int CombatDataLoader_LoadSkill(int character_id, int skill_id, SkillCombatData* out_skill);

/**
 * Calculate total combat attributes (base + equipment + buffs)
 * @param base Base character attributes
 * @param equipment Array of equipment
 * @param equipment_count Equipment count
 * @param out_total Output: total attributes
 * @return 0 on success, error code on failure
 */
int CombatDataLoader_CalculateTotalAttributes(
    const CharacterCombatData* base,
    const EquipmentCombatData* equipment,
    int equipment_count,
    CharacterCombatData* out_total
);

/**
 * Cleanup combat data loader
 */
void CombatDataLoader_Cleanup(void);

#endif /* SRC_COMBAT_COMBAT_DATA_LOADER_H_ */
