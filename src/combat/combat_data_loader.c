/**
 * Combat Data Loader - Implementation
 * Purpose: Load combat-related data from database using ORM
 *
 * Phase: 6.1 - Combat System Implementation (ORM Integrated)
 * Created: 2025-11-23
 * Updated: 2025-11-23 - ORM Integration Complete
 * Updated: 2025-11-24 - Configuration abstraction (Stage 1)
 */

#include "combat_data_loader.h"
#include "../../include/config/game_config.h"
#include "../../src/orm/core_game/taiwan_cain/headers/charac_info_orm.h"
#include "../../src/orm/core_game/d_taiwan/headers/member_info_orm.h"
#include "../../src/orm/core_game/taiwan_cain/headers/charac_item_stat_orm.h"
#include "../../src/orm/extended_features/taiwan_cain_2nd/headers/skill_orm.h"
#include <string.h>
#include <stdio.h>

static DBConnectionManager* g_db_manager = NULL;

/**
 * Initialize combat data loader
 */
int CombatDataLoader_Initialize(DBConnectionManager* db_manager) {
    if (!db_manager) {
        return ERROR_INVALID_PARAMETER;
    }

    g_db_manager = db_manager;
    printf("[Combat] Data loader initialized\n");
    return 0;
}

/**
 * Load character combat data from database
 */
int CombatDataLoader_LoadCharacter(int character_id, CharacterCombatData* out_data) {
    if (!g_db_manager || !out_data) {
        return ERROR_INVALID_PARAMETER;
    }

    // Initialize output structure
    memset(out_data, 0, sizeof(CharacterCombatData));
    out_data->character_id = character_id;

    // Load character data from database using ORM
    CharacInfo charac;
    if (CharacInfo_GetByCharacNo(g_db_manager, character_id, &charac) != 0) {
        printf("[Combat] ERROR: Failed to load character %d from database\n", character_id);
        return ERROR_DATABASE_QUERY_FAILED;
    }

    // Map ORM data to combat data structure
    out_data->level = charac.lev;
    out_data->job_class = charac.job;

    // Direct combat attributes from database
    out_data->physical_attack = charac.phy_attack;
    out_data->magical_attack = charac.mag_attack;
    out_data->physical_defense = charac.phy_defense;
    out_data->magical_defense = charac.mag_defense;

    // HP/MP
    out_data->hp = charac.HP;
    out_data->max_hp = charac.maxHP;
    out_data->mp = 0;  // Not stored in CharacInfo, calculate separately
    out_data->max_mp = charac.maxMP;

    // Speed attributes
    out_data->move_speed = charac.move_speed;
    out_data->attack_speed = charac.attack_speed;

    // ✅ UPDATED (2025-11-24): Use configuration interface instead of hardcoded values
    // Load character growth configuration for this job class
    const CharacterGrowthConfig* growth = CombatConfig_GetCharacterGrowth(charac.job);

    // Calculate attributes based on configuration
    out_data->strength = growth->base_strength + (charac.lev * growth->strength_per_level);
    out_data->intelligence = growth->base_intelligence + (charac.lev * growth->intelligence_per_level);
    out_data->vitality = growth->base_vitality + (charac.lev * growth->vitality_per_level);
    out_data->spirit = growth->base_spirit + (charac.lev * growth->spirit_per_level);

    // Calculate combat modifiers based on configuration
    out_data->critical_rate = growth->base_critical_rate + (int)(charac.lev * growth->critical_rate_per_level);
    out_data->critical_damage = growth->base_critical_damage;
    out_data->evasion = growth->base_evasion;
    out_data->accuracy = growth->base_accuracy;

    printf("[Combat] Loaded character %d (Lv.%d, Job:%d) - HP:%d/%d, ATK:%d, DEF:%d\n",
        character_id, charac.lev, charac.job, charac.HP, charac.maxHP,
        charac.phy_attack, charac.phy_defense);

    return 0;
}

/**
 * Load equipment combat data from database
 */
