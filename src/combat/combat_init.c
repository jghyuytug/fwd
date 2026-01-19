/**
 * Combat Module - Implementation
 *
 * Handles combat system, damage calculation, status effects, and buffs
 */

#include "combat_interface.h"
#include "combat_damage.h"
#include "combat_log.h"
#include <config/game_config.h>
#include <user/user_interface_v2.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/* Combat encounter structure */
typedef struct {
    int combat_id;
    unsigned int attacker_id;
    unsigned int defender_id;
    unsigned int start_time;
    int is_active;
} CombatEncounter;

/* Combat module global state */
static struct {
    int initialized;
    int max_active_combats;
    int max_status_effects;

    // Combat encounters
    CombatEncounter* encounters;
    int encounter_count;
    int next_combat_id;

    // Status effects
    StatusEffect* status_effects;
    int status_effect_count;
    int next_effect_id;

    // Buffs/Debuffs
    BuffInfo* buffs;
    int buff_count;
    int next_buff_id;

    // Statistics
    struct {
        unsigned int total_combats_started;
        unsigned int total_combats_ended;
        unsigned int total_damage_calculated;
        unsigned int total_status_effects_applied;
        unsigned int total_buffs_applied;
    } stats;
} g_combat_state = {0};

static void FillCombatDataFromCharacter(const Character* character, CharacterCombatData* out_data)
{
    memset(out_data, 0, sizeof(*out_data));
    if (!character) {
        return;
    }

    out_data->character_id = (int)character->character_id;
    out_data->level = (int)character->level;
    out_data->job_class = (int)character->job_class;

    out_data->strength = (int)character->attributes.strength;
    out_data->intelligence = (int)character->attributes.intelligence;
    out_data->vitality = (int)character->attributes.vitality;
    out_data->spirit = (int)character->attributes.spirit;

    out_data->physical_attack = (int)character->attributes.physical_attack;
    out_data->magical_attack = (int)character->attributes.magical_attack;
    out_data->physical_defense = (int)character->attributes.physical_defense;
    out_data->magical_defense = (int)character->attributes.magical_defense;

    out_data->hp = (int)character->attributes.current_hp;
    out_data->max_hp = (int)character->attributes.max_hp;
    out_data->mp = (int)character->attributes.current_mp;
    out_data->max_mp = (int)character->attributes.max_mp;

    out_data->move_speed = (int)character->attributes.move_speed;
    out_data->attack_speed = (int)character->attributes.attack_speed;

    /* Convert user-module floats to combat-engine integer percentages. */
    int critical_rate_pct = (int)(character->attributes.critical_rate * 100.0f);
    if (critical_rate_pct < 0) critical_rate_pct = 0;
    if (critical_rate_pct > 100) critical_rate_pct = 100;
    out_data->critical_rate = critical_rate_pct;

    float critical_multiplier = character->attributes.critical_damage;
    int critical_bonus_pct = (int)((critical_multiplier - 1.0f) * 100.0f);
    if (critical_bonus_pct < 0) critical_bonus_pct = 0;
    out_data->critical_damage = critical_bonus_pct;

    const CharacterGrowthConfig* growth = CombatConfig_GetCharacterGrowth(out_data->job_class);
    out_data->accuracy = growth ? growth->base_accuracy : 0;
    out_data->evasion = growth ? growth->base_evasion : 0;
}

/**
 * Initialize Combat Module
 */
int Combat_Initialize(int max_active_combats, int max_status_effects)
{
    if (g_combat_state.initialized) {
        fprintf(stderr, "[Combat] Module already initialized\n");
        return ERR_INVALID_STATE;
    }

    if (max_active_combats <= 0 || max_active_combats > 10000) {
        fprintf(stderr, "[Combat] Invalid max active combats: %d\n", max_active_combats);
        return ERR_INVALID_PARAMETER;
    }

    if (max_status_effects <= 0 || max_status_effects > 100000) {
        fprintf(stderr, "[Combat] Invalid max status effects: %d\n", max_status_effects);
        return ERR_INVALID_PARAMETER;
    }

    printf("[Combat] Initializing combat module...\n");

    // Allocate encounter array
    g_combat_state.encounters = (CombatEncounter*)calloc(max_active_combats, sizeof(CombatEncounter));
    if (!g_combat_state.encounters) {
        fprintf(stderr, "[Combat] Failed to allocate encounter array\n");
        return ERR_OUT_OF_MEMORY;
    }

    // Allocate status effects array
    g_combat_state.status_effects = (StatusEffect*)calloc(max_status_effects, sizeof(StatusEffect));
    if (!g_combat_state.status_effects) {
        fprintf(stderr, "[Combat] Failed to allocate status effects array\n");
        free(g_combat_state.encounters);
        return ERR_OUT_OF_MEMORY;
    }

    // Allocate buffs array
    g_combat_state.buffs = (BuffInfo*)calloc(max_status_effects, sizeof(BuffInfo));
    if (!g_combat_state.buffs) {
        fprintf(stderr, "[Combat] Failed to allocate buffs array\n");
        free(g_combat_state.status_effects);
        free(g_combat_state.encounters);
        return ERR_OUT_OF_MEMORY;
    }

    g_combat_state.max_active_combats = max_active_combats;
    g_combat_state.max_status_effects = max_status_effects;
    g_combat_state.encounter_count = 0;
    g_combat_state.status_effect_count = 0;
    g_combat_state.buff_count = 0;
    g_combat_state.next_combat_id = 1;
    g_combat_state.next_effect_id = 1;
    g_combat_state.next_buff_id = 1;
    g_combat_state.initialized = 1;

    /* Initialize combat subsystems (formula, logging). */
    if (CombatDamage_Initialize() != 0) {
        fprintf(stderr, "[Combat] Failed to initialize damage calculator\n");
        Combat_Cleanup();
        return ERR_FAILURE;
    }
    if (CombatLog_Initialize() != 0) {
        fprintf(stderr, "[Combat] Failed to initialize combat log\n");
        Combat_Cleanup();
        return ERR_FAILURE;
    }

    printf("[Combat] Module initialized. Max combats: %d, Max effects: %d\n",
           max_active_combats, max_status_effects);

    return ERR_SUCCESS;
}

