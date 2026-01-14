/**
 * @file combat_engine.h
 * @brief Combat system engine
 *
 * Handles damage calculation, skill execution, and combat flow.
 */

#ifndef ENGINE_GAME_COMBAT_COMBAT_ENGINE_H
#define ENGINE_GAME_COMBAT_COMBAT_ENGINE_H

#include "../../core/types.h"
#include "../entity/entity.h"

#ifdef __cplusplus
extern "C" {
#endif

#define COMBAT_MAX_TARGETS      16
#define COMBAT_HIT_CHECK_RATE   100     /* Base hit rate */
#define COMBAT_CRIT_MULTIPLIER  150     /* 150% = 1.5x */

/* Skill types */
typedef enum {
    SKILL_TYPE_NONE = 0,
    SKILL_TYPE_ATTACK,          /* Direct damage */
    SKILL_TYPE_BUFF,            /* Self/ally buff */
    SKILL_TYPE_DEBUFF,          /* Enemy debuff */
    SKILL_TYPE_HEAL,            /* HP recovery */
    SKILL_TYPE_SUMMON,          /* Summon entity */
    SKILL_TYPE_PASSIVE,         /* Passive skill */
    SKILL_TYPE_MAX
} SkillType;

/* Skill target types */
typedef enum {
    SKILL_TARGET_NONE = 0,
    SKILL_TARGET_SELF,
    SKILL_TARGET_SINGLE,
    SKILL_TARGET_AOE_CIRCLE,
    SKILL_TARGET_AOE_CONE,
    SKILL_TARGET_AOE_LINE,
    SKILL_TARGET_ALL_ENEMIES,
    SKILL_TARGET_ALL_ALLIES,
    SKILL_TARGET_MAX
} SkillTargetType;

/* Skill definition */
typedef struct {
    u16             skill_id;
    u8              level;
    SkillType       type;
    SkillTargetType target_type;
    DamageType      damage_type;

    /* Base values */
    i32             base_damage;
    i32             damage_per_level;
    i32             base_healing;
    i32             healing_per_level;

    /* Scaling */
    i32             str_ratio;      /* Physical scaling (%) */
    i32             int_ratio;      /* Magical scaling (%) */

    /* AOE parameters */
    i32             range;          /* Attack range */
    i32             aoe_radius;     /* AOE radius */

    /* Timing */
    u32             cast_time_ms;
    u32             cooldown_ms;
    i32             mp_cost;

    /* Effects */
    u16             buff_id;        /* Associated buff */
    u32             buff_duration;

    /* Hit count */
    u8              hit_count;      /* Multi-hit skills */
    u8              can_crit;

    /* Prerequisites */
    u16             prereq_skill;
    u8              prereq_level;
} SkillDefinition;

/* Combat result */
typedef struct {
    u32     target_id;
    i32     damage;
    i32     healing;
    u8      is_crit;
    u8      is_miss;
    u8      is_dodge;
    u8      is_block;
} CombatResult;

/* Combat event for multi-target skills */
typedef struct {
    u32             source_id;
    u16             skill_id;
    int             result_count;
    CombatResult    results[COMBAT_MAX_TARGETS];
} CombatEvent;

/**
 * Calculate skill damage
 * @param source     Attacker entity
 * @param target     Target entity
 * @param skill      Skill definition
 * @param out_result Output combat result
 * @return ENGINE_OK on success
 */
int combat_calculate_damage(const Entity* source, const Entity* target,
                            const SkillDefinition* skill, CombatResult* out_result);

/**
 * Calculate healing
 * @param source    Healer entity
 * @param target    Target entity
 * @param skill     Skill definition
 * @param out_result Output combat result
 * @return ENGINE_OK on success
 */
int combat_calculate_healing(const Entity* source, const Entity* target,
                             const SkillDefinition* skill, CombatResult* out_result);

/**
 * Check if attack hits
 * @param source Attacker
 * @param target Target
 * @return 1 if hit, 0 if miss
 */
int combat_check_hit(const Entity* source, const Entity* target);

/**
 * Check if attack is critical
 * @param source Attacker
 * @return 1 if crit, 0 otherwise
 */
int combat_check_critical(const Entity* source);

/**
 * Apply combat result to target
 * @param target Target entity
 * @param result Combat result
 * @param source_id Attacker ID
 * @return ENGINE_OK on success
 */
int combat_apply_result(Entity* target, const CombatResult* result, u32 source_id);

/**
 * Execute skill on single target
 * @param source    Attacker entity
 * @param target    Target entity
 * @param skill     Skill definition
 * @param out_result Output result
 * @return ENGINE_OK on success
 */
int combat_execute_skill(Entity* source, Entity* target,
                         const SkillDefinition* skill, CombatResult* out_result);

/**
 * Execute AOE skill
 * @param source     Attacker entity
 * @param targets    Array of target entities
 * @param target_count Number of targets
 * @param skill      Skill definition
 * @param out_event  Output combat event
 * @return ENGINE_OK on success
 */
int combat_execute_aoe(Entity* source, Entity** targets, int target_count,
                       const SkillDefinition* skill, CombatEvent* out_event);

/**
 * Check if entity can use skill
 * @param entity Entity
 * @param skill  Skill definition
 * @return 1 if can use, 0 otherwise
 */
int combat_can_use_skill(const Entity* entity, const SkillDefinition* skill);

/**
 * Get damage type name
 * @param type Damage type
 * @return Type name string
 */
const char* combat_damage_type_name(DamageType type);

/**
 * Get skill type name
 * @param type Skill type
 * @return Type name string
 */
const char* combat_skill_type_name(SkillType type);

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_GAME_COMBAT_COMBAT_ENGINE_H */