int CombatDataLoader_LoadEquipment(int character_id, EquipmentCombatData* out_equipment, int* equipment_count) {
    if (!g_db_manager || !out_equipment || !equipment_count) {
        return ERROR_INVALID_PARAMETER;
    }

    int max_count = *equipment_count;
    *equipment_count = 0;

    // Load equipment data from database using ORM
    CharacItemStat item_stat;
    int ret = CharacItemStat_GetByCharacNo(g_db_manager, character_id, &item_stat);

    if (ret != 0) {
        // Character has no equipment data yet, return empty
        printf("[Combat] No equipment data found for character %d\n", character_id);
        return 0;  // Not an error, just no equipment
    }

    /*
     * CharacItemStat BLOB Parsing:
     *
     * The charac_item_stat table contains serialized binary data:
     * - cooltime_item: Item cooldown timers (format: [item_id:4][cooldown:4]...)
     * - effect_item: Active item effects (format: [effect_id:2][duration:4][power:2]...)
     * - check_flag: Item validation flags
     *
     * Production parsing requires:
     * 1. PVF item definition to map item_id -> equipment stats
     * 2. Binary protocol parser for the BLOB format
     * 3. Equipment slot mapper (weapon, armor, accessories)
     *
     * For now, using placeholder data until BLOB parser is implemented.
     */

    // Placeholder: Assume character has some basic equipment
    if (max_count > 0 && item_stat.cooltime_item_len > 0) {
        memset(&out_equipment[0], 0, sizeof(EquipmentCombatData));
        out_equipment[0].equipment_id = 1;  // Placeholder

        /*
         * Equipment bonus calculation placeholder:
         * Real implementation should parse BLOB data and look up item stats
         * from PVF (Script.pvf) item definitions.
         */
        int bonus_factor = (item_stat.cooltime_item_len / 100) + 1;
        out_equipment[0].physical_attack_bonus = 50 * bonus_factor;
        out_equipment[0].magical_attack_bonus = 30 * bonus_factor;
        out_equipment[0].strength_bonus = 10 * bonus_factor;
        out_equipment[0].intelligence_bonus = 10 * bonus_factor;
        out_equipment[0].physical_defense_bonus = 20 * bonus_factor;
        out_equipment[0].magical_defense_bonus = 20 * bonus_factor;

        *equipment_count = 1;

        printf("[Combat] Loaded equipment data for character %d (1 item, bonuses estimated)\n",
            character_id);
    } else {
        printf("[Combat] No valid equipment data for character %d\n", character_id);
    }

    return 0;
}

/**
 * Load skill combat data from database
 */
int CombatDataLoader_LoadSkill(int character_id, int skill_id, SkillCombatData* out_skill) {
    if (!g_db_manager || !out_skill) {
        return ERROR_INVALID_PARAMETER;
    }

    memset(out_skill, 0, sizeof(SkillCombatData));
    out_skill->skill_id = skill_id;

    // Load skill data from database using ORM
    Skill skill_data;
    int ret = Skill_Get(g_db_manager, character_id, &skill_data);

    if (ret != 0) {
        printf("[Combat] ERROR: Failed to load skill data for character %d\n", character_id);
        return ERROR_DATABASE_QUERY_FAILED;
    }

    // Note: Skill table contains SP (skill points) management
    // Actual skill levels and details are in separate tables
    // For now, derive skill parameters from available data

    // Estimate skill level based on used SP
    // (Rough approximation: 100 SP ≈ 10 skill levels)
    int total_sp = skill_data.used_sp;
    int estimated_skill_level = (total_sp / 100) + 1;
    if (estimated_skill_level > 60) estimated_skill_level = 60;  // Cap at max level

    out_skill->skill_level = estimated_skill_level;

    // ✅ UPDATED (2025-11-24): Use configuration interface instead of hardcoded formulas
    // Load skill configuration from centralized config
    const SkillConfig* skill_config = CombatConfig_GetSkill(skill_id, estimated_skill_level);

    out_skill->base_damage = skill_config->base_damage;
    out_skill->damage_multiplier = skill_config->damage_multiplier;
    out_skill->cooldown = skill_config->cooldown_ms;
    out_skill->mp_cost = skill_config->mp_cost;

    // Placeholder for status effects
    out_skill->status_effect_id = 0;  // No status effect by default
    out_skill->status_effect_chance = 0;

    printf("[Combat] Loaded skill %d for character %d (Lv.%d, DMG:%d, CD:%dms)\n",
        skill_id, character_id, estimated_skill_level,
        out_skill->base_damage, out_skill->cooldown);

    return 0;
}

/**
 * Calculate total combat attributes
 */
int CombatDataLoader_CalculateTotalAttributes(
    const CharacterCombatData* base,
    const EquipmentCombatData* equipment,
    int equipment_count,
    CharacterCombatData* out_total
) {
    if (!base || !out_total) {
        return ERROR_INVALID_PARAMETER;
    }

    // Start with base attributes
    memcpy(out_total, base, sizeof(CharacterCombatData));

    // Add equipment bonuses
    for (int i = 0; i < equipment_count; i++) {
        const EquipmentCombatData* eq = &equipment[i];

        out_total->strength += eq->strength_bonus;
        out_total->intelligence += eq->intelligence_bonus;
        out_total->vitality += eq->vitality_bonus;
        out_total->spirit += eq->spirit_bonus;

        out_total->physical_attack += eq->physical_attack_bonus;
        out_total->magical_attack += eq->magical_attack_bonus;
        out_total->physical_defense += eq->physical_defense_bonus;
        out_total->magical_defense += eq->magical_defense_bonus;

        out_total->critical_rate += eq->critical_rate_bonus;
        out_total->critical_damage += eq->critical_damage_bonus;
    }

    printf("[Combat] Calculated total attributes (base + %d equipment)\n", equipment_count);

    return 0;
}

/**
 * Cleanup combat data loader
 */
void CombatDataLoader_Cleanup(void) {
    g_db_manager = NULL;
    printf("[Combat] Data loader cleanup complete\n");
}
