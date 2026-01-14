#include "systems/buff_system.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "systems/time_utils.h"
/* Buff info structure */
typedef struct {
    uint16_t buff_id;
    const char* name;
    uint8_t is_debuff;            /* 1 = debuff, 0 = buff */
    uint8_t max_stacks;           /* Maximum stack count */
    uint8_t can_dispel;           /* 1 = can be dispelled */
    uint16_t default_duration_ms; /* Default duration */
    int16_t  default_value;       /* Default effect value */
} BuffInfo;

/* Buff data table */
static const BuffInfo g_buff_table[] = {
    /* Positive buffs */
    {BUFF_ATTACK_UP, "Attack Up", 0, 1, 1, 30000, 20},
    {BUFF_MAGIC_UP, "Magic Up", 0, 1, 1, 30000, 20},
    {BUFF_DEFENSE_UP, "Defense Up", 0, 1, 1, 30000, 25},
    {BUFF_MAGIC_DEF_UP, "Magic Defense Up", 0, 1, 1, 30000, 25},
    {BUFF_SPEED_UP, "Speed Up", 0, 1, 1, 20000, 30},
    {BUFF_ATTACK_SPEED_UP, "Attack Speed Up", 0, 1, 1, 20000, 15},
    {BUFF_CRIT_UP, "Critical Up", 0, 1, 1, 30000, 10},
    {BUFF_HP_REGEN, "HP Regeneration", 0, 1, 1, 60000, 50},
    {BUFF_MP_REGEN, "MP Regeneration", 0, 1, 1, 60000, 30},
    {BUFF_ALL_STATS_UP, "All Stats Up", 0, 1, 1, 30000, 10},
    {BUFF_INVINCIBLE, "Invincible", 0, 1, 0, 3000, 0},
    {BUFF_SUPER_ARMOR, "Super Armor", 0, 1, 0, 5000, 0},

    /* Debuffs */
    {DEBUFF_ATTACK_DOWN, "Attack Down", 1, 1, 1, 15000, -15},
    {DEBUFF_DEFENSE_DOWN, "Defense Down", 1, 1, 1, 15000, -20},
    {DEBUFF_SPEED_DOWN, "Speed Down", 1, 1, 1, 10000, -30},
    {DEBUFF_POISON, "Poison", 1, 5, 1, 10000, 2},      /* 2% max HP per tick */
    {DEBUFF_BURN, "Burn", 1, 3, 1, 8000, 100},         /* 100 fire damage per tick */
    {DEBUFF_FREEZE, "Freeze", 1, 1, 0, 3000, 0},
    {DEBUFF_STUN, "Stun", 1, 1, 0, 2000, 0},
    {DEBUFF_SILENCE, "Silence", 1, 1, 1, 5000, 0},
    {DEBUFF_BLIND, "Blind", 1, 1, 1, 8000, -30},
    {DEBUFF_BLEED, "Bleed", 1, 5, 1, 12000, 80},       /* 80 physical damage per tick */
    {DEBUFF_CURSE, "Curse", 1, 1, 1, 20000, -10},
    {DEBUFF_SLOW, "Slow", 1, 1, 1, 8000, -20},

    /* Special buffs */
    {BUFF_SHIELD, "Shield", 0, 1, 1, 30000, 500},     /* 500 HP shield */
    {BUFF_LIFESTEAL, "Lifesteal", 0, 1, 1, 20000, 10}, /* 10% lifesteal */
    {BUFF_REFLECT, "Damage Reflect", 0, 1, 1, 15000, 20}, /* 20% reflect */
    {BUFF_ELEMENTAL_FIRE, "Fire Enchant", 0, 1, 1, 60000, 0},
    {BUFF_ELEMENTAL_ICE, "Ice Enchant", 0, 1, 1, 60000, 0},
    {BUFF_ELEMENTAL_LIGHT, "Light Enchant", 0, 1, 1, 60000, 0},
    {BUFF_ELEMENTAL_DARK, "Dark Enchant", 0, 1, 1, 60000, 0},

    /* End marker */
    {0, NULL, 0, 0, 0, 0, 0}
};

