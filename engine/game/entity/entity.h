/**
 * @file entity.h
 * @brief Game entity base system
 *
 * Common entity structure for players, monsters, NPCs.
 */

#ifndef ENGINE_GAME_ENTITY_ENTITY_H
#define ENGINE_GAME_ENTITY_ENTITY_H

#include "../../core/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ENTITY_NAME_MAX     32
#define ENTITY_MAX_BUFFS    32
#define ENTITY_MAX_SKILLS   64

/* Entity types */
typedef enum {
    ENTITY_TYPE_NONE = 0,
    ENTITY_TYPE_PLAYER,
    ENTITY_TYPE_MONSTER,
    ENTITY_TYPE_NPC,
    ENTITY_TYPE_PET,
    ENTITY_TYPE_PROJECTILE,
    ENTITY_TYPE_MAX
} EntityType;

/* Entity state */
typedef enum {
    ENTITY_STATE_NONE = 0,
    ENTITY_STATE_IDLE,
    ENTITY_STATE_MOVING,
    ENTITY_STATE_ATTACKING,
    ENTITY_STATE_CASTING,
    ENTITY_STATE_STUNNED,
    ENTITY_STATE_DEAD,
    ENTITY_STATE_MAX
} EntityState;

/* Damage types */
typedef enum {
    DAMAGE_TYPE_PHYSICAL = 0,
    DAMAGE_TYPE_MAGICAL,
    DAMAGE_TYPE_FIRE,
    DAMAGE_TYPE_ICE,
    DAMAGE_TYPE_LIGHT,
    DAMAGE_TYPE_DARK,
    DAMAGE_TYPE_PURE,
    DAMAGE_TYPE_MAX
} DamageType;

/* Combat stats */
typedef struct {
    i32     strength;
    i32     intelligence;
    i32     vitality;
    i32     spirit;
    i32     physical_attack;
    i32     magical_attack;
    i32     physical_defense;
    i32     magical_defense;
    i32     hit_rate;
    i32     evasion;
    i32     critical_rate;
    i32     critical_damage;
    i32     attack_speed;
    i32     move_speed;
} CombatStats;

/* Buff entry */
typedef struct {
    u16     buff_id;
    u16     skill_id;       /* Source skill */
    u32     source_id;      /* Caster entity ID */
    u32     expire_time;
    i32     value;          /* Effect value */
    u8      stacks;
    u8      type;           /* Buff type */
} BuffEntry;

/* Skill cooldown */
typedef struct {
    u16     skill_id;
    u32     expire_time;
} SkillCooldown;

/* Entity base structure */
typedef struct Entity {
    /* Identity */
    u32             id;
    EntityType      type;
    char            name[ENTITY_NAME_MAX];

    /* State */
    EntityState     state;
    u32             state_time;     /* Time entered current state */

    /* Position */
    u32             map_id;
    i32             pos_x;
    i32             pos_y;
    i32             pos_z;
    i16             dir;            /* Direction (0-360) */

    /* Vital stats */
    i32             hp;
    i32             max_hp;
    i32             mp;
    i32             max_mp;
    u8              level;
    u8              job;

    /* Combat */
    CombatStats     base_stats;     /* Base stats */
    CombatStats     bonus_stats;    /* Bonus from equipment/buffs */
    CombatStats     final_stats;    /* Calculated final stats */

    /* Buffs */
    BuffEntry       buffs[ENTITY_MAX_BUFFS];
    u8              buff_count;

    /* Skill cooldowns */
    SkillCooldown   cooldowns[ENTITY_MAX_SKILLS];
    u8              cooldown_count;

    /* Combat state */
    u32             target_id;      /* Current target */
    u32             last_attack;    /* Last attack time */
    u32             last_hit;       /* Last time hit */
    u32             combat_time;    /* Time in combat */

    /* Flags */
    u8              active;
    u8              invincible;
    u8              in_combat;

    /* Type-specific data pointer */
    void*           type_data;
} Entity;

/**
 * Initialize entity
 * @param entity Entity instance
 * @param type   Entity type
 * @return ENGINE_OK on success
 */
int entity_init(Entity* entity, EntityType type);

/**
 * Cleanup entity
 * @param entity Entity instance
 */
void entity_cleanup(Entity* entity);

/**
 * Reset entity to initial state
 * @param entity Entity instance
 */
