/**
 * @file buff_system.c
 * @brief Buff/Debuff system implementation
 */

#include "buff_system.h"
#include <string.h>
#include <stdio.h>

static const char* category_names[] = {
    "NONE",
    "BUFF",
    "DEBUFF",
    "DOT",
    "HOT",
    "CROWD_CONTROL"
};

static const char* effect_names[] = {
    "NONE",
    "STRENGTH", "INTELLIGENCE", "VITALITY", "SPIRIT",
    "PHYS_ATTACK", "MAG_ATTACK", "PHYS_DEFENSE", "MAG_DEFENSE",
    "HIT_RATE", "EVASION", "CRIT_RATE", "CRIT_DAMAGE",
    "ATTACK_SPEED", "MOVE_SPEED",
    "HP_REGEN", "MP_REGEN", "HP_DRAIN", "MP_DRAIN",
    "STUN", "SILENCE", "SLOW", "ROOT", "FEAR", "SLEEP",
    "SHIELD", "INVINCIBLE", "INVISIBLE"
};

int buff_system_init(BuffSystem* system)
{
    if (!system) {
        return ENGINE_ERROR_PARAM;
    }

    memset(system, 0, sizeof(BuffSystem));
    system->definition_count = 0;
    system->last_tick = 0;

    return ENGINE_OK;
}

void buff_system_cleanup(BuffSystem* system)
{
    if (!system) return;
    memset(system, 0, sizeof(BuffSystem));
}

int buff_system_register(BuffSystem* system, const BuffDefinition* def)
{
    int i;

    if (!system || !def) {
        return ENGINE_ERROR_PARAM;
    }

    /* Check for duplicate */
    for (i = 0; i < system->definition_count; i++) {
        if (system->definitions[i].buff_id == def->buff_id) {
            /* Update existing */
            system->definitions[i] = *def;
            return ENGINE_OK;
        }
    }

    /* Add new */
    if (system->definition_count >= BUFF_MAX_DEFINITIONS) {
        return ENGINE_ERROR_FULL;
    }

    system->definitions[system->definition_count] = *def;
    system->definition_count++;

    return ENGINE_OK;
}

const BuffDefinition* buff_system_get_def(const BuffSystem* system, u16 buff_id)
{
    int i;

    if (!system) return NULL;

    for (i = 0; i < system->definition_count; i++) {
        if (system->definitions[i].buff_id == buff_id) {
            return &system->definitions[i];
        }
    }

    return NULL;
}

static void apply_stat_modifier(CombatStats* stats, BuffEffectType effect,
                                i32 value, BuffModType mod_type)
{
    i32* target = NULL;

    switch (effect) {
        case BUFF_EFFECT_STRENGTH:
            target = &stats->strength;
            break;
        case BUFF_EFFECT_INTELLIGENCE:
            target = &stats->intelligence;
            break;
        case BUFF_EFFECT_VITALITY:
            target = &stats->vitality;
            break;
        case BUFF_EFFECT_SPIRIT:
            target = &stats->spirit;
            break;
        case BUFF_EFFECT_PHYS_ATTACK:
            target = &stats->physical_attack;
            break;
        case BUFF_EFFECT_MAG_ATTACK:
            target = &stats->magical_attack;
            break;
        case BUFF_EFFECT_PHYS_DEFENSE:
            target = &stats->physical_defense;
            break;
        case BUFF_EFFECT_MAG_DEFENSE:
            target = &stats->magical_defense;
            break;
        case BUFF_EFFECT_HIT_RATE:
            target = &stats->hit_rate;
            break;
        case BUFF_EFFECT_EVASION:
            target = &stats->evasion;
            break;
        case BUFF_EFFECT_CRIT_RATE:
            target = &stats->critical_rate;
            break;
        case BUFF_EFFECT_CRIT_DAMAGE:
            target = &stats->critical_damage;
            break;
        case BUFF_EFFECT_ATTACK_SPEED:
            target = &stats->attack_speed;
            break;
        case BUFF_EFFECT_MOVE_SPEED:
            target = &stats->move_speed;
            break;
        default:
            return;
    }

    if (target) {
        if (mod_type == BUFF_MOD_FLAT) {
            *target += value;
        } else {
            /* Percent modifier */
            *target = *target * (100 + value) / 100;
        }
    }
}

