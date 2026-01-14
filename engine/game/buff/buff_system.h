/**
 * @file buff_system.h
 * @brief Buff/Debuff system
 *
 * Manages buffs, debuffs, DOT effects, and stat modifiers.
 */

#ifndef ENGINE_GAME_BUFF_BUFF_SYSTEM_H
#define ENGINE_GAME_BUFF_BUFF_SYSTEM_H

#include "../../core/types.h"
#include "../entity/entity.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BUFF_MAX_DEFINITIONS    256
#define BUFF_TICK_INTERVAL      1000    /* 1 second */

/* Buff categories */
typedef enum {
    BUFF_CAT_NONE = 0,
    BUFF_CAT_BUFF,          /* Positive effect */
    BUFF_CAT_DEBUFF,        /* Negative effect */
    BUFF_CAT_DOT,           /* Damage over time */
    BUFF_CAT_HOT,           /* Heal over time */
    BUFF_CAT_CROWD_CONTROL, /* CC effects */
    BUFF_CAT_MAX
} BuffCategory;

/* Buff effect types */
typedef enum {
    BUFF_EFFECT_NONE = 0,
    /* Stat modifiers */
    BUFF_EFFECT_STRENGTH,
    BUFF_EFFECT_INTELLIGENCE,
    BUFF_EFFECT_VITALITY,
    BUFF_EFFECT_SPIRIT,
    BUFF_EFFECT_PHYS_ATTACK,
    BUFF_EFFECT_MAG_ATTACK,
    BUFF_EFFECT_PHYS_DEFENSE,
    BUFF_EFFECT_MAG_DEFENSE,
    BUFF_EFFECT_HIT_RATE,
    BUFF_EFFECT_EVASION,
    BUFF_EFFECT_CRIT_RATE,
    BUFF_EFFECT_CRIT_DAMAGE,
    BUFF_EFFECT_ATTACK_SPEED,
    BUFF_EFFECT_MOVE_SPEED,
    /* DOT/HOT */
    BUFF_EFFECT_HP_REGEN,
    BUFF_EFFECT_MP_REGEN,
    BUFF_EFFECT_HP_DRAIN,
    BUFF_EFFECT_MP_DRAIN,
    /* Crowd control */
    BUFF_EFFECT_STUN,
    BUFF_EFFECT_SILENCE,
    BUFF_EFFECT_SLOW,
    BUFF_EFFECT_ROOT,
    BUFF_EFFECT_FEAR,
    BUFF_EFFECT_SLEEP,
    /* Special */
    BUFF_EFFECT_SHIELD,
    BUFF_EFFECT_INVINCIBLE,
    BUFF_EFFECT_INVISIBLE,
    BUFF_EFFECT_MAX
} BuffEffectType;

/* Buff modifier type */
typedef enum {
    BUFF_MOD_FLAT = 0,      /* Add flat value */
    BUFF_MOD_PERCENT        /* Add percentage */
} BuffModType;

/* Buff definition */
typedef struct {
    u16             buff_id;
    char            name[32];
    BuffCategory    category;
    BuffEffectType  effect_type;
    BuffModType     mod_type;
    i32             base_value;
    i32             value_per_stack;
    u8              max_stacks;
    u8              is_dispellable;
    u8              is_debuff;
    u16             icon_id;
} BuffDefinition;

/* Buff system context */
typedef struct {
    BuffDefinition  definitions[BUFF_MAX_DEFINITIONS];
    int             definition_count;
    u32             last_tick;
} BuffSystem;

/**
 * Initialize buff system
 * @param system Buff system instance
 * @return ENGINE_OK on success
 */
int buff_system_init(BuffSystem* system);

/**
 * Cleanup buff system
 * @param system Buff system instance
 */
void buff_system_cleanup(BuffSystem* system);

/**
 * Register buff definition
 * @param system Buff system
 * @param def    Buff definition
 * @return ENGINE_OK on success
 */
int buff_system_register(BuffSystem* system, const BuffDefinition* def);

/**
 * Get buff definition by ID
 * @param system  Buff system
 * @param buff_id Buff ID
 * @return Buff definition or NULL
 */
const BuffDefinition* buff_system_get_def(const BuffSystem* system, u16 buff_id);

/**
 * Apply buff effects to entity stats
 * @param system Buff system
 * @param entity Entity to update
 */
void buff_apply_stat_effects(const BuffSystem* system, Entity* entity);

/**
 * Process tick effects (DOT/HOT)
 * @param system       Buff system
 * @param entity       Entity to process
 * @param current_time Current time
 */
void buff_process_tick_effects(const BuffSystem* system, Entity* entity,
                               u32 current_time);

/**
 * Check if entity has a CC effect
 * @param system Buff system
 * @param entity Entity to check
 * @param effect Effect type to check
 * @return 1 if has effect, 0 otherwise
 */
int buff_has_cc_effect(const BuffSystem* system, const Entity* entity,
                       BuffEffectType effect);

/**
 * Dispel buffs from entity
 * @param entity     Entity
 * @param is_debuff  1 to dispel debuffs, 0 to dispel buffs
 * @param max_dispel Maximum number to dispel
 * @return Number of buffs dispelled
 */
int buff_dispel(Entity* entity, int is_debuff, int max_dispel);

/**
 * Get buff category name
 * @param category Buff category
 * @return Category name string
 */
const char* buff_category_name(BuffCategory category);

/**
 * Get buff effect name
 * @param effect Buff effect type
 * @return Effect name string
 */
const char* buff_effect_name(BuffEffectType effect);

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_GAME_BUFF_BUFF_SYSTEM_H */
