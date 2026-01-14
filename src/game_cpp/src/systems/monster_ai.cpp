#include "systems/monster_ai.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*==============================================================================
 * Phase 6: Monster Skill Data
 *============================================================================*/

static const MonsterSkillInfo g_monster_skill_table[] = {
    /* Basic monster skills (skill_id 10000+) */
    /* {id, type, base_dmg, scaling, range, aoe, cd, cast, recover, debuff, dur, val, elem} */

    {10001, MONSTER_ATTACK_MELEE, 50, 10, 50, 0, 30, 0, 10, 0, 0, 0, ELEMENT_NONE},
    {10002, MONSTER_ATTACK_MELEE, 80, 15, 60, 0, 45, 5, 15, 0, 0, 0, ELEMENT_NONE},
    {10003, MONSTER_ATTACK_MELEE, 120, 20, 50, 30, 60, 10, 20, 0, 0, 0, ELEMENT_NONE},

    {10010, MONSTER_ATTACK_RANGED, 40, 8, 200, 0, 25, 5, 10, 0, 0, 0, ELEMENT_NONE},
    {10011, MONSTER_ATTACK_RANGED, 60, 12, 250, 0, 35, 10, 15, 0, 0, 0, ELEMENT_FIRE},
    {10012, MONSTER_ATTACK_RANGED, 50, 10, 300, 0, 40, 15, 10, DEBUFF_SLOW, 5000, -20, ELEMENT_ICE},

    {10020, MONSTER_ATTACK_AOE_CIRCLE, 100, 18, 0, 80, 90, 30, 30, 0, 0, 0, ELEMENT_FIRE},
    {10021, MONSTER_ATTACK_AOE_CONE, 80, 15, 100, 60, 60, 20, 20, 0, 0, 0, ELEMENT_NONE},
    {10022, MONSTER_ATTACK_AOE_LINE, 90, 16, 150, 40, 75, 25, 25, 0, 0, 0, ELEMENT_LIGHT},

    {10030, MONSTER_ATTACK_CHARGE, 150, 25, 200, 0, 120, 20, 40, DEBUFF_STUN, 2000, 0, ELEMENT_NONE},
    {10031, MONSTER_ATTACK_GRAB, 30, 5, 40, 0, 150, 10, 60, 0, 0, 0, ELEMENT_NONE},
    {10032, MONSTER_ATTACK_DEBUFF_PLAYER, 0, 0, 150, 0, 180, 15, 20, DEBUFF_POISON, 10000, 3, ELEMENT_DARK},
    {10033, MONSTER_ATTACK_DEBUFF_PLAYER, 0, 0, 100, 50, 200, 20, 30, DEBUFF_CURSE, 15000, -15, ELEMENT_DARK},

    {10040, MONSTER_ATTACK_BUFF_SELF, 0, 0, 0, 0, 300, 30, 20, BUFF_ATTACK_UP, 20000, 30, ELEMENT_NONE},
    {10041, MONSTER_ATTACK_BUFF_SELF, 0, 0, 0, 0, 400, 40, 30, BUFF_DEFENSE_UP, 25000, 50, ELEMENT_NONE},
    {10042, MONSTER_ATTACK_HEAL, 0, 0, 0, 100, 250, 30, 20, 0, 0, 20, ELEMENT_LIGHT},
    {10043, MONSTER_ATTACK_SUMMON, 0, 0, 0, 0, 500, 60, 60, 0, 0, 2, ELEMENT_NONE},

    {10100, MONSTER_ATTACK_AOE_CIRCLE, 200, 35, 0, 150, 150, 45, 45, DEBUFF_BURN, 8000, 150, ELEMENT_FIRE},
    {10101, MONSTER_ATTACK_AOE_CIRCLE, 180, 30, 0, 120, 180, 50, 40, DEBUFF_FREEZE, 3000, 0, ELEMENT_ICE},
    {10102, MONSTER_ATTACK_CHARGE, 300, 50, 300, 0, 200, 30, 60, DEBUFF_STUN, 3000, 0, ELEMENT_NONE},
    {10103, MONSTER_ATTACK_GRAB, 50, 10, 60, 0, 300, 15, 90, 0, 0, 0, ELEMENT_DARK},
    {10104, MONSTER_ATTACK_SUMMON, 0, 0, 0, 0, 600, 90, 90, 0, 0, 4, ELEMENT_NONE},

    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

static const MonsterSkillInfo* find_monster_skill(uint16_t skill_id) {
    for (int i = 0; g_monster_skill_table[i].skill_id != 0; i++) {
        if (g_monster_skill_table[i].skill_id == skill_id) {
            return &g_monster_skill_table[i];
        }
    }
    return NULL;
}

static void monster_get_skills(uint8_t ai_type, uint8_t level,
                               uint16_t* skills, int* count) {
    *count = 0;

    switch (ai_type) {
        case MONSTER_AI_PASSIVE:
        case MONSTER_AI_NORMAL:
            skills[(*count)++] = 10001;
            if (level >= 10) skills[(*count)++] = 10002;
            break;
        case MONSTER_AI_AGGRESSIVE:
            skills[(*count)++] = 10002;
            skills[(*count)++] = 10003;
            if (level >= 20) skills[(*count)++] = 10030;
            break;
        case MONSTER_AI_RANGED:
            skills[(*count)++] = 10010;
            skills[(*count)++] = 10011;
            if (level >= 15) skills[(*count)++] = 10012;
            break;
        case MONSTER_AI_SUPPORT:
            skills[(*count)++] = 10001;
            skills[(*count)++] = 10042;
            skills[(*count)++] = 10040;
            break;
        case MONSTER_AI_ASSASSIN:
            skills[(*count)++] = 10002;
            skills[(*count)++] = 10030;
            if (level >= 25) skills[(*count)++] = 10032;
            break;
        case MONSTER_AI_TANK:
            skills[(*count)++] = 10003;
            skills[(*count)++] = 10041;
            if (level >= 20) skills[(*count)++] = 10020;
            break;
        case MONSTER_AI_SWARM:
            skills[(*count)++] = 10001;
            break;
        case MONSTER_AI_BOSS:
            skills[(*count)++] = 10100;
            skills[(*count)++] = 10101;
            skills[(*count)++] = 10102;
            skills[(*count)++] = 10104;
            break;
        case MONSTER_AI_STATIONARY:
            skills[(*count)++] = 10011;
            skills[(*count)++] = 10022;
            break;
        default:
            skills[(*count)++] = 10001;
            break;
    }
}

/*==============================================================================
 * Phase 6: Enhanced Monster AI Functions
 *============================================================================*/

/* Calculate distance between two points */
static uint32_t monster_distance(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    int dx = (int)x1 - (int)x2;
    int dy = (int)y1 - (int)y2;
    /* Simple manhattan distance for performance */
    return (uint32_t)(abs(dx) + abs(dy));
}

/* Move monster towards target */
static void monster_move_towards(MonsterInfo* m, uint16_t tx, uint16_t ty) {
    int dx = (int)tx - (int)m->x;
    int dy = (int)ty - (int)m->y;
    
    /* Normalize movement by speed */
    if (abs(dx) > m->move_speed) {
        dx = (dx > 0) ? m->move_speed : -m->move_speed;
    }
    if (abs(dy) > m->move_speed) {
        dy = (dy > 0) ? m->move_speed : -m->move_speed;
    }
    
    m->x = (uint16_t)((int)m->x + dx);
    m->y = (uint16_t)((int)m->y + dy);
}

/* Update single monster AI (called per tick) */
int monster_update_ai(MonsterInfo* m, uint16_t player_x, uint16_t player_y,
                      uint32_t player_charac_no, uint32_t current_time) {
    if (!m->alive) return 0;
    
    uint32_t dist_to_player = monster_distance(m->x, m->y, player_x, player_y);
    int action_taken = 0;
    
    /* Reduce attack cooldown */
    if (m->attack_cooldown > 0) m->attack_cooldown--;
    
    /* Reduce hit stun */
    if (m->hit_stun_frames > 0) {
        m->hit_stun_frames--;
        m->state = MONSTER_STATE_HIT_STUN;
        return 0;  /* Can't act while stunned */
    }
    
    switch (m->state) {
        case MONSTER_STATE_IDLE:
            /* Check if player in aggro range */
            if (dist_to_player <= m->aggro_range) {
                m->state = MONSTER_STATE_CHASE;
                m->target_player = player_charac_no;
                m->target_x = player_x;
                m->target_y = player_y;
            }
            break;
            
        case MONSTER_STATE_CHASE:
            /* Update target position */
            m->target_x = player_x;
            m->target_y = player_y;
            
            /* Check if in attack range */
            if (dist_to_player <= m->attack_range && m->attack_cooldown == 0) {
                m->state = MONSTER_STATE_ATTACK;
            } else if (dist_to_player > m->aggro_range * 2) {
                /* Lost target, return to spawn */
                m->state = MONSTER_STATE_PATROL;
                m->target_player = 0;
            } else {
                /* Move towards player */
                monster_move_towards(m, player_x, player_y);
                action_taken = 1;
            }
            break;
            
        case MONSTER_STATE_ATTACK:
            if (m->attack_cooldown == 0 && dist_to_player <= m->attack_range) {
                /* Attack ready - set cooldown (30 ticks ~= 1 second at 30fps) */
                m->attack_cooldown = 30;
                action_taken = 2;  /* 2 = attack action */
            } else {
                /* Return to chase */
                m->state = MONSTER_STATE_CHASE;
            }
            break;
            
        case MONSTER_STATE_PATROL:
            /* Return to spawn point */
            if (monster_distance(m->x, m->y, m->spawn_x, m->spawn_y) > 10) {
                monster_move_towards(m, m->spawn_x, m->spawn_y);
                action_taken = 1;
            } else {
                m->state = MONSTER_STATE_IDLE;
            }
            /* Re-check for player */
            if (dist_to_player <= m->aggro_range) {
                m->state = MONSTER_STATE_CHASE;
                m->target_player = player_charac_no;
            }
            break;
            
        case MONSTER_STATE_HIT_STUN:
            /* Wait for stun to end */
            break;
            
        case MONSTER_STATE_DEAD:
            /* Do nothing */
            break;
    }
    
    m->last_action_time = current_time;
    return action_taken;
}

/* Apply hit stun to monster when damaged */
void monster_apply_hit_stun(MonsterInfo* m, uint8_t stun_frames) {
    if (m->alive) {
        m->hit_stun_frames = stun_frames;
        m->state = MONSTER_STATE_HIT_STUN;
    }
}

/*------------------------------------------------------------------------------
 * Phase 10 Task 2: Enhanced Monster Skill System
 *----------------------------------------------------------------------------*/

/* Monster skill slot IDs for specific monster types */
#define MONSTER_SKILL_SLOT_NORMAL   0
#define MONSTER_SKILL_SLOT_AOE      1
#define MONSTER_SKILL_SLOT_SPECIAL  2
#define MONSTER_SKILL_SLOT_ENRAGE   3

/*==============================================================================
 * Phase 12 Task 1: Enhanced Boss System
 *============================================================================*/

/* Boss phase thresholds */
#define BOSS_PHASE_NORMAL    0   /* 100% - 70% HP */
#define BOSS_PHASE_ENHANCED  1   /* 70% - 30% HP */
#define BOSS_PHASE_ENRAGED   2   /* Below 30% HP */

#define BOSS_PHASE_ENHANCED_THRESHOLD  70  /* % HP */
#define BOSS_PHASE_ENRAGED_THRESHOLD   30  /* % HP */

/* Boss skill cycle patterns */
#define BOSS_CYCLE_MAX_SKILLS 8

/* Boss template structure */
typedef struct {
    uint32_t boss_id;
    char     name[32];
    uint32_t base_hp;
    uint32_t base_attack;
    uint32_t base_phys_def;
    uint32_t base_mag_def;
    uint8_t  weak_element;        /* ElementType */
    uint8_t  armor_break_hits;    /* Hits needed to break armor */
    uint16_t armor_break_duration;/* Ticks armor stays broken */
    /* Skill cycle per phase: skill_id sequence (0 = end) */
    uint16_t phase_skills[3][BOSS_CYCLE_MAX_SKILLS];
    /* Phase skill cooldowns */
    uint16_t phase_cooldowns[3];  /* CD between cycle advances per phase */
    /* Phase stat multipliers (100 = no change) */
    uint8_t  phase_atk_mult[3];   /* 100, 120, 150 typical */
    uint8_t  phase_aspd_mult[3];  /* 100, 110, 130 typical */
} BossTemplate;

/* Boss templates table (positional init for GCC 4.1.2 compatibility)
 * Fields: boss_id, name, base_hp, base_attack, base_phys_def, base_mag_def,
 *         weak_element, armor_break_hits, armor_break_duration,
 *         phase_skills[3][8], phase_cooldowns[3], phase_atk_mult[3], phase_aspd_mult[3]
 */
static const BossTemplate g_boss_templates[] = {
    /* Boss ID 1: Goblin King - Fire weakness, balanced stats */
    {
        1, "Goblin King", 50000, 500, 200, 100,
        ELEMENT_FIRE, 10, 150,
        /* phase_skills[3][8]: Normal, Enhanced, Enraged */
        {{1001, 1002, 1001, 1003, 0, 0, 0, 0},
         {1002, 1003, 1002, 1004, 0, 0, 0, 0},
         {1004, 1003, 1004, 1004, 0, 0, 0, 0}},
        {60, 45, 30},      /* phase_cooldowns */
        {100, 120, 150},   /* phase_atk_mult */
        {100, 115, 140}    /* phase_aspd_mult */
    },
    /* Boss ID 2: Dark Mage - Light weakness, high magic def */
    {
        2, "Dark Mage", 40000, 600, 100, 300,
        ELEMENT_LIGHT, 8, 120,
        {{2001, 2002, 2001, 0, 0, 0, 0, 0},
         {2002, 2003, 2002, 2003, 0, 0, 0, 0},
         {2003, 2004, 2003, 2004, 0, 0, 0, 0}},
        {75, 55, 40},
        {100, 125, 160},
        {100, 110, 125}
    },
    /* Boss ID 3: Ice Dragon - Fire weakness, high HP and def */
    {
        3, "Ice Dragon", 80000, 700, 350, 400,
        ELEMENT_FIRE, 15, 180,
        {{3001, 3002, 3001, 3002, 3003, 0, 0, 0},
         {3002, 3003, 3004, 3002, 3003, 0, 0, 0},
         {3004, 3004, 3003, 3004, 3004, 0, 0, 0}},
        {90, 70, 45},
        {100, 130, 170},
        {100, 120, 150}
    },
    /* Terminator */
    {0, "", 0, 0, 0, 0, 0, 0, 0, {{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}}, {0,0,0}, {0,0,0}, {0,0,0}}
};

/* Find boss template by ID */
static const BossTemplate* find_boss_template(uint32_t boss_id) {
    for (int i = 0; g_boss_templates[i].boss_id != 0; i++) {
        if (g_boss_templates[i].boss_id == boss_id) {
            return &g_boss_templates[i];
        }
    }
    return NULL;
}

/* Initialize boss monster from template */
void boss_init_from_template(MonsterInfo* m, uint32_t boss_id, uint8_t difficulty) {
    const BossTemplate* tmpl = find_boss_template(boss_id);
    if (!tmpl) return;

    m->is_boss = 1;
    m->boss_id = boss_id;
    m->boss_phase = BOSS_PHASE_NORMAL;
    m->weak_element = tmpl->weak_element;
    m->armor_broken = 0;
    m->armor_break_timer = 0;
    m->skill_cycle_index = 0;
    m->ai_type = MONSTER_AI_BOSS;

    /* Apply difficulty scaling (1.0x, 1.3x, 1.6x, 2.0x, 2.5x) */
    float diff_mult = 1.0f + (difficulty * 0.3f);
    if (difficulty >= 4) diff_mult = 2.5f;

    m->max_hp = (uint32_t)(tmpl->base_hp * diff_mult);
    m->hp = m->max_hp;
    m->attack_damage = (uint32_t)(tmpl->base_attack * diff_mult);
    m->phys_def = (uint32_t)(tmpl->base_phys_def * diff_mult);
    m->mag_def = (uint32_t)(tmpl->base_mag_def * diff_mult);

    /* Initialize phase cooldowns */
    for (int i = 0; i < 3; i++) {
        m->phase_skill_cd[i] = 0;
    }

    printf("[BOSS] Initialized '%s' (ID=%u) difficulty=%u HP=%u ATK=%u\n",
           tmpl->name, boss_id, difficulty, m->max_hp, m->attack_damage);
}

/* Check and update boss phase based on HP */
static uint8_t boss_check_phase_transition(MonsterInfo* m) {
    if (!m->is_boss || !m->alive) return 0;

    uint8_t hp_percent = (uint8_t)((m->hp * 100) / m->max_hp);
    uint8_t old_phase = m->boss_phase;
    uint8_t new_phase = old_phase;

    if (hp_percent <= BOSS_PHASE_ENRAGED_THRESHOLD) {
        new_phase = BOSS_PHASE_ENRAGED;
    } else if (hp_percent <= BOSS_PHASE_ENHANCED_THRESHOLD) {
        new_phase = BOSS_PHASE_ENHANCED;
    } else {
        new_phase = BOSS_PHASE_NORMAL;
    }

    if (new_phase != old_phase) {
        m->boss_phase = new_phase;
        m->skill_cycle_index = 0;  /* Reset skill cycle on phase change */

        const BossTemplate* tmpl = find_boss_template(m->boss_id);
        if (tmpl) {
            /* Apply phase stat multipliers */
            uint32_t base_atk = m->attack_damage * 100 / tmpl->phase_atk_mult[old_phase];
            m->attack_damage = base_atk * tmpl->phase_atk_mult[new_phase] / 100;

            printf("[BOSS] Phase transition: %u -> %u (HP=%u%%, ATK=%u)\n",
                   old_phase, new_phase, hp_percent, m->attack_damage);
        }

        /* Phase transition effects */
        if (new_phase == BOSS_PHASE_ENRAGED) {
            m->enraged = 1;
            m->attack_cooldown = 0;  /* Immediate attack on enrage */
        }

        return 1;  /* Phase changed */
    }

    return 0;  /* No change */
}

/* Get next skill in boss's skill cycle for current phase */
static uint16_t boss_get_cycle_skill(MonsterInfo* m) {
    if (!m->is_boss) return 0;

    const BossTemplate* tmpl = find_boss_template(m->boss_id);
    if (!tmpl) return 0;

    uint8_t phase = m->boss_phase;
    if (phase > 2) phase = 2;

    /* Find current skill in cycle */
    uint16_t skill_id = tmpl->phase_skills[phase][m->skill_cycle_index];
    if (skill_id == 0) {
        /* Cycle back to start */
        m->skill_cycle_index = 0;
        skill_id = tmpl->phase_skills[phase][0];
    }

    /* Advance cycle for next call */
    m->skill_cycle_index++;
    if (m->skill_cycle_index >= BOSS_CYCLE_MAX_SKILLS ||
        tmpl->phase_skills[phase][m->skill_cycle_index] == 0) {
        m->skill_cycle_index = 0;
    }

    return skill_id;
}

/* Apply armor break to boss */
static void boss_apply_armor_break(MonsterInfo* m) {
    if (!m->is_boss || m->armor_broken) return;

    const BossTemplate* tmpl = find_boss_template(m->boss_id);
    if (!tmpl) return;

    m->armor_broken = 1;
    m->armor_break_timer = tmpl->armor_break_duration;

    /* Reduce defense by 50% */
    m->phys_def = m->phys_def / 2;
    m->mag_def = m->mag_def / 2;

    printf("[BOSS] Armor BROKEN! DEF reduced for %u ticks\n", m->armor_break_timer);
}

/* Update armor break timer */
static void boss_update_armor_break(MonsterInfo* m) {
    if (!m->is_boss || !m->armor_broken) return;

    if (m->armor_break_timer > 0) {
        m->armor_break_timer--;
    }

    if (m->armor_break_timer == 0) {
        m->armor_broken = 0;

        /* Restore defense */
        const BossTemplate* tmpl = find_boss_template(m->boss_id);
        if (tmpl) {
            float diff_mult = 1.0f;  /* Would need dungeon difficulty here */
            m->phys_def = (uint32_t)(tmpl->base_phys_def * diff_mult);
            m->mag_def = (uint32_t)(tmpl->base_mag_def * diff_mult);
        }

        printf("[BOSS] Armor restored!\n");
    }
}

/* Calculate boss weakness damage bonus */
static uint32_t boss_apply_weakness(MonsterInfo* m, uint32_t damage, uint8_t attack_element) {
    if (!m->is_boss) return damage;

    if (attack_element != ELEMENT_NONE && attack_element == m->weak_element) {
        /* 50% extra damage on weakness */
        uint32_t bonus = damage * 50 / 100;
        printf("[BOSS] WEAKNESS HIT! +%u damage (element=%u)\n", bonus, attack_element);
        return damage + bonus;
    }

    return damage;
}

/* Enhanced enrage check with boss phase support */
static void monster_check_enrage(MonsterInfo* m) {
    if (!m->alive) return;

    /* For bosses, use phase system instead */
    if (m->is_boss) {
        boss_check_phase_transition(m);
        boss_update_armor_break(m);
        return;
    }

    /* Check if already enraged (non-boss) */
    if (m->enraged) return;

    /* Enrage at 30% HP */
    uint32_t enrage_threshold = m->max_hp * 30 / 100;
    if (m->hp <= enrage_threshold) {
        m->enraged = 1;
        /* Boost attack when enraged */
        m->attack_damage = m->attack_damage * 130 / 100;
        m->attack_cooldown = 0;  /* Reset cooldown for immediate attack */
    }
}

/* Select best skill based on distance, cooldowns, and HP */
static uint8_t monster_select_skill(MonsterInfo* m, uint32_t dist_to_player) {
    if (!m->alive) return 0xFF;

    /* Update cooldowns */
    for (int i = 0; i < 4; i++) {
        if (m->skill_cooldowns[i] > 0) {
            m->skill_cooldowns[i]--;
        }
    }

    /* Enraged monsters prefer enrage skill */
    if (m->enraged && m->skill_cooldowns[MONSTER_SKILL_SLOT_ENRAGE] == 0) {
        return MONSTER_SKILL_SLOT_ENRAGE;
    }

    /* Check special skill (often has condition) */
    if (m->skill_cooldowns[MONSTER_SKILL_SLOT_SPECIAL] == 0) {
        /* Use special when HP low */
        if (m->hp < m->max_hp / 2) {
            return MONSTER_SKILL_SLOT_SPECIAL;
        }
    }

    /* Check AOE if multiple targets nearby (simplified - always available) */
    if (m->skill_cooldowns[MONSTER_SKILL_SLOT_AOE] == 0) {
        /* Use AOE at medium range */
        if (dist_to_player > m->attack_range / 2 && dist_to_player <= m->attack_range) {
            return MONSTER_SKILL_SLOT_AOE;
        }
    }

    /* Default to normal attack */
    if (m->skill_cooldowns[MONSTER_SKILL_SLOT_NORMAL] == 0) {
        return MONSTER_SKILL_SLOT_NORMAL;
    }

    return 0xFF;  /* No skill available */
}

/* Execute monster skill - returns damage output info */
static MonsterAttackResult monster_execute_skill(MonsterInfo* m, uint8_t skill_slot) {
    MonsterAttackResult result;
    memset(&result, 0, sizeof(result));

    if (!m->alive || skill_slot >= 4) return result;

    /* Base damage from monster stats */
    uint32_t base_damage = m->attack_damage;

    /* Apply skill modifiers based on slot */
    switch (skill_slot) {
        case MONSTER_SKILL_SLOT_NORMAL:
            /* Normal attack - 100% damage */
            result.damage = base_damage;
            result.aoe_radius = 0;
            m->skill_cooldowns[skill_slot] = 20;  /* ~0.7s at 30fps */
            break;

        case MONSTER_SKILL_SLOT_AOE:
            /* AOE attack - 80% damage but hits area */
            result.damage = base_damage * 80 / 100;
            result.aoe_radius = 100;
            m->skill_cooldowns[skill_slot] = 90;  /* 3s cooldown */
            break;

        case MONSTER_SKILL_SLOT_SPECIAL:
            /* Special - 120% damage with debuff */
            result.damage = base_damage * 120 / 100;
            result.aoe_radius = 0;
            result.debuff_type = DEBUFF_SLOW;
            result.debuff_duration = 3000;  /* 3 seconds */
            result.debuff_value = -30;      /* -30% speed */
            m->skill_cooldowns[skill_slot] = 150;  /* 5s cooldown */
            break;

        case MONSTER_SKILL_SLOT_ENRAGE:
            /* Enrage skill - 150% damage, small AOE */
            result.damage = base_damage * 150 / 100;
            result.aoe_radius = 50;
            result.debuff_type = DEBUFF_STUN;
            result.debuff_duration = 1000;  /* 1 second stun */
            result.debuff_value = 0;
            m->skill_cooldowns[skill_slot] = 180;  /* 6s cooldown */
            break;
    }

    result.skill_used = skill_slot + 1;  /* 1-based for non-zero check */

    /* Set cast time (simplified - instant for now) */
    m->current_skill = skill_slot;
    m->cast_timer = 0;
    m->recovery_timer = 15;  /* Brief recovery after attack */

    return result;
}

/* Enhanced AI update with skill selection */
MonsterAttackResult monster_update_ai_v2(MonsterInfo* m, uint16_t player_x,
                                         uint16_t player_y, uint32_t player_charac_no,
                                         uint32_t current_time) {
    MonsterAttackResult result;
    memset(&result, 0, sizeof(result));

    if (!m->alive) return result;

    /* Check for phase transition */
    monster_check_enrage(m);

    /* Handle recovery timer */
    if (m->recovery_timer > 0) {
        m->recovery_timer--;
        return result;
    }

    uint32_t dist_to_player = monster_distance(m->x, m->y, player_x, player_y);

    /* Reduce attack cooldown */
    if (m->attack_cooldown > 0) m->attack_cooldown--;

    /* Handle hit stun */
    if (m->hit_stun_frames > 0) {
        m->hit_stun_frames--;
        m->state = MONSTER_STATE_HIT_STUN;
        return result;
    }

    switch (m->state) {
        case MONSTER_STATE_IDLE:
            if (dist_to_player <= m->aggro_range) {
                m->state = MONSTER_STATE_CHASE;
                m->target_player = player_charac_no;
            }
            break;

        case MONSTER_STATE_CHASE:
            m->target_x = player_x;
            m->target_y = player_y;

            if (dist_to_player <= m->attack_range && m->attack_cooldown == 0) {
                m->state = MONSTER_STATE_ATTACK;
            } else if (dist_to_player > m->aggro_range * 2) {
                m->state = MONSTER_STATE_PATROL;
                m->target_player = 0;
            } else {
                monster_move_towards(m, player_x, player_y);
            }
            break;

        case MONSTER_STATE_ATTACK:
            if (m->attack_cooldown == 0 && dist_to_player <= m->attack_range) {
                /* Select and execute skill */
                uint8_t skill = monster_select_skill(m, dist_to_player);
                if (skill != 0xFF) {
                    result = monster_execute_skill(m, skill);
                    m->attack_cooldown = 30;
                }
            } else {
                m->state = MONSTER_STATE_CHASE;
            }
            break;

        case MONSTER_STATE_PATROL:
            if (monster_distance(m->x, m->y, m->spawn_x, m->spawn_y) > 10) {
                monster_move_towards(m, m->spawn_x, m->spawn_y);
            } else {
                m->state = MONSTER_STATE_IDLE;
            }
            if (dist_to_player <= m->aggro_range) {
                m->state = MONSTER_STATE_CHASE;
                m->target_player = player_charac_no;
            }
            break;

        default:
            break;
    }

    m->last_action_time = current_time;
    return result;
}


/*==============================================================================
 * Phase 16 P2: Multi-Target Aggro System
 *============================================================================*/

/**
 * Add aggro for a player when they deal damage
 * @param m Monster info
 * @param player_id Player character ID
 * @param damage Damage dealt (adds to aggro)
 */
void monster_add_aggro(MonsterInfo* m, uint32_t player_id, int32_t damage) {
    if (!m || player_id == 0 || damage <= 0) return;

    /* Find existing entry or empty slot */
    int found_idx = -1;
    int empty_idx = -1;

    for (int i = 0; i < m->aggro_count && i < MAX_AGGRO_TARGETS; i++) {
        if (m->aggro_list[i].player_id == player_id) {
            found_idx = i;
            break;
        }
    }

    if (found_idx == -1) {
        /* Find empty slot */
        for (int i = 0; i < MAX_AGGRO_TARGETS; i++) {
            if (m->aggro_list[i].player_id == 0) {
                empty_idx = i;
                break;
            }
        }
    }

    if (found_idx >= 0) {
        /* Update existing entry */
        m->aggro_list[found_idx].aggro_value += damage;
        m->aggro_list[found_idx].last_damage_time = m->last_action_time;
    } else if (empty_idx >= 0) {
        /* Add new entry */
        m->aggro_list[empty_idx].player_id = player_id;
        m->aggro_list[empty_idx].aggro_value = damage;
        m->aggro_list[empty_idx].last_damage_time = m->last_action_time;
        if (empty_idx >= m->aggro_count) {
            m->aggro_count = empty_idx + 1;
        }
    } else if (m->aggro_count < MAX_AGGRO_TARGETS) {
        /* Append to list */
        int idx = m->aggro_count;
        m->aggro_list[idx].player_id = player_id;
        m->aggro_list[idx].aggro_value = damage;
        m->aggro_list[idx].last_damage_time = m->last_action_time;
        m->aggro_count++;
    }
    /* else: list is full, ignore (could replace lowest) */
}

/**
 * Get the player with highest aggro
 * @param m Monster info
 * @return Player ID with highest aggro, or 0 if none
 */
uint32_t monster_get_highest_aggro_target(MonsterInfo* m) {
    if (!m || m->aggro_count == 0) return 0;

    int32_t max_aggro = 0;
    uint32_t target = 0;

    for (int i = 0; i < m->aggro_count && i < MAX_AGGRO_TARGETS; i++) {
        if (m->aggro_list[i].player_id != 0 &&
            m->aggro_list[i].aggro_value > max_aggro) {
            max_aggro = m->aggro_list[i].aggro_value;
            target = m->aggro_list[i].player_id;
        }
    }

    return target;
}

/**
 * Decay all aggro values (call every second)
 * @param m Monster info
 * @param decay_percent Percentage to decay (e.g., 5 = 5%)
 */
void monster_decay_aggro(MonsterInfo* m, int decay_percent) {
    if (!m || decay_percent <= 0) return;

    for (int i = 0; i < m->aggro_count && i < MAX_AGGRO_TARGETS; i++) {
        if (m->aggro_list[i].player_id != 0) {
            int32_t decay = m->aggro_list[i].aggro_value * decay_percent / 100;
            if (decay < 1) decay = 1;
            m->aggro_list[i].aggro_value -= decay;

            /* Remove entry if aggro drops to 0 or below */
            if (m->aggro_list[i].aggro_value <= 0) {
                m->aggro_list[i].player_id = 0;
                m->aggro_list[i].aggro_value = 0;
            }
        }
    }
}

/**
 * Clear all aggro (monster reset or death)
 * @param m Monster info
 */
void monster_clear_aggro(MonsterInfo* m) {
    if (!m) return;

    for (int i = 0; i < MAX_AGGRO_TARGETS; i++) {
        m->aggro_list[i].player_id = 0;
        m->aggro_list[i].aggro_value = 0;
        m->aggro_list[i].last_damage_time = 0;
    }
    m->aggro_count = 0;
    m->target_player = 0;
}

/**
 * Update monster target based on aggro (call in AI update)
 * Updates m->target_player to highest aggro target
 * @param m Monster info
 */
void monster_update_target_from_aggro(MonsterInfo* m) {
    if (!m) return;

    uint32_t new_target = monster_get_highest_aggro_target(m);
    if (new_target != 0) {
        m->target_player = new_target;
    }
}