/**
 * Cleanup Combat Module
 */
void Combat_Cleanup()
{
    if (!g_combat_state.initialized) {
        return;
    }

    printf("[Combat] Cleaning up combat module...\n");
    printf("[Combat] Statistics:\n");
    printf("  - Total Combats Started: %u\n", g_combat_state.stats.total_combats_started);
    printf("  - Total Combats Ended: %u\n", g_combat_state.stats.total_combats_ended);
    printf("  - Total Damage Calculations: %u\n", g_combat_state.stats.total_damage_calculated);
    printf("  - Total Status Effects Applied: %u\n", g_combat_state.stats.total_status_effects_applied);
    printf("  - Total Buffs Applied: %u\n", g_combat_state.stats.total_buffs_applied);
    printf("  - Active Encounters: %d\n", g_combat_state.encounter_count);
    printf("  - Active Status Effects: %d\n", g_combat_state.status_effect_count);
    printf("  - Active Buffs: %d\n", g_combat_state.buff_count);

    free(g_combat_state.buffs);
    free(g_combat_state.status_effects);
    free(g_combat_state.encounters);

    CombatLog_Cleanup();
    CombatDamage_Cleanup();

    memset(&g_combat_state, 0, sizeof(g_combat_state));

    printf("[Combat] Module cleanup complete\n");
}

/**
 * Calculate Damage (Modular combat engine)
 */
int Combat_CalculateDamage(unsigned int attacker_id,
                           unsigned int defender_id,
                           int base_damage,
                           DamageType damage_type,
                           DamageResult* result)
{
    if (!g_combat_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!result) {
        return ERR_INVALID_PARAMETER;
    }

    memset(result, 0, sizeof(*result));
    result->damage_type = damage_type;

    CharacterCombatData attacker_data;
    CharacterCombatData defender_data;
    memset(&attacker_data, 0, sizeof(attacker_data));
    memset(&defender_data, 0, sizeof(defender_data));

    Character* attacker_character = NULL;
    Character* defender_character = NULL;
    (void)User_GetCharacterByID((uint32_t)attacker_id, &attacker_character);
    (void)User_GetCharacterByID((uint32_t)defender_id, &defender_character);
    FillCombatDataFromCharacter(attacker_character, &attacker_data);
    FillCombatDataFromCharacter(defender_character, &defender_data);

    SkillCombatData skill = {0};
    skill.base_damage = base_damage;
    skill.damage_multiplier = 0;
    skill.skill_type = (damage_type == DAMAGE_TYPE_PHYSICAL) ? 0 : 1;

    CombatDamageResult dmg = {0};
    int ret = CombatDamage_CalculateFinal(&attacker_data, &defender_data, &skill, &dmg);
    if (ret != 0) {
        result->damage_amount = 0;
        result->is_missed = 1;
        return ERR_FAILURE;
    }

    result->damage_amount = dmg.final_damage;
    result->is_critical = dmg.is_critical ? 1 : 0;
    result->is_dodged = dmg.is_evaded ? 1 : 0;

    g_combat_state.stats.total_damage_calculated++;

    (void)CombatLog_LogAttack((int)attacker_id, (int)defender_id, 0, result->damage_amount, result->is_critical != 0);

    return ERR_SUCCESS;
}

/**
 * Apply Status Effect
 */
