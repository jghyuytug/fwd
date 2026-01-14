/**
 * @file entity.c
 * @brief Game entity base system implementation
 */

#include "entity.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/* Type names */
static const char* type_names[] = {
    "NONE",
    "PLAYER",
    "MONSTER",
    "NPC",
    "PET",
    "PROJECTILE"
};

/* State names */
static const char* state_names[] = {
    "NONE",
    "IDLE",
    "MOVING",
    "ATTACKING",
    "CASTING",
    "STUNNED",
    "DEAD"
};

static u32 get_time_ms(void)
{
    /* Simple time getter - should be replaced with actual implementation */
    return 0;
}

int entity_init(Entity* entity, EntityType type)
{
    if (!entity) {
        return ENGINE_ERROR_PARAM;
    }

    memset(entity, 0, sizeof(Entity));

    entity->id = 0;
    entity->type = type;
    entity->state = ENTITY_STATE_IDLE;
    entity->active = 1;

    /* Default stats */
    entity->max_hp = 100;
    entity->hp = 100;
    entity->max_mp = 50;
    entity->mp = 50;
    entity->level = 1;

    return ENGINE_OK;
}

void entity_cleanup(Entity* entity)
{
    if (!entity) return;

    /* Free type-specific data if any */
    if (entity->type_data) {
        free(entity->type_data);
        entity->type_data = NULL;
    }

    entity->active = 0;
}

void entity_reset(Entity* entity)
{
    if (!entity) return;

    entity->state = ENTITY_STATE_IDLE;
    entity->hp = entity->max_hp;
    entity->mp = entity->max_mp;
    entity->buff_count = 0;
    entity->cooldown_count = 0;
    entity->target_id = 0;
    entity->in_combat = 0;
    entity->invincible = 0;

    memset(entity->buffs, 0, sizeof(entity->buffs));
    memset(entity->cooldowns, 0, sizeof(entity->cooldowns));
}

void entity_set_state(Entity* entity, EntityState state)
{
    if (!entity) return;
    if (state >= ENTITY_STATE_MAX) return;

    entity->state = state;
    entity->state_time = get_time_ms();
}

int entity_is_alive(const Entity* entity)
{
    if (!entity) return 0;
    return entity->active && entity->state != ENTITY_STATE_DEAD && entity->hp > 0;
}

int entity_can_attack(const Entity* entity)
{
    if (!entity) return 0;
    if (!entity_is_alive(entity)) return 0;

    /* Cannot attack while stunned or casting */
    if (entity->state == ENTITY_STATE_STUNNED ||
        entity->state == ENTITY_STATE_CASTING) {
        return 0;
    }

    return 1;
}

int entity_can_move(const Entity* entity)
{
    if (!entity) return 0;
    if (!entity_is_alive(entity)) return 0;

    /* Cannot move while stunned, casting, or attacking */
    if (entity->state == ENTITY_STATE_STUNNED ||
        entity->state == ENTITY_STATE_CASTING ||
        entity->state == ENTITY_STATE_ATTACKING) {
        return 0;
    }

    return 1;
}

void entity_calculate_stats(Entity* entity)
{
    if (!entity) return;

    /* Final = Base + Bonus */
    entity->final_stats.strength = entity->base_stats.strength +
                                   entity->bonus_stats.strength;
    entity->final_stats.intelligence = entity->base_stats.intelligence +
                                       entity->bonus_stats.intelligence;
    entity->final_stats.vitality = entity->base_stats.vitality +
                                   entity->bonus_stats.vitality;
    entity->final_stats.spirit = entity->base_stats.spirit +
                                 entity->bonus_stats.spirit;

    entity->final_stats.physical_attack = entity->base_stats.physical_attack +
                                          entity->bonus_stats.physical_attack;
    entity->final_stats.magical_attack = entity->base_stats.magical_attack +
                                         entity->bonus_stats.magical_attack;
    entity->final_stats.physical_defense = entity->base_stats.physical_defense +
                                           entity->bonus_stats.physical_defense;
    entity->final_stats.magical_defense = entity->base_stats.magical_defense +
                                          entity->bonus_stats.magical_defense;

    entity->final_stats.hit_rate = entity->base_stats.hit_rate +
                                   entity->bonus_stats.hit_rate;
    entity->final_stats.evasion = entity->base_stats.evasion +
                                  entity->bonus_stats.evasion;
    entity->final_stats.critical_rate = entity->base_stats.critical_rate +
                                        entity->bonus_stats.critical_rate;
    entity->final_stats.critical_damage = entity->base_stats.critical_damage +
                                          entity->bonus_stats.critical_damage;

    entity->final_stats.attack_speed = entity->base_stats.attack_speed +
                                       entity->bonus_stats.attack_speed;
    entity->final_stats.move_speed = entity->base_stats.move_speed +
                                     entity->bonus_stats.move_speed;
}

