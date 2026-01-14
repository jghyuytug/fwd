/**
 * @file monster_ai.h
 * @brief Monster AI system
 *
 * Handles monster behavior, pathfinding, aggro, and combat decisions.
 */

#ifndef ENGINE_GAME_AI_MONSTER_AI_H
#define ENGINE_GAME_AI_MONSTER_AI_H

#include "../../core/types.h"
#include "../entity/entity.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AI_MAX_AGGRO_LIST       16
#define AI_AGGRO_DECAY_RATE     10      /* Per second */
#define AI_LEASH_DISTANCE       1500    /* Return to spawn */
#define AI_ATTACK_RANGE         100     /* Default melee range */
#define AI_SIGHT_RANGE          500     /* Default vision range */
#define AI_UPDATE_INTERVAL      100     /* ms between AI updates */

/* AI behavior types */
typedef enum {
    AI_TYPE_NONE = 0,
    AI_TYPE_PASSIVE,        /* Never attacks first */
    AI_TYPE_NEUTRAL,        /* Attacks when hit */
    AI_TYPE_AGGRESSIVE,     /* Attacks on sight */
    AI_TYPE_GUARD,          /* Protects area */
    AI_TYPE_PATROL,         /* Walks patrol path */
    AI_TYPE_FOLLOW,         /* Follows target */
    AI_TYPE_FLEE,           /* Runs away when low HP */
    AI_TYPE_BOSS,           /* Boss behavior (phases) */
    AI_TYPE_SUPPORT,        /* Heals/buffs allies */
    AI_TYPE_RANGED,         /* Maintains distance */
    AI_TYPE_MAX
} AIType;

/* AI state */
typedef enum {
    AI_STATE_IDLE = 0,
    AI_STATE_PATROL,
    AI_STATE_CHASE,
    AI_STATE_ATTACK,
    AI_STATE_CAST,
    AI_STATE_FLEE,
    AI_STATE_RETURN,
    AI_STATE_DEAD,
    AI_STATE_MAX
} AIState;

/* Aggro entry */
typedef struct {
    u32     entity_id;
    i32     aggro;
    u32     last_update;
} AggroEntry;

/* Patrol point */
typedef struct {
    i32     x;
    i32     y;
    i32     z;
    u32     wait_time;      /* Time to wait at this point */
} PatrolPoint;

/* AI context for a monster */
typedef struct {
    Entity*         owner;
    AIType          type;
    AIState         state;

    /* Spawn position (for leash) */
    i32             spawn_x;
    i32             spawn_y;
    i32             spawn_z;

    /* Current target */
    u32             target_id;
    Entity*         target;

    /* Aggro list */
    AggroEntry      aggro_list[AI_MAX_AGGRO_LIST];
    int             aggro_count;

    /* Ranges */
    i32             sight_range;
    i32             attack_range;
    i32             leash_range;

    /* Patrol */
    PatrolPoint*    patrol_points;
    int             patrol_count;
    int             patrol_index;

    /* Timing */
    u32             last_update;
    u32             state_time;
    u32             attack_cooldown;
    u32             skill_cooldown;

    /* Boss specific */
    u8              phase;
    u8              max_phases;
    u8              phase_hp_thresholds[4];

    /* Flags */
    u8              can_flee;
    u8              flee_hp_percent;
    u8              is_social;          /* Calls for help */
    u8              has_special_skill;
} MonsterAI;

/**
 * Initialize monster AI
 * @param ai     AI context
 * @param owner  Owner entity
 * @param type   AI behavior type
 * @return ENGINE_OK on success
 */
int monster_ai_init(MonsterAI* ai, Entity* owner, AIType type);

/**
 * Cleanup monster AI
 * @param ai AI context
 */
void monster_ai_cleanup(MonsterAI* ai);

/**
 * Update AI (main tick)
 * @param ai           AI context
 * @param current_time Current time
 */
void monster_ai_update(MonsterAI* ai, u32 current_time);

/**
 * Add aggro to an entity
 * @param ai        AI context
 * @param entity_id Target entity ID
 * @param amount    Aggro amount
 */
void monster_ai_add_aggro(MonsterAI* ai, u32 entity_id, i32 amount);

/**
 * Remove entity from aggro list
 * @param ai        AI context
 * @param entity_id Target entity ID
 */
void monster_ai_remove_aggro(MonsterAI* ai, u32 entity_id);

/**
 * Clear all aggro
 * @param ai AI context
 */
void monster_ai_clear_aggro(MonsterAI* ai);

/**
 * Get highest aggro target
 * @param ai AI context
 * @return Entity ID with highest aggro, or 0
 */
u32 monster_ai_get_top_aggro(const MonsterAI* ai);

/**
 * Notify AI of damage taken
 * @param ai        AI context
 * @param attacker  Attacker entity
 * @param damage    Damage amount
 */
void monster_ai_on_damage(MonsterAI* ai, Entity* attacker, i32 damage);

/**
 * Notify AI of death
 * @param ai AI context
 */
void monster_ai_on_death(MonsterAI* ai);

/**
 * Set patrol path
 * @param ai     AI context
 * @param points Array of patrol points
 * @param count  Number of points
 * @return ENGINE_OK on success
 */
int monster_ai_set_patrol(MonsterAI* ai, const PatrolPoint* points, int count);

/**
 * Set AI target
 * @param ai     AI context
 * @param target Target entity
 */
void monster_ai_set_target(MonsterAI* ai, Entity* target);

/**
 * Check if should use skill
 * @param ai AI context
 * @return Skill ID to use, or 0
 */
u16 monster_ai_select_skill(const MonsterAI* ai);

/**
 * Get AI type name
 * @param type AI type
 * @return Type name string
 */
const char* monster_ai_type_name(AIType type);

/**
 * Get AI state name
 * @param state AI state
 * @return State name string
 */
const char* monster_ai_state_name(AIState state);

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_GAME_AI_MONSTER_AI_H */
