/**
 * @file monster_ai.c
 * @brief Monster AI system implementation
 */

#include "monster_ai.h"
#include <string.h>
#include <stdlib.h>

static const char* ai_type_names[] = {
    "NONE",
    "PASSIVE",
    "NEUTRAL",
    "AGGRESSIVE",
    "GUARD",
    "PATROL",
    "FOLLOW",
    "FLEE",
    "BOSS",
    "SUPPORT",
    "RANGED"
};

static const char* ai_state_names[] = {
    "IDLE",
    "PATROL",
    "CHASE",
    "ATTACK",
    "CAST",
    "FLEE",
    "RETURN",
    "DEAD"
};

int monster_ai_init(MonsterAI* ai, Entity* owner, AIType type)
{
    if (!ai || !owner) {
        return ENGINE_ERROR_PARAM;
    }

    memset(ai, 0, sizeof(MonsterAI));

    ai->owner = owner;
    ai->type = type;
    ai->state = AI_STATE_IDLE;

    /* Save spawn position */
    ai->spawn_x = owner->pos_x;
    ai->spawn_y = owner->pos_y;
    ai->spawn_z = owner->pos_z;

    /* Default ranges */
    ai->sight_range = AI_SIGHT_RANGE;
    ai->attack_range = AI_ATTACK_RANGE;
    ai->leash_range = AI_LEASH_DISTANCE;

    /* Default behavior */
    ai->can_flee = 0;
    ai->flee_hp_percent = 20;
    ai->is_social = 0;

    /* Boss defaults */
    ai->phase = 1;
    ai->max_phases = 1;

    return ENGINE_OK;
}

void monster_ai_cleanup(MonsterAI* ai)
{
    if (!ai) return;

    if (ai->patrol_points) {
        free(ai->patrol_points);
        ai->patrol_points = NULL;
    }

    memset(ai, 0, sizeof(MonsterAI));
}

static void ai_update_idle(MonsterAI* ai, u32 current_time)
{
    (void)current_time;

    /* Aggressive monsters look for targets */
    if (ai->type == AI_TYPE_AGGRESSIVE) {
        /* Would scan for nearby enemies here */
        /* For now, wait for external trigger */
    }

    /* Patrol monsters start patrolling */
    if (ai->type == AI_TYPE_PATROL && ai->patrol_points && ai->patrol_count > 0) {
        ai->state = AI_STATE_PATROL;
    }
}

static void ai_update_patrol(MonsterAI* ai, u32 current_time)
{
    PatrolPoint* point;
    i32 dx, dy;
    i32 dist_sq;

    if (!ai->patrol_points || ai->patrol_count <= 0) {
        ai->state = AI_STATE_IDLE;
        return;
    }

    point = &ai->patrol_points[ai->patrol_index];

    /* Check if reached point */
    dx = ai->owner->pos_x - point->x;
    dy = ai->owner->pos_y - point->y;
    dist_sq = dx * dx + dy * dy;

    if (dist_sq < 100 * 100) {
        /* Wait at point */
        if (current_time - ai->state_time >= point->wait_time) {
            /* Move to next point */
            ai->patrol_index = (ai->patrol_index + 1) % ai->patrol_count;
            ai->state_time = current_time;
        }
    } else {
        /* Move towards point */
        /* Movement would be handled by entity system */
    }
}

static void ai_update_chase(MonsterAI* ai, u32 current_time)
{
    i32 dist;
    i32 dx, dy;
    i32 dist_from_spawn;

    (void)current_time;

    if (!ai->target || !entity_is_alive(ai->target)) {
        /* Target lost, return to spawn */
        ai->target = NULL;
        ai->target_id = 0;
        ai->state = AI_STATE_RETURN;
        return;
    }

    /* Check leash distance */
    dx = ai->owner->pos_x - ai->spawn_x;
    dy = ai->owner->pos_y - ai->spawn_y;
    dist_from_spawn = dx * dx + dy * dy;

    if (dist_from_spawn > ai->leash_range * ai->leash_range) {
        /* Too far from spawn, return */
        ai->state = AI_STATE_RETURN;
        monster_ai_clear_aggro(ai);
        return;
    }

    /* Check if in attack range */
    dist = entity_distance_to(ai->owner, ai->target);

    if (dist <= ai->attack_range) {
        ai->state = AI_STATE_ATTACK;
    }

    /* Chase movement would be handled by entity system */
}

