/**
 * Combat Module - Public Interface
 * Purpose: Game combat system, damage calculation, status effects, and buff management
 *
 * Components:
 * - Damage calculation engine
 * - Status effect management (bleeding, burn, freeze, etc.)
 * - Buff/Debuff system
 * - Combat state tracking
 * - Hit detection and validation
 */

#ifndef SRC_COMBAT_COMBAT_INTERFACE_H_
#define SRC_COMBAT_COMBAT_INTERFACE_H_

#include <common/defs.h>
#include <common/error_codes.h>

struct DBConnectionManager;

/**
 * Damage types in combat
 */
typedef enum {
    DAMAGE_TYPE_PHYSICAL = 0,
    DAMAGE_TYPE_MAGICAL = 1,
    DAMAGE_TYPE_FIRE = 2,
    DAMAGE_TYPE_ICE = 3,
    DAMAGE_TYPE_LIGHTNING = 4,
    DAMAGE_TYPE_POISON = 5,
    DAMAGE_TYPE_DARK = 6,
    DAMAGE_TYPE_LIGHT = 7,
    DAMAGE_TYPE_TRUE = 8      // Ignores defense
} DamageType;

/**
 * Status effect types
 */
typedef enum {
    STATUS_NONE = 0,
    STATUS_BLEEDING = 1,
    STATUS_BURN = 2,
    STATUS_POISON = 3,
    STATUS_FREEZE = 4,
    STATUS_STUN = 5,
    STATUS_SLOW = 6,
    STATUS_CURSE = 7,
    STATUS_BLIND = 8,
    STATUS_SILENCE = 9,
    STATUS_PETRIFY = 10
} StatusEffectType;

/**
 * Buff/Debuff categories
 */
typedef enum {
    BUFF_TYPE_STAT_BOOST = 0,
    BUFF_TYPE_DEFENSE_BOOST = 1,
    BUFF_TYPE_ATTACK_BOOST = 2,
    BUFF_TYPE_SPEED_BOOST = 3,
    BUFF_TYPE_RESIST_BOOST = 4,
    DEBUFF_TYPE_STAT_REDUCE = 5,
    DEBUFF_TYPE_DEFENSE_REDUCE = 6,
    DEBUFF_TYPE_ATTACK_REDUCE = 7,
    DEBUFF_TYPE_SPEED_REDUCE = 8
} BuffType;

/**
 * Combat statistics structure
 */
typedef struct {
    unsigned int total_damage_dealt;
    unsigned int total_damage_received;
    unsigned int total_hits;
    unsigned int total_misses;
    unsigned int total_crits;
    unsigned int total_blocks;
    unsigned int total_dodges;
    unsigned int total_kills;
    unsigned int total_deaths;
} CombatStats;

/**
 * Damage calculation result
 */
typedef struct {
    int damage_amount;
    DamageType damage_type;
    int is_critical;
    int is_blocked;
    int is_dodged;
    int is_missed;
} DamageResult;

/**
 * Status effect instance
 */
typedef struct {
    StatusEffectType effect_type;
    unsigned int caster_id;
    unsigned int target_id;
    int duration;            // Remaining duration (ms)
    int tick_interval;       // Damage/effect per tick
    int stack_count;         // Number of stacks
    unsigned int start_time;
} StatusEffect;

/**
 * Buff/Debuff instance
 */
typedef struct {
    BuffType buff_type;
    unsigned int caster_id;
    unsigned int target_id;
    int duration;            // Remaining duration (ms)
    int stat_modifier;       // % or flat value
    int is_percentage;       // 1 for %, 0 for flat
    unsigned int start_time;
} BuffInfo;

/**
 * Initialize Combat Module
 *
 * Sets up combat system, damage calculation engine
 *
 * Parameters:
 *   max_active_combats: Maximum simultaneous combat instances
 *   max_status_effects: Maximum active status effects
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Combat_Initialize(int max_active_combats, int max_status_effects);

/**
 * Cleanup Combat Module
 *
 * Releases combat system resources
 */
void Combat_Cleanup();