i32 entity_take_damage(Entity* entity, i32 damage,
                       DamageType damage_type, u32 source_id)
{
    i32 actual_damage;
    i32 defense;

    if (!entity) return 0;
    if (!entity_is_alive(entity)) return 0;
    if (entity->invincible) return 0;
    if (damage <= 0) return 0;

    /* Get appropriate defense */
    if (damage_type == DAMAGE_TYPE_PHYSICAL) {
        defense = entity->final_stats.physical_defense;
    } else if (damage_type == DAMAGE_TYPE_MAGICAL ||
               damage_type == DAMAGE_TYPE_FIRE ||
               damage_type == DAMAGE_TYPE_ICE ||
               damage_type == DAMAGE_TYPE_LIGHT ||
               damage_type == DAMAGE_TYPE_DARK) {
        defense = entity->final_stats.magical_defense;
    } else {
        /* Pure damage ignores defense */
        defense = 0;
    }

    /* Defense reduction formula: damage * 100 / (100 + defense) */
    if (defense > 0) {
        actual_damage = damage * 100 / (100 + defense);
    } else {
        actual_damage = damage;
    }

    /* Minimum 1 damage */
    if (actual_damage < 1) actual_damage = 1;

    /* Apply damage */
    entity->hp -= actual_damage;
    entity->last_hit = get_time_ms();
    entity->in_combat = 1;
    entity->combat_time = entity->last_hit;

    /* Check death */
    if (entity->hp <= 0) {
        entity->hp = 0;
        entity_die(entity, source_id);
    }

    return actual_damage;
}

i32 entity_heal(Entity* entity, i32 amount, u32 source_id)
{
    i32 actual_heal;

    (void)source_id;

    if (!entity) return 0;
    if (!entity_is_alive(entity)) return 0;
    if (amount <= 0) return 0;

    /* Calculate actual heal */
    actual_heal = amount;
    if (entity->hp + actual_heal > entity->max_hp) {
        actual_heal = entity->max_hp - entity->hp;
    }

    entity->hp += actual_heal;

    return actual_heal;
}

void entity_die(Entity* entity, u32 killer_id)
{
    (void)killer_id;

    if (!entity) return;

    entity_set_state(entity, ENTITY_STATE_DEAD);
    entity->hp = 0;
    entity->in_combat = 0;
    entity->target_id = 0;

    /* Clear all buffs on death */
    entity->buff_count = 0;
    memset(entity->buffs, 0, sizeof(entity->buffs));
}

void entity_resurrect(Entity* entity, i32 hp_percent)
{
    if (!entity) return;
    if (entity->state != ENTITY_STATE_DEAD) return;

    /* Clamp percent */
    if (hp_percent < 1) hp_percent = 1;
    if (hp_percent > 100) hp_percent = 100;

    entity->hp = entity->max_hp * hp_percent / 100;
    if (entity->hp < 1) entity->hp = 1;

    entity->mp = entity->max_mp / 2;

    entity_set_state(entity, ENTITY_STATE_IDLE);
    entity->invincible = 1;  /* Brief invincibility after resurrect */
}

int entity_add_buff(Entity* entity, u16 buff_id, u16 skill_id,
                    u32 source_id, u32 duration_ms, i32 value)
{
    int i;
    BuffEntry* slot;

    if (!entity) return ENGINE_ERROR_PARAM;
    if (!entity_is_alive(entity)) return ENGINE_ERROR;

    /* Check if buff already exists (refresh) */
    for (i = 0; i < entity->buff_count; i++) {
        if (entity->buffs[i].buff_id == buff_id) {
            /* Refresh existing buff */
            entity->buffs[i].expire_time = get_time_ms() + duration_ms;
            entity->buffs[i].value = value;
            if (entity->buffs[i].stacks < 255) {
                entity->buffs[i].stacks++;
            }
            return ENGINE_OK;
        }
    }

    /* Add new buff */
    if (entity->buff_count >= ENTITY_MAX_BUFFS) {
        return ENGINE_ERROR_FULL;
    }

    slot = &entity->buffs[entity->buff_count];
    slot->buff_id = buff_id;
    slot->skill_id = skill_id;
    slot->source_id = source_id;
    slot->expire_time = get_time_ms() + duration_ms;
    slot->value = value;
    slot->stacks = 1;
    slot->type = 0;

    entity->buff_count++;

    return ENGINE_OK;
}