static void ai_update_attack(MonsterAI* ai, u32 current_time)
{
    i32 dist;

    if (!ai->target || !entity_is_alive(ai->target)) {
        /* Target lost */
        ai->target = NULL;
        ai->target_id = 0;
        ai->state = AI_STATE_IDLE;
        return;
    }

    /* Check if still in range */
    dist = entity_distance_to(ai->owner, ai->target);

    if (dist > ai->attack_range) {
        /* Chase again */
        ai->state = AI_STATE_CHASE;
        return;
    }

    /* Attack if cooldown ready */
    if (current_time >= ai->attack_cooldown) {
        /* Perform attack - would call combat system */
        ai->attack_cooldown = current_time + 1000;  /* 1 second cooldown */
        ai->owner->last_attack = current_time;
    }

    /* Check for flee condition */
    if (ai->can_flee) {
        i32 hp_percent = ai->owner->hp * 100 / ai->owner->max_hp;
        if (hp_percent <= ai->flee_hp_percent) {
            ai->state = AI_STATE_FLEE;
        }
    }
}

static void ai_update_flee(MonsterAI* ai, u32 current_time)
{
    (void)current_time;

    /* Flee away from target */
    /* Movement would be handled by entity system */

    /* Check if safe (far from target or healed) */
    if (!ai->target) {
        ai->state = AI_STATE_IDLE;
        return;
    }

    i32 dist = entity_distance_to(ai->owner, ai->target);
    if (dist > ai->sight_range) {
        ai->state = AI_STATE_RETURN;
    }

    i32 hp_percent = ai->owner->hp * 100 / ai->owner->max_hp;
    if (hp_percent > ai->flee_hp_percent + 10) {
        /* Recovered enough, fight again */
        ai->state = AI_STATE_CHASE;
    }
}

static void ai_update_return(MonsterAI* ai, u32 current_time)
{
    i32 dx, dy;
    i32 dist_sq;

    (void)current_time;

    /* Move towards spawn */
    dx = ai->owner->pos_x - ai->spawn_x;
    dy = ai->owner->pos_y - ai->spawn_y;
    dist_sq = dx * dx + dy * dy;

    if (dist_sq < 100 * 100) {
        /* Reached spawn, become idle */
        ai->state = AI_STATE_IDLE;

        /* Heal to full when returning */
        ai->owner->hp = ai->owner->max_hp;
        ai->owner->mp = ai->owner->max_mp;
    }

    /* Movement would be handled by entity system */
}

static void ai_update_boss(MonsterAI* ai, u32 current_time)
{
    int i;
    i32 hp_percent;

    /* Boss phase transitions */
    hp_percent = ai->owner->hp * 100 / ai->owner->max_hp;

    for (i = ai->max_phases - 1; i >= 0; i--) {
        if (hp_percent <= ai->phase_hp_thresholds[i] && ai->phase < i + 2) {
            ai->phase = i + 2;
            /* Phase transition - could trigger special skills */
            break;
        }
    }

    /* Normal combat behavior */
    ai_update_attack(ai, current_time);
}

void monster_ai_update(MonsterAI* ai, u32 current_time)
{
    if (!ai || !ai->owner) return;

    /* Check if dead */
    if (!entity_is_alive(ai->owner)) {
        ai->state = AI_STATE_DEAD;
        return;
    }

    /* Throttle updates */
    if (current_time - ai->last_update < AI_UPDATE_INTERVAL) {
        return;
    }
    ai->last_update = current_time;

    /* Decay aggro */
    {
        int i;
        for (i = ai->aggro_count - 1; i >= 0; i--) {
            ai->aggro_list[i].aggro -= AI_AGGRO_DECAY_RATE;
            if (ai->aggro_list[i].aggro <= 0) {
                /* Remove from list */
                if (i < ai->aggro_count - 1) {
                    ai->aggro_list[i] = ai->aggro_list[ai->aggro_count - 1];
                }
                ai->aggro_count--;
            }
        }
    }

    /* Update target from aggro list */
    if (ai->aggro_count > 0 && ai->state != AI_STATE_RETURN &&
        ai->state != AI_STATE_FLEE) {
        u32 top_id = monster_ai_get_top_aggro(ai);
        if (top_id != ai->target_id) {
            ai->target_id = top_id;
            /* Would need to look up entity by ID */
        }
    }

    /* State-specific update */
    switch (ai->state) {
        case AI_STATE_IDLE:
            ai_update_idle(ai, current_time);
            break;

        case AI_STATE_PATROL:
            ai_update_patrol(ai, current_time);
            break;

        case AI_STATE_CHASE:
            ai_update_chase(ai, current_time);
            break;

        case AI_STATE_ATTACK:
            if (ai->type == AI_TYPE_BOSS) {
                ai_update_boss(ai, current_time);
            } else {
                ai_update_attack(ai, current_time);
            }
            break;

        case AI_STATE_FLEE:
            ai_update_flee(ai, current_time);
            break;

        case AI_STATE_RETURN:
            ai_update_return(ai, current_time);
            break;

        case AI_STATE_DEAD:
            /* Do nothing */
            break;

        default:
            break;
    }
}

