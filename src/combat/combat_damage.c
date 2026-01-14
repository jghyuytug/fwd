/**
 * Combat Damage Calculator - Implementation
 *
 * Phase: 6.1 - Combat System Implementation
 * Created: 2025-11-23
 * Updated: 2025-11-24 - Configuration abstraction (Stage 1)
 */

#include "combat_damage.h"
#include "../../include/config/game_config.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static bool g_initialized = false;

/**
 * Initialize damage calculator
 */
int CombatDamage_Initialize(void) {
    if (g_initialized) {
        return 0;
    }

    srand((unsigned int)time(NULL));
    g_initialized = true;
    printf("[Combat] Damage calculator initialized\n");
    return 0;
}

/**
 * Calculate basic damage
 */
int CombatDamage_CalculateBase(const CharacterCombatData* attacker, const SkillCombatData* skill) {
    if (!attacker || !skill) {
        return 0;
    }

    // Formula: Skill base * (1 + damage_multiplier/100)
    int damage = skill->base_damage;
    damage = (int)(damage * (1.0f + skill->damage_multiplier / 100.0f));

    return damage;
}

/**
 * Apply attribute modifiers
 */
int CombatDamage_ApplyAttributeModifiers(int base_damage, int strength, int intelligence) {
    // ✅ UPDATED (2025-11-24): Use configuration instead of hardcoded divisor
    const DamageFormulaConfig* formula = CombatConfig_GetDamageFormula();

    // Formula: base * (1 + (str + int) / divisor)
    float attribute_bonus = (strength + intelligence) / (float)formula->attribute_divisor;
    int modified_damage = (int)(base_damage * (1.0f + attribute_bonus));

    return modified_damage;
}

/**
 * Apply defense reduction
 */
int CombatDamage_ApplyDefense(int damage, int physical_defense, int magical_defense, int damage_type) {
    // ✅ UPDATED (2025-11-24): Use configuration instead of hardcoded constant
    const DamageFormulaConfig* formula = CombatConfig_GetDamageFormula();

    int defense = (damage_type == 0) ? physical_defense : magical_defense;

    // Formula: damage_reduced = damage * defense/(defense + constant)
    int reduction = (int)(damage * defense / (defense + (float)formula->defense_constant));
    int final_damage = damage - reduction;

    // Apply minimum damage rate from configuration
    int min_damage = (int)(damage * formula->min_damage_rate);
    if (min_damage < 1) min_damage = 1;

    return (final_damage > min_damage) ? final_damage : min_damage;
}

/**
 * Roll for critical hit
 */
float CombatDamage_RollCritical(int critical_rate, int critical_damage, bool* is_critical) {
    if (!is_critical) {
        return 1.0f;
    }

    // Roll random number 0-100
    int roll = rand() % 100;

    if (roll < critical_rate) {
        *is_critical = true;
        return 1.0f + (critical_damage / 100.0f);
    } else {
        *is_critical = false;
        return 1.0f;
    }
}

/**
 * Calculate final damage
 */
int CombatDamage_CalculateFinal(
    const CharacterCombatData* attacker,
    const CharacterCombatData* defender,
    const SkillCombatData* skill,
    CombatDamageResult* result
) {
    if (!attacker || !defender || !skill || !result) {
        return ERROR_INVALID_PARAMETER;
    }

    // Initialize result
    result->final_damage = 0;
    result->is_critical = false;
    result->is_evaded = false;
    result->element_bonus = 0;
    result->critical_multiplier = 1.0f;

    // Step 1: Calculate base damage
    int damage = CombatDamage_CalculateBase(attacker, skill);

    // Step 2: Apply attribute modifiers
    damage = CombatDamage_ApplyAttributeModifiers(damage, attacker->strength, attacker->intelligence);

    // Step 3: Apply defense
    damage = CombatDamage_ApplyDefense(damage, defender->physical_defense, defender->magical_defense, skill->skill_type);

    // Step 4: Roll for critical
    result->critical_multiplier = CombatDamage_RollCritical(
        attacker->critical_rate,
        attacker->critical_damage,
        &result->is_critical
    );
    damage = (int)(damage * result->critical_multiplier);

    // Step 5: Check evasion
    int evasion_roll = rand() % 100;
    if (evasion_roll < defender->evasion - attacker->accuracy) {
        result->is_evaded = true;
        result->final_damage = 0;
        return 0;
    }

    result->final_damage = damage;

    printf("[Combat] Damage calculated: %d %s\n",
        damage,
        result->is_critical ? "(CRITICAL)" : "");

    return 0;
}

/**
 * Cleanup damage calculator
 */
void CombatDamage_Cleanup(void) {
    g_initialized = false;
    printf("[Combat] Damage calculator cleanup complete\n");
}