/* Find buff info by ID */
static const BuffInfo* find_buff_info(uint16_t buff_id) {
    for (int i = 0; g_buff_table[i].name != NULL; i++) {
        if (g_buff_table[i].buff_id == buff_id) {
            return &g_buff_table[i];
        }
    }
    return NULL;
}

/* Add a buff to a player */
int buff_add(ClientSession* s, uint16_t buff_id, uint32_t source_id,
                    int32_t value, uint32_t duration_ms) {
    if (!s) return -1;

    const BuffInfo* info = find_buff_info(buff_id);
    if (!info) return -1;

    uint32_t now = get_current_time_ms();
    uint32_t expire_time = now + duration_ms;

    /* Check if buff already exists */
    for (int i = 0; i < s->buff_count; i++) {
        if (s->active_buffs[i].buff_id == buff_id) {
            /* Buff exists - check stacking behavior */
            if (info->max_stacks > 1 && s->active_buffs[i].stacks < info->max_stacks) {
                /* Add stack */
                s->active_buffs[i].stacks++;
                s->active_buffs[i].expire_time = expire_time;  /* Refresh duration */
                printf("[BUFF] Added stack to %s (stacks: %d)\n",
                       info->name, s->active_buffs[i].stacks);
            } else {
                /* Refresh duration only */
                s->active_buffs[i].expire_time = expire_time;
                printf("[BUFF] Refreshed %s duration\n", info->name);
            }
            return 0;
        }
    }

    /* Add new buff */
    if (s->buff_count >= MAX_ACTIVE_BUFFS) {
        printf("[BUFF] Cannot add buff - max buffs reached\n");
        return -1;
    }

    s->active_buffs[s->buff_count].buff_id = buff_id;
    s->active_buffs[s->buff_count].source_id = source_id;
    s->active_buffs[s->buff_count].expire_time = expire_time;
    s->active_buffs[s->buff_count].value = value;
    s->active_buffs[s->buff_count].stacks = 1;
    s->buff_count++;

    printf("[BUFF] Applied %s (value: %d, duration: %ums)\n",
           info->name, value, duration_ms);
    return 0;
}

/* Remove a buff from a player */
int buff_remove(ClientSession* s, uint16_t buff_id) {
    if (!s) return -1;

    for (int i = 0; i < s->buff_count; i++) {
        if (s->active_buffs[i].buff_id == buff_id) {
            /* Shift remaining buffs */
            for (int j = i; j < s->buff_count - 1; j++) {
                s->active_buffs[j] = s->active_buffs[j + 1];
            }
            s->buff_count--;

            const BuffInfo* info = find_buff_info(buff_id);
            printf("[BUFF] Removed %s\n", info ? info->name : "unknown buff");
            return 0;
        }
    }
    return -1;  /* Buff not found */
}

/* Check if player has a specific buff */
int buff_has(ClientSession* s, uint16_t buff_id) {
    if (!s) return 0;

    uint32_t now = get_current_time_ms();
    for (int i = 0; i < s->buff_count; i++) {
        if (s->active_buffs[i].buff_id == buff_id &&
            s->active_buffs[i].expire_time > now) {
            return 1;
        }
    }
    return 0;
}

/* Get buff value (with stacks) */
static int32_t buff_get_value(ClientSession* s, uint16_t buff_id) {
    if (!s) return 0;

    uint32_t now = get_current_time_ms();
    for (int i = 0; i < s->buff_count; i++) {
        if (s->active_buffs[i].buff_id == buff_id &&
            s->active_buffs[i].expire_time > now) {
            return s->active_buffs[i].value * s->active_buffs[i].stacks;
        }
    }
    return 0;
}

/* Update buffs - expire old ones, process DOTs */
void buff_update(ClientSession* s) {
    if (!s) return;

    uint32_t now = get_current_time_ms();
    int i = 0;

    while (i < s->buff_count) {
        if (s->active_buffs[i].expire_time <= now) {
            /* Buff expired - remove it */
            const BuffInfo* info = find_buff_info(s->active_buffs[i].buff_id);
            printf("[BUFF] %s expired\n", info ? info->name : "unknown");

            for (int j = i; j < s->buff_count - 1; j++) {
                s->active_buffs[j] = s->active_buffs[j + 1];
            }
            s->buff_count--;
            /* Don't increment i - check the shifted buff */
        } else {
            i++;
        }
    }
}

