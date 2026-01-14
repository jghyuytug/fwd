/**
 * @file combat_engine.c
 * @brief Combat system engine implementation
 */

#include "combat_engine.h"
#include <stdlib.h>
#include <string.h>

/* Random number generator (simple LCG) */
static u32 g_rand_seed = 12345;

static u32 combat_rand(void)
{
    g_rand_seed = g_rand_seed * 1103515245 + 12345;
    return (g_rand_seed >> 16) & 0x7FFF;
}

static int combat_rand_range(int min, int max)
{
    if (min >= max) return min;
    return min + (int)(combat_rand() % (max - min + 1));
}

static int combat_rand_percent(int percent)
{
    return (int)(combat_rand() % 100) < percent;
}

/* Damage type names */
static const char* damage_type_names[] = {
    "PHYSICAL",
    "MAGICAL",
    "FIRE",
    "ICE",
    "LIGHT",
    "DARK",
    "PURE"
};

/* Skill type names */
static const char* skill_type_names[] = {
    "NONE",
    "ATTACK",
    "BUFF",
    "DEBUFF",
    "HEAL",
    "SUMMON",
    "PASSIVE"
};

int combat_check_hit(const Entity* source, const Entity* target)
{
    int hit_rate;
    int evasion;
    int final_rate;

    if (!source || !target) return 0;

    hit_rate = COMBAT_HIT_CHECK_RATE + source->final_stats.hit_rate;
    evasion = target->final_stats.evasion;

    /* Final hit rate = hit_rate - evasion, min 5%, max 95% */
    final_rate = hit_rate - evasion;
    if (final_rate < 5) final_rate = 5;
    if (final_rate > 95) final_rate = 95;

    return combat_rand_percent(final_rate);
}

int combat_check_critical(const Entity* source)
{
    int crit_rate;

    if (!source) return 0;

    crit_rate = source->final_stats.critical_rate;
    if (crit_rate < 0) crit_rate = 0;
    if (crit_rate > 100) crit_rate = 100;

    return combat_rand_percent(crit_rate);
}

int combat_calculate_damage(const Entity* source, const Entity* target,
                            const SkillDefinition* skill, CombatResult* out_result)
{
    i32 base_damage;
    i32 stat_bonus;
    i32 final_damage;
    int is_crit;

    if (!source || !target || !skill || !out_result) {
        return ENGINE_ERROR_PARAM;
    }

    memset(out_result, 0, sizeof(CombatResult));
    out_result->target_id = target->id;

    /* Check hit */
    if (!combat_check_hit(source, target)) {
        out_result->is_miss = 1;
        return ENGINE_OK;
    }

    /* Base damage from skill */
    base_damage = skill->base_damage + skill->damage_per_level * skill->level;

    /* Stat scaling */
    stat_bonus = 0;
    if (skill->str_ratio > 0) {
        stat_bonus += source->final_stats.strength * skill->str_ratio / 100;
    }
    if (skill->int_ratio > 0) {
        stat_bonus += source->final_stats.intelligence * skill->int_ratio / 100;
    }

    /* Add attack stat based on damage type */
    if (skill->damage_type == DAMAGE_TYPE_PHYSICAL) {
        stat_bonus += source->final_stats.physical_attack;
    } else if (skill->damage_type != DAMAGE_TYPE_PURE) {
        stat_bonus += source->final_stats.magical_attack;
    }

    final_damage = base_damage + stat_bonus;

    /* Damage variance (90-110%) */
    final_damage = final_damage * combat_rand_range(90, 110) / 100;

    /* Critical hit check */
    is_crit = 0;
    if (skill->can_crit) {
        is_crit = combat_check_critical(source);
        if (is_crit) {
            int crit_mult = COMBAT_CRIT_MULTIPLIER +
                           source->final_stats.critical_damage;
            final_damage = final_damage * crit_mult / 100;
            out_result->is_crit = 1;
        }
    }

    /* Apply defense reduction (done in entity_take_damage) */
    if (final_damage < 1) final_damage = 1;

    out_result->damage = final_damage;

    return ENGINE_OK;
}

int combat_calculate_healing(const Entity* source, const Entity* target,
                             const SkillDefinition* skill, CombatResult* out_result)
{
    i32 base_heal;
    i32 stat_bonus;
    i32 final_heal;

    if (!source || !target || !skill || !out_result) {
        return ENGINE_ERROR_PARAM;
    }

    memset(out_result, 0, sizeof(CombatResult));
    out_result->target_id = target->id;

    /* Base healing from skill */
    base_heal = skill->base_healing + skill->healing_per_level * skill->level;

    /* Stat scaling (intelligence for healing) */
    stat_bonus = 0;
    if (skill->int_ratio > 0) {
        stat_bonus += source->final_stats.intelligence * skill->int_ratio / 100;
    }

    final_heal = base_heal + stat_bonus;

    /* Healing variance (95-105%) */
    final_heal = final_heal * combat_rand_range(95, 105) / 100;

    if (final_heal < 0) final_heal = 0;

    out_result->healing = final_heal;

    return ENGINE_OK;
}