void entity_reset(Entity* entity);

/**
 * Set entity state
 * @param entity Entity instance
 * @param state  New state
 */
void entity_set_state(Entity* entity, EntityState state);

/**
 * Check if entity is alive
 * @param entity Entity instance
 * @return 1 if alive, 0 otherwise
 */
int entity_is_alive(const Entity* entity);

/**
 * Check if entity can attack
 * @param entity Entity instance
 * @return 1 if can attack, 0 otherwise
 */
int entity_can_attack(const Entity* entity);

/**
 * Check if entity can move
 * @param entity Entity instance
 * @return 1 if can move, 0 otherwise
 */
int entity_can_move(const Entity* entity);

/**
 * Calculate final stats
 * @param entity Entity instance
 */
void entity_calculate_stats(Entity* entity);

/**
 * Apply damage to entity
 * @param entity     Entity instance
 * @param damage     Damage amount
 * @param damage_type Damage type
 * @param source_id  Attacker ID
 * @return Actual damage dealt
 */
i32 entity_take_damage(Entity* entity, i32 damage,
                       DamageType damage_type, u32 source_id);

/**
 * Heal entity
 * @param entity    Entity instance
 * @param amount    Heal amount
 * @param source_id Healer ID
 * @return Actual heal amount
 */
i32 entity_heal(Entity* entity, i32 amount, u32 source_id);

/**
 * Kill entity
 * @param entity    Entity instance
 * @param killer_id Killer entity ID
 */
void entity_die(Entity* entity, u32 killer_id);

/**
 * Resurrect entity
 * @param entity Entity instance
 * @param hp_percent HP percentage to restore
 */
void entity_resurrect(Entity* entity, i32 hp_percent);

/**
 * Add buff to entity
 * @param entity      Entity instance
 * @param buff_id     Buff ID
 * @param skill_id    Source skill
 * @param source_id   Caster ID
 * @param duration_ms Duration in milliseconds
 * @param value       Effect value
 * @return ENGINE_OK on success
 */
int entity_add_buff(Entity* entity, u16 buff_id, u16 skill_id,
                    u32 source_id, u32 duration_ms, i32 value);

/**
 * Remove buff from entity
 * @param entity  Entity instance
 * @param buff_id Buff ID to remove
 * @return ENGINE_OK on success
 */
int entity_remove_buff(Entity* entity, u16 buff_id);

/**
 * Check if entity has buff
 * @param entity  Entity instance
 * @param buff_id Buff ID
 * @return 1 if has buff, 0 otherwise
 */
int entity_has_buff(const Entity* entity, u16 buff_id);

/**
 * Update buff timers
 * @param entity       Entity instance
 * @param current_time Current time
 * @return Number of expired buffs removed
 */
int entity_update_buffs(Entity* entity, u32 current_time);

/**
 * Check if skill is on cooldown
 * @param entity   Entity instance
 * @param skill_id Skill ID
 * @return 1 if on cooldown, 0 otherwise
 */
int entity_skill_on_cooldown(const Entity* entity, u16 skill_id);

/**
 * Start skill cooldown
 * @param entity      Entity instance
 * @param skill_id    Skill ID
 * @param duration_ms Cooldown duration
 * @return ENGINE_OK on success
 */
int entity_start_cooldown(Entity* entity, u16 skill_id, u32 duration_ms);

/**
 * Update cooldown timers
 * @param entity       Entity instance
 * @param current_time Current time
 * @return Number of expired cooldowns removed
 */
int entity_update_cooldowns(Entity* entity, u32 current_time);

/**
 * Get distance to another entity
 * @param entity Entity instance
 * @param other  Other entity
 * @return Distance
 */
i32 entity_distance_to(const Entity* entity, const Entity* other);

/**
 * Check if in range of another entity
 * @param entity Entity instance
 * @param other  Other entity
 * @param range  Range to check
 * @return 1 if in range, 0 otherwise
 */
int entity_in_range(const Entity* entity, const Entity* other, i32 range);

/**
 * Get entity type name
 * @param type Entity type
 * @return Type name string
 */
const char* entity_type_name(EntityType type);

/**
 * Get entity state name
 * @param state Entity state
 * @return State name string
 */
const char* entity_state_name(EntityState state);

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_GAME_ENTITY_ENTITY_H */