void monster_ai_add_aggro(MonsterAI* ai, u32 entity_id, i32 amount)
{
    int i;

    if (!ai || entity_id == 0 || amount <= 0) return;

    /* Find existing entry */
    for (i = 0; i < ai->aggro_count; i++) {
        if (ai->aggro_list[i].entity_id == entity_id) {
            ai->aggro_list[i].aggro += amount;
            return;
        }
    }

    /* Add new entry */
    if (ai->aggro_count < AI_MAX_AGGRO_LIST) {
        ai->aggro_list[ai->aggro_count].entity_id = entity_id;
        ai->aggro_list[ai->aggro_count].aggro = amount;
        ai->aggro_list[ai->aggro_count].last_update = 0;
        ai->aggro_count++;
    }
}

void monster_ai_remove_aggro(MonsterAI* ai, u32 entity_id)
{
    int i;

    if (!ai) return;

    for (i = 0; i < ai->aggro_count; i++) {
        if (ai->aggro_list[i].entity_id == entity_id) {
            if (i < ai->aggro_count - 1) {
                ai->aggro_list[i] = ai->aggro_list[ai->aggro_count - 1];
            }
            ai->aggro_count--;
            return;
        }
    }
}

void monster_ai_clear_aggro(MonsterAI* ai)
{
    if (!ai) return;

    ai->aggro_count = 0;
    ai->target = NULL;
    ai->target_id = 0;
}

u32 monster_ai_get_top_aggro(const MonsterAI* ai)
{
    int i;
    u32 top_id = 0;
    i32 top_aggro = 0;

    if (!ai) return 0;

    for (i = 0; i < ai->aggro_count; i++) {
        if (ai->aggro_list[i].aggro > top_aggro) {
            top_aggro = ai->aggro_list[i].aggro;
            top_id = ai->aggro_list[i].entity_id;
        }
    }

    return top_id;
}

void monster_ai_on_damage(MonsterAI* ai, Entity* attacker, i32 damage)
{
    if (!ai || !attacker) return;

    /* Add aggro based on damage */
    monster_ai_add_aggro(ai, attacker->id, damage);

    /* Passive monsters become hostile */
    if (ai->type == AI_TYPE_PASSIVE || ai->type == AI_TYPE_NEUTRAL) {
        if (ai->state == AI_STATE_IDLE || ai->state == AI_STATE_PATROL) {
            ai->target = attacker;
            ai->target_id = attacker->id;
            ai->state = AI_STATE_CHASE;
        }
    }

    /* Aggressive monsters that were idle now engage */
    if (ai->state == AI_STATE_IDLE) {
        ai->target = attacker;
        ai->target_id = attacker->id;
        ai->state = AI_STATE_CHASE;
    }
}

void monster_ai_on_death(MonsterAI* ai)
{
    if (!ai) return;

    ai->state = AI_STATE_DEAD;
    monster_ai_clear_aggro(ai);
}

int monster_ai_set_patrol(MonsterAI* ai, const PatrolPoint* points, int count)
{
    if (!ai || !points || count <= 0) {
        return ENGINE_ERROR_PARAM;
    }

    /* Free old patrol */
    if (ai->patrol_points) {
        free(ai->patrol_points);
    }

    /* Allocate new patrol */
    ai->patrol_points = malloc(sizeof(PatrolPoint) * count);
    if (!ai->patrol_points) {
        return ENGINE_ERROR_MEMORY;
    }

    memcpy(ai->patrol_points, points, sizeof(PatrolPoint) * count);
    ai->patrol_count = count;
    ai->patrol_index = 0;

    return ENGINE_OK;
}

void monster_ai_set_target(MonsterAI* ai, Entity* target)
{
    if (!ai) return;

    ai->target = target;
    ai->target_id = target ? target->id : 0;

    if (target && entity_is_alive(target)) {
        if (ai->state == AI_STATE_IDLE || ai->state == AI_STATE_PATROL) {
            ai->state = AI_STATE_CHASE;
        }
    }
}

u16 monster_ai_select_skill(const MonsterAI* ai)
{
    if (!ai || !ai->owner) return 0;
    if (!ai->has_special_skill) return 0;

    /* Simple skill selection based on phase and HP */
    /* Would be expanded with actual skill tables */

    if (ai->type == AI_TYPE_BOSS) {
        /* Boss uses different skills per phase */
        return ai->phase * 100;  /* Placeholder */
    }

    return 0;
}

const char* monster_ai_type_name(AIType type)
{
    if (type >= 0 && type < AI_TYPE_MAX) {
        return ai_type_names[type];
    }
    return "UNKNOWN";
}

const char* monster_ai_state_name(AIState state)
{
    if (state >= 0 && state < AI_STATE_MAX) {
        return ai_state_names[state];
    }
    return "UNKNOWN";
}