/* Process DOT (Damage Over Time) and HOT (Heal Over Time) effects - call every tick */
void buff_process_dots_and_hots(ClientSession* s, uint32_t delta_ms) {
    if (!s) return;

    uint32_t now = get_current_time_ms();

    for (int i = 0; i < s->buff_count; i++) {
        if (s->active_buffs[i].expire_time <= now) continue;

        uint16_t buff_id = s->active_buffs[i].buff_id;
        int32_t value = s->active_buffs[i].value * s->active_buffs[i].stacks;

        /* DOT debuffs - deal damage over time */
        if (buff_id == DEBUFF_POISON) {
            /* Poison: % max HP damage per second */
            uint32_t dot_damage = (s->combat_stats.max_hp * value) / 100;
            dot_damage = (dot_damage * delta_ms) / 1000;  /* Scale by time */
            if (dot_damage > 0 && s->current_hp > dot_damage) {
                s->current_hp -= dot_damage;
                printf("[DOT] Poison dealt %u damage, HP=%u\n", dot_damage, s->current_hp);
            }
        } else if (buff_id == DEBUFF_BURN) {
            /* Burn: fixed fire damage per second */
            uint32_t dot_damage = (value * delta_ms) / 1000;
            if (dot_damage > 0 && s->current_hp > dot_damage) {
                s->current_hp -= dot_damage;
                printf("[DOT] Burn dealt %u damage, HP=%u\n", dot_damage, s->current_hp);
            }
        } else if (buff_id == DEBUFF_BLEED) {
            /* Bleed: fixed physical damage per second */
            uint32_t dot_damage = (value * delta_ms) / 1000;
            if (dot_damage > 0 && s->current_hp > dot_damage) {
                s->current_hp -= dot_damage;
                printf("[DOT] Bleed dealt %u damage, HP=%u\n", dot_damage, s->current_hp);
            }
        }
        /* HOT buffs - heal over time */
        else if (buff_id == BUFF_REGEN) {
            /* Regeneration: % max HP heal per second */
            uint32_t hot_heal = (s->combat_stats.max_hp * value) / 100;
            hot_heal = (hot_heal * delta_ms) / 1000;
            if (hot_heal > 0) {
                s->current_hp += hot_heal;
                if (s->current_hp > s->combat_stats.max_hp) {
                    s->current_hp = s->combat_stats.max_hp;
                }
                printf("[HOT] Regen healed %u, HP=%u\n", hot_heal, s->current_hp);
            }
        } else if (buff_id == BUFF_HEAL_OVER_TIME) {
            /* Fixed HP heal per second */
            uint32_t hot_heal = (value * delta_ms) / 1000;
            if (hot_heal > 0) {
                s->current_hp += hot_heal;
                if (s->current_hp > s->combat_stats.max_hp) {
                    s->current_hp = s->combat_stats.max_hp;
                }
            }
        }
    }
}

/* Legacy wrapper for compatibility */
void buff_process_dots(ClientSession* s, uint32_t delta_ms) {
    buff_process_dots_and_hots(s, delta_ms);
}

/* Calculate total buff modifier for a stat type */
int32_t buff_get_stat_modifier(ClientSession* s, BuffType stat_buff) {
    if (!s) return 0;

    int32_t total = 0;
    uint32_t now = get_current_time_ms();

    for (int i = 0; i < s->buff_count; i++) {
        if (s->active_buffs[i].expire_time <= now) continue;

        if (s->active_buffs[i].buff_id == stat_buff) {
            total += s->active_buffs[i].value * s->active_buffs[i].stacks;
        }
    }
    return total;
}