void buff_apply_stat_effects(const BuffSystem* system, Entity* entity)
{
    int i;
    const BuffDefinition* def;
    i32 total_value;

    if (!system || !entity) return;

    /* First calculate base stats */
    entity_calculate_stats(entity);

    /* Apply buff effects to bonus stats */
    for (i = 0; i < entity->buff_count; i++) {
        def = buff_system_get_def(system, entity->buffs[i].buff_id);
        if (!def) continue;

        /* Skip non-stat effects */
        if (def->effect_type < BUFF_EFFECT_STRENGTH ||
            def->effect_type > BUFF_EFFECT_MOVE_SPEED) {
            continue;
        }

        /* Calculate total value with stacks */
        total_value = def->base_value +
                      def->value_per_stack * (entity->buffs[i].stacks - 1);

        apply_stat_modifier(&entity->bonus_stats, def->effect_type,
                           total_value, def->mod_type);
    }

    /* Recalculate final stats */
    entity_calculate_stats(entity);
}

void buff_process_tick_effects(const BuffSystem* system, Entity* entity,
                               u32 current_time)
{
    int i;
    const BuffDefinition* def;
    i32 tick_value;

    if (!system || !entity) return;
    if (!entity_is_alive(entity)) return;

    for (i = 0; i < entity->buff_count; i++) {
        def = buff_system_get_def(system, entity->buffs[i].buff_id);
        if (!def) continue;

        /* Calculate tick value with stacks */
        tick_value = def->base_value +
                     def->value_per_stack * (entity->buffs[i].stacks - 1);

        switch (def->effect_type) {
            case BUFF_EFFECT_HP_REGEN:
                entity_heal(entity, tick_value, entity->buffs[i].source_id);
                break;

            case BUFF_EFFECT_MP_REGEN:
                entity->mp += tick_value;
                if (entity->mp > entity->max_mp) {
                    entity->mp = entity->max_mp;
                }
                break;

            case BUFF_EFFECT_HP_DRAIN:
                entity_take_damage(entity, tick_value, DAMAGE_TYPE_PURE,
                                  entity->buffs[i].source_id);
                break;

            case BUFF_EFFECT_MP_DRAIN:
                entity->mp -= tick_value;
                if (entity->mp < 0) {
                    entity->mp = 0;
                }
                break;

            default:
                break;
        }
    }

    (void)current_time;
}

int buff_has_cc_effect(const BuffSystem* system, const Entity* entity,
                       BuffEffectType effect)
{
    int i;
    const BuffDefinition* def;

    if (!system || !entity) return 0;

    /* Validate it's a CC effect */
    if (effect < BUFF_EFFECT_STUN || effect > BUFF_EFFECT_SLEEP) {
        return 0;
    }

    for (i = 0; i < entity->buff_count; i++) {
        def = buff_system_get_def(system, entity->buffs[i].buff_id);
        if (!def) continue;

        if (def->effect_type == effect) {
            return 1;
        }
    }

    return 0;
}

int buff_dispel(Entity* entity, int is_debuff, int max_dispel)
{
    int i;
    int dispelled = 0;

    if (!entity || max_dispel <= 0) return 0;

    for (i = entity->buff_count - 1; i >= 0 && dispelled < max_dispel; i--) {
        /* Check if buff matches type and is dispellable */
        /* Note: would need BuffSystem to check is_dispellable properly */

        /* For now, assume all buffs at odd indices are debuffs */
        int buff_is_debuff = (entity->buffs[i].type != 0);

        if (buff_is_debuff == is_debuff) {
            /* Remove this buff */
            if (i < entity->buff_count - 1) {
                entity->buffs[i] = entity->buffs[entity->buff_count - 1];
            }
            entity->buff_count--;
            dispelled++;
        }
    }

    return dispelled;
}

const char* buff_category_name(BuffCategory category)
{
    if (category >= 0 && category < BUFF_CAT_MAX) {
        return category_names[category];
    }
    return "UNKNOWN";
}

const char* buff_effect_name(BuffEffectType effect)
{
    if (effect >= 0 && effect < BUFF_EFFECT_MAX) {
        return effect_names[effect];
    }
    return "UNKNOWN";
}