int Combat_ApplyStatusEffect(StatusEffect* effect)
{
    if (!g_combat_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!effect) {
        return ERR_INVALID_PARAMETER;
    }

    if (g_combat_state.status_effect_count >= g_combat_state.max_status_effects) {
        fprintf(stderr, "[Combat] Status effect limit reached\n");
        return ERR_OUT_OF_MEMORY;
    }

    int effect_id = g_combat_state.next_effect_id++;
    g_combat_state.status_effects[g_combat_state.status_effect_count] = *effect;
    g_combat_state.status_effects[g_combat_state.status_effect_count].start_time = (unsigned int)time(NULL);
    g_combat_state.status_effect_count++;

    g_combat_state.stats.total_status_effects_applied++;

    return effect_id;
}

/**
 * Remove Status Effect
 */
int Combat_RemoveStatusEffect(int effect_id)
{
    if (!g_combat_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    // Stub: Simple removal (would need proper index lookup in production)
    if (g_combat_state.status_effect_count > 0) {
        g_combat_state.status_effect_count--;
    }

    return ERR_SUCCESS;
}

/**
 * Apply Buff/Debuff
 */
int Combat_ApplyBuff(BuffInfo* buff)
{
    if (!g_combat_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!buff) {
        return ERR_INVALID_PARAMETER;
    }

    if (g_combat_state.buff_count >= g_combat_state.max_status_effects) {
        fprintf(stderr, "[Combat] Buff limit reached\n");
        return ERR_OUT_OF_MEMORY;
    }

    int buff_id = g_combat_state.next_buff_id++;
    g_combat_state.buffs[g_combat_state.buff_count] = *buff;
    g_combat_state.buffs[g_combat_state.buff_count].start_time = (unsigned int)time(NULL);
    g_combat_state.buff_count++;

    g_combat_state.stats.total_buffs_applied++;

    return buff_id;
}

/**
 * Remove Buff/Debuff
 */
int Combat_RemoveBuff(int buff_id)
{
    if (!g_combat_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    // Stub: Simple removal
    if (g_combat_state.buff_count > 0) {
        g_combat_state.buff_count--;
    }

    return ERR_SUCCESS;
}

/**
 * Update Combat State
 */
int Combat_Update(int delta_time_ms)
{
    if (!g_combat_state.initialized) {
        return 0;
    }

    // Stub: Would process status effects, buff expirations, etc.
    return 0;
}

/**
 * Get Combat Statistics
 */
int Combat_GetStatistics(unsigned int entity_id, CombatStats* stats)
{
    if (!g_combat_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!stats) {
        return ERR_INVALID_PARAMETER;
    }

    // Stub: Return zeros
    memset(stats, 0, sizeof(CombatStats));

    return ERR_SUCCESS;
}

/**
 * Reset Combat Statistics
 */
void Combat_ResetStatistics(unsigned int entity_id)
{
    // Stub
    (void)entity_id;
}

/**
 * Get Active Status Effects
 */
int Combat_GetActiveStatusEffects(unsigned int target_id,
                                  StatusEffect* effects,
                                  int max_effects)
{
    if (!g_combat_state.initialized) {
        return 0;
    }

    // Stub: Return count
    (void)target_id;
    (void)effects;
    (void)max_effects;

    return g_combat_state.status_effect_count;
}

/**
 * Get Active Buffs
 */
int Combat_GetActiveBuffs(unsigned int target_id,
                         BuffInfo* buffs,
                         int max_buffs)
{
    if (!g_combat_state.initialized) {
        return 0;
    }

    // Stub: Return count
    (void)target_id;
    (void)buffs;
    (void)max_buffs;

    return g_combat_state.buff_count;
}

/**
 * Start Combat Encounter
 */
int Combat_StartEncounter(unsigned int attacker_id, unsigned int defender_id)
{
    if (!g_combat_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (g_combat_state.encounter_count >= g_combat_state.max_active_combats) {
        fprintf(stderr, "[Combat] Combat limit reached\n");
        return ERR_OUT_OF_MEMORY;
    }

    int combat_id = g_combat_state.next_combat_id++;

    CombatEncounter* encounter = &g_combat_state.encounters[g_combat_state.encounter_count];
    encounter->combat_id = combat_id;
    encounter->attacker_id = attacker_id;
    encounter->defender_id = defender_id;
    encounter->start_time = (unsigned int)time(NULL);
    encounter->is_active = 1;

    g_combat_state.encounter_count++;
    g_combat_state.stats.total_combats_started++;

    return combat_id;
}

/**
 * End Combat Encounter
 */
int Combat_EndEncounter(int combat_id)
{
    if (!g_combat_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    // Stub: Simple removal
    if (g_combat_state.encounter_count > 0) {
        g_combat_state.encounter_count--;
        g_combat_state.stats.total_combats_ended++;
    }

    return ERR_SUCCESS;
}

/**
 * Validate Hit
 */
int Combat_ValidateHit(unsigned int attacker_id,
                       unsigned int defender_id,
                       int base_accuracy)
{
    if (!g_combat_state.initialized) {
        return 0;
    }

    (void)attacker_id;
    (void)defender_id;

    // Stub: Simple accuracy check
    int roll = rand() % 100;
    return (roll < base_accuracy) ? 1 : 0;
}