/**
 * Calculate Damage
 *
 * Computes damage based on attacker/defender stats
 *
 * Parameters:
 *   attacker_id: Attacking entity ID
 *   defender_id: Defending entity ID
 *   base_damage: Base damage value
 *   damage_type: Type of damage
 *   result: Output damage result structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Combat_CalculateDamage(unsigned int attacker_id,
                           unsigned int defender_id,
                           int base_damage,
                           DamageType damage_type,
                           DamageResult* result);

/**
 * Apply Status Effect
 *
 * Applies a status effect to target entity
 *
 * Parameters:
 *   effect: Status effect to apply
 *
 * Returns: Effect ID on success, negative error code on failure
 */
int Combat_ApplyStatusEffect(StatusEffect* effect);

/**
 * Remove Status Effect
 *
 * Removes a status effect from target
 *
 * Parameters:
 *   effect_id: ID of effect to remove
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Combat_RemoveStatusEffect(int effect_id);

/**
 * Apply Buff/Debuff
 *
 * Applies a buff or debuff to target entity
 *
 * Parameters:
 *   buff: Buff/debuff to apply
 *
 * Returns: Buff ID on success, negative error code on failure
 */
int Combat_ApplyBuff(BuffInfo* buff);

/**
 * Remove Buff/Debuff
 *
 * Removes a buff or debuff from target
 *
 * Parameters:
 *   buff_id: ID of buff to remove
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Combat_RemoveBuff(int buff_id);

/**
 * Update Combat State
 *
 * Processes combat tick, updates status effects, buffs
 *
 * Parameters:
 *   delta_time_ms: Time elapsed since last update (milliseconds)
 *
 * Returns: Number of combat events processed
 */
int Combat_Update(int delta_time_ms);

/**
 * Get Combat Statistics
 *
 * Retrieves combat statistics for an entity
 *
 * Parameters:
 *   entity_id: Entity ID
 *   stats: Output statistics structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Combat_GetStatistics(unsigned int entity_id, CombatStats* stats);

/**
 * Reset Combat Statistics
 *
 * Resets combat statistics for an entity
 *
 * Parameters:
 *   entity_id: Entity ID (0 for all)
 */
void Combat_ResetStatistics(unsigned int entity_id);

/**
 * Get Active Status Effects
 *
 * Retrieves list of active status effects on target
 *
 * Parameters:
 *   target_id: Target entity ID
 *   effects: Output array of status effects
 *   max_effects: Maximum effects to retrieve
 *
 * Returns: Number of active effects
 */
int Combat_GetActiveStatusEffects(unsigned int target_id,
                                  StatusEffect* effects,
                                  int max_effects);

/**
 * Get Active Buffs
 *
 * Retrieves list of active buffs/debuffs on target
 *
 * Parameters:
 *   target_id: Target entity ID
 *   buffs: Output array of buffs
 *   max_buffs: Maximum buffs to retrieve
 *
 * Returns: Number of active buffs
 */
int Combat_GetActiveBuffs(unsigned int target_id,
                         BuffInfo* buffs,
                         int max_buffs);

/**
 * Start Combat Encounter
 *
 * Initializes a combat encounter between entities
 *
 * Parameters:
 *   attacker_id: Attacking entity ID
 *   defender_id: Defending entity ID
 *
 * Returns: Combat instance ID on success, negative error code on failure
 */
int Combat_StartEncounter(unsigned int attacker_id, unsigned int defender_id);

/**
 * End Combat Encounter
 *
 * Ends a combat encounter and cleanup resources
 *
 * Parameters:
 *   combat_id: Combat instance ID
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Combat_EndEncounter(int combat_id);

/**
 * Validate Hit
 *
 * Determines if an attack hits the target (accuracy check)
 *
 * Parameters:
 *   attacker_id: Attacking entity ID
 *   defender_id: Defending entity ID
 *   base_accuracy: Base hit chance (0-100)
 *
 * Returns: 1 if hit, 0 if miss
 */
int Combat_ValidateHit(unsigned int attacker_id,
                       unsigned int defender_id,
                       int base_accuracy);

#endif // SRC_COMBAT_COMBAT_INTERFACE_H_
