/**
 * Combat Damage Calculator - Header
 * Purpose: Calculate damage using game formulas
 *
 * Phase: 6.1 - Combat System Implementation
 * Created: 2025-11-23
 */

#ifndef SRC_COMBAT_COMBAT_DAMAGE_H_
#define SRC_COMBAT_COMBAT_DAMAGE_H_

#include "combat_data_loader.h"

/**
 * Internal damage calculation result (separate from public API)
 */
typedef struct {
    int final_damage;           // 最终伤害
    bool is_critical;           // 是否暴击
    bool is_evaded;             // 是否闪避
    int element_bonus;          // 元素加成
    float critical_multiplier;  // 暴击倍率
} CombatDamageResult;

/**
 * Initialize damage calculator
 * @return 0 on success
 */
int CombatDamage_Initialize(void);

/**
 * Calculate basic damage
 * @param attacker Attacker combat data
 * @param skill Skill data
 * @return Basic damage value
 */
int CombatDamage_CalculateBase(const CharacterCombatData* attacker, const SkillCombatData* skill);

/**
 * Apply attribute modifiers
 * @param base_damage Base damage
 * @param strength Attacker strength
 * @param intelligence Attacker intelligence
 * @return Modified damage
 */
int CombatDamage_ApplyAttributeModifiers(int base_damage, int strength, int intelligence);

/**
 * Apply defense reduction
 * @param damage Incoming damage
 * @param physical_defense Physical defense value
 * @param magical_defense Magical defense value
 * @param damage_type Damage type
 * @return Reduced damage
 */
int CombatDamage_ApplyDefense(int damage, int physical_defense, int magical_defense, int damage_type);

/**
 * Roll for critical hit
 * @param critical_rate Critical rate (%)
 * @param critical_damage Critical damage multiplier (%)
 * @param is_critical Output: whether critical hit
 * @return Critical multiplier
 */
float CombatDamage_RollCritical(int critical_rate, int critical_damage, bool* is_critical);

/**
 * Calculate final damage
 * @param attacker Attacker data
 * @param defender Defender data
 * @param skill Skill data
 * @param result Output: damage result
 * @return 0 on success
 */
int CombatDamage_CalculateFinal(
    const CharacterCombatData* attacker,
    const CharacterCombatData* defender,
    const SkillCombatData* skill,
    CombatDamageResult* result
);

/**
 * Cleanup damage calculator
 */
void CombatDamage_Cleanup(void);

#endif /* SRC_COMBAT_COMBAT_DAMAGE_H_ */