int combat_apply_result(Entity* target, const CombatResult* result, u32 source_id)
{
    if (!target || !result) {
        return ENGINE_ERROR_PARAM;
    }

    /* Apply damage */
    if (result->damage > 0 && !result->is_miss && !result->is_dodge) {
        entity_take_damage(target, result->damage, DAMAGE_TYPE_PURE, source_id);
    }

    /* Apply healing */
    if (result->healing > 0) {
        entity_heal(target, result->healing, source_id);
    }

    return ENGINE_OK;
}

int combat_can_use_skill(const Entity* entity, const SkillDefinition* skill)
{
    if (!entity || !skill) return 0;

    /* Must be alive */
    if (!entity_is_alive(entity)) return 0;

    /* Check MP cost */
    if (entity->mp < skill->mp_cost) return 0;

    /* Check cooldown */
    if (entity_skill_on_cooldown(entity, skill->skill_id)) return 0;

    /* Check state (cannot cast while stunned) */
    if (entity->state == ENTITY_STATE_STUNNED) return 0;

    return 1;
}

int combat_execute_skill(Entity* source, Entity* target,
                         const SkillDefinition* skill, CombatResult* out_result)
{
    int ret;
    int i;

    if (!source || !target || !skill || !out_result) {
        return ENGINE_ERROR_PARAM;
    }

    /* Check if can use */
    if (!combat_can_use_skill(source, skill)) {
        return ENGINE_ERROR;
    }

    /* Consume MP */
    source->mp -= skill->mp_cost;
    if (source->mp < 0) source->mp = 0;

    /* Start cooldown */
    entity_start_cooldown((Entity*)source, skill->skill_id, skill->cooldown_ms);

    /* Calculate based on skill type */
    if (skill->type == SKILL_TYPE_ATTACK || skill->type == SKILL_TYPE_DEBUFF) {
        ret = combat_calculate_damage(source, target, skill, out_result);
    } else if (skill->type == SKILL_TYPE_HEAL) {
        ret = combat_calculate_healing(source, target, skill, out_result);
    } else {
        memset(out_result, 0, sizeof(CombatResult));
        out_result->target_id = target->id;
        ret = ENGINE_OK;
    }

    if (ret != ENGINE_OK) return ret;

    /* Multi-hit skills */
    if (skill->hit_count > 1) {
        for (i = 1; i < skill->hit_count; i++) {
            CombatResult extra;
            combat_calculate_damage(source, target, skill, &extra);
            out_result->damage += extra.damage;
            if (extra.is_crit) out_result->is_crit = 1;
        }
    }

    /* Apply result */
    combat_apply_result(target, out_result, source->id);

    /* Apply buff/debuff if skill has one */
    if (skill->buff_id > 0 && skill->buff_duration > 0) {
        entity_add_buff(target, skill->buff_id, skill->skill_id,
                       source->id, skill->buff_duration, 0);
    }

    return ENGINE_OK;
}

int combat_execute_aoe(Entity* source, Entity** targets, int target_count,
                       const SkillDefinition* skill, CombatEvent* out_event)
{
    int i;
    int ret;

    if (!source || !targets || !skill || !out_event) {
        return ENGINE_ERROR_PARAM;
    }

    if (target_count <= 0 || target_count > COMBAT_MAX_TARGETS) {
        return ENGINE_ERROR_PARAM;
    }

    /* Check if can use */
    if (!combat_can_use_skill(source, skill)) {
        return ENGINE_ERROR;
    }

    memset(out_event, 0, sizeof(CombatEvent));
    out_event->source_id = source->id;
    out_event->skill_id = skill->skill_id;

    /* Consume MP */
    source->mp -= skill->mp_cost;
    if (source->mp < 0) source->mp = 0;

    /* Start cooldown */
    entity_start_cooldown((Entity*)source, skill->skill_id, skill->cooldown_ms);

    /* Calculate for each target */
    for (i = 0; i < target_count; i++) {
        if (!targets[i]) continue;
        if (!entity_is_alive(targets[i])) continue;

        /* Check range */
        if (skill->aoe_radius > 0) {
            if (!entity_in_range(source, targets[i], skill->range + skill->aoe_radius)) {
                continue;
            }
        }

        if (skill->type == SKILL_TYPE_ATTACK || skill->type == SKILL_TYPE_DEBUFF) {
            ret = combat_calculate_damage(source, targets[i], skill,
                                         &out_event->results[out_event->result_count]);
        } else if (skill->type == SKILL_TYPE_HEAL) {
            ret = combat_calculate_healing(source, targets[i], skill,
                                          &out_event->results[out_event->result_count]);
        } else {
            continue;
        }

        if (ret == ENGINE_OK) {
            /* Apply result */
            combat_apply_result(targets[i],
                               &out_event->results[out_event->result_count],
                               source->id);

            /* Apply buff/debuff */
            if (skill->buff_id > 0 && skill->buff_duration > 0) {
                entity_add_buff(targets[i], skill->buff_id, skill->skill_id,
                               source->id, skill->buff_duration, 0);
            }

            out_event->result_count++;
        }
    }

    return ENGINE_OK;
}

const char* combat_damage_type_name(DamageType type)
{
    if (type >= 0 && type < DAMAGE_TYPE_MAX) {
        return damage_type_names[type];
    }
    return "UNKNOWN";
}

const char* combat_skill_type_name(SkillType type)
{
    if (type >= 0 && type < SKILL_TYPE_MAX) {
        return skill_type_names[type];
    }
    return "UNKNOWN";
}