int entity_remove_buff(Entity* entity, u16 buff_id)
{
    int i;

    if (!entity) return ENGINE_ERROR_PARAM;

    for (i = 0; i < entity->buff_count; i++) {
        if (entity->buffs[i].buff_id == buff_id) {
            /* Move last buff to this slot */
            if (i < entity->buff_count - 1) {
                entity->buffs[i] = entity->buffs[entity->buff_count - 1];
            }
            entity->buff_count--;
            return ENGINE_OK;
        }
    }

    return ENGINE_ERROR;  /* Buff not found */
}

int entity_has_buff(const Entity* entity, u16 buff_id)
{
    int i;

    if (!entity) return 0;

    for (i = 0; i < entity->buff_count; i++) {
        if (entity->buffs[i].buff_id == buff_id) {
            return 1;
        }
    }

    return 0;
}

int entity_update_buffs(Entity* entity, u32 current_time)
{
    int i;
    int expired = 0;

    if (!entity) return 0;

    for (i = entity->buff_count - 1; i >= 0; i--) {
        if (current_time >= entity->buffs[i].expire_time) {
            /* Remove expired buff */
            if (i < entity->buff_count - 1) {
                entity->buffs[i] = entity->buffs[entity->buff_count - 1];
            }
            entity->buff_count--;
            expired++;
        }
    }

    return expired;
}

int entity_skill_on_cooldown(const Entity* entity, u16 skill_id)
{
    int i;
    u32 now;

    if (!entity) return 0;

    now = get_time_ms();

    for (i = 0; i < entity->cooldown_count; i++) {
        if (entity->cooldowns[i].skill_id == skill_id) {
            return entity->cooldowns[i].expire_time > now;
        }
    }

    return 0;
}

int entity_start_cooldown(Entity* entity, u16 skill_id, u32 duration_ms)
{
    int i;
    SkillCooldown* slot;

    if (!entity) return ENGINE_ERROR_PARAM;

    /* Update existing cooldown */
    for (i = 0; i < entity->cooldown_count; i++) {
        if (entity->cooldowns[i].skill_id == skill_id) {
            entity->cooldowns[i].expire_time = get_time_ms() + duration_ms;
            return ENGINE_OK;
        }
    }

    /* Add new cooldown */
    if (entity->cooldown_count >= ENTITY_MAX_SKILLS) {
        return ENGINE_ERROR_FULL;
    }

    slot = &entity->cooldowns[entity->cooldown_count];
    slot->skill_id = skill_id;
    slot->expire_time = get_time_ms() + duration_ms;

    entity->cooldown_count++;

    return ENGINE_OK;
}

int entity_update_cooldowns(Entity* entity, u32 current_time)
{
    int i;
    int expired = 0;

    if (!entity) return 0;

    for (i = entity->cooldown_count - 1; i >= 0; i--) {
        if (current_time >= entity->cooldowns[i].expire_time) {
            /* Remove expired cooldown */
            if (i < entity->cooldown_count - 1) {
                entity->cooldowns[i] = entity->cooldowns[entity->cooldown_count - 1];
            }
            entity->cooldown_count--;
            expired++;
        }
    }

    return expired;
}

i32 entity_distance_to(const Entity* entity, const Entity* other)
{
    i32 dx, dy, dz;
    double dist;

    if (!entity || !other) return 0x7FFFFFFF;

    dx = entity->pos_x - other->pos_x;
    dy = entity->pos_y - other->pos_y;
    dz = entity->pos_z - other->pos_z;

    /* 3D distance */
    dist = sqrt((double)(dx * dx + dy * dy + dz * dz));

    return (i32)dist;
}

int entity_in_range(const Entity* entity, const Entity* other, i32 range)
{
    i32 dist;

    if (!entity || !other || range < 0) return 0;

    dist = entity_distance_to(entity, other);

    return dist <= range;
}

const char* entity_type_name(EntityType type)
{
    if (type >= 0 && type < ENTITY_TYPE_MAX) {
        return type_names[type];
    }
    return "UNKNOWN";
}

const char* entity_state_name(EntityState state)
{
    if (state >= 0 && state < ENTITY_STATE_MAX) {
        return state_names[state];
    }
    return "UNKNOWN";
}