/* Dispel all debuffs from a player */
static int buff_dispel_debuffs(ClientSession* s) {
    if (!s) return 0;

    int removed = 0;
    int i = 0;

    while (i < s->buff_count) {
        const BuffInfo* info = find_buff_info(s->active_buffs[i].buff_id);
        if (info && info->is_debuff && info->can_dispel) {
            /* Remove this debuff */
            for (int j = i; j < s->buff_count - 1; j++) {
                s->active_buffs[j] = s->active_buffs[j + 1];
            }
            s->buff_count--;
            removed++;
        } else {
            i++;
        }
    }

    printf("[BUFF] Dispelled %d debuffs\n", removed);
    return removed;
}

/* Alias for buff_dispel_debuffs (Phase 14 compatibility) */
int buff_remove_debuffs(ClientSession* s) {
    return buff_dispel_debuffs(s);
}

/* Clear all buffs (on death, etc.) */
void buff_clear_all(ClientSession* s) {
    if (!s) return;
    s->buff_count = 0;
    printf("[BUFF] Cleared all buffs\n");
}

/* Apply buff modifiers to combat stats (call after equipment changes or buff changes) */
void buff_apply_stat_modifiers(ClientSession* s) {
    if (!s) return;

    /* Store base stats (without buff modifiers) - assuming base_stats exists */
    /* For now, apply percentage modifiers directly */

    /* Attack buffs */
    int32_t attack_mod = buff_get_stat_modifier(s, BUFF_ATTACK_UP);
    int32_t magic_mod = buff_get_stat_modifier(s, BUFF_MAGIC_UP);
    int32_t all_mod = buff_get_stat_modifier(s, BUFF_ALL_STATS_UP);

    /* Defense buffs */
    int32_t def_mod = buff_get_stat_modifier(s, BUFF_DEFENSE_UP);
    int32_t mdef_mod = buff_get_stat_modifier(s, BUFF_MAGIC_DEF_UP);

    /* Speed buffs */
    int32_t speed_mod = buff_get_stat_modifier(s, BUFF_SPEED_UP);
    int32_t aspeed_mod = buff_get_stat_modifier(s, BUFF_ATTACK_SPEED_UP);

    /* Crit buff */
    int32_t crit_mod = buff_get_stat_modifier(s, BUFF_CRIT_UP);

    /* Debuff modifiers */
    int32_t attack_down = buff_get_stat_modifier(s, DEBUFF_ATTACK_DOWN);
    int32_t def_down = buff_get_stat_modifier(s, DEBUFF_DEFENSE_DOWN);
    int32_t speed_down = buff_get_stat_modifier(s, DEBUFF_SPEED_DOWN);
    int32_t curse_mod = buff_get_stat_modifier(s, DEBUFF_CURSE);
    int32_t slow_mod = buff_get_stat_modifier(s, DEBUFF_SLOW);

    /* Berserk special buff */
    int berserk_active = buff_has(s, BUFF_BERSERK);

    /* Calculate net modifiers */
    int32_t net_attack = attack_mod + all_mod - attack_down - curse_mod + (berserk_active ? 30 : 0);
    int32_t net_magic = magic_mod + all_mod - curse_mod;
    int32_t net_def = def_mod + all_mod - def_down - curse_mod - (berserk_active ? 20 : 0);
    int32_t net_mdef = mdef_mod + all_mod - curse_mod;
    int32_t net_speed = speed_mod - speed_down;
    int32_t net_aspeed = aspeed_mod - slow_mod;
    int32_t net_crit = crit_mod + (berserk_active ? 15 : 0);

    /* Apply to combat stats (percentage modifiers) */
    /* These modify the effective stats used in combat calculations */
    s->combat_stats.buff_attack_mod = net_attack;
    s->combat_stats.buff_magic_mod = net_magic;
    s->combat_stats.buff_def_mod = net_def;
    s->combat_stats.buff_mdef_mod = net_mdef;
    s->combat_stats.buff_speed_mod = net_speed;
    s->combat_stats.buff_aspeed_mod = net_aspeed;
    s->combat_stats.buff_crit_mod = net_crit;
}

/* Get effective stat with buff modifier applied */
static uint32_t get_effective_stat(uint32_t base_stat, int32_t buff_percent) {
    if (buff_percent == 0) return base_stat;
    int32_t modified = (int32_t)base_stat * (100 + buff_percent) / 100;
    return (modified > 0) ? (uint32_t)modified : 1;
}
