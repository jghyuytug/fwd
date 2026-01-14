#include "systems/dungeon_manager.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "systems/buff_system.h"
#include "systems/combat_system.h"
#include "systems/monster_ai.h"
/*------------------------------------------------------------------------------
 * Phase 10 Task 3: Player Death and Resurrection System
 *----------------------------------------------------------------------------*/

/* Item IDs for resurrection consumables */
#define ITEM_RESURRECT_COIN     3000    /* Basic resurrect coin */
#define ITEM_RESURRECT_TOKEN    3001    /* Premium resurrect token (full HP) */

/* Check if player is dead */
int player_is_dead(ClientSession* s) {
    if (!s) return 0;
    return s->current_hp == 0;
}

/* Kill player - set HP to 0 and notify */
void player_die(ClientSession* s) {
    if (!s) return;
    s->current_hp = 0;

    /* Phase 12: Track death for achievement system */
    if (s->state == STATE_IN_DUNGEON) {
        s->dungeon.death_count++;
        printf("[DEATH] Player %u died in dungeon. Death count: %u\n",
               s->character_id, s->dungeon.death_count);
    }
}

/* Check if player has resurrect items */
static int player_has_resurrect_item(ClientSession* s) {
    if (!s) return 0;

    /* Check inventory for resurrect coins */
    for (int i = 0; i < s->inventory.item_count; i++) {
        if (s->inventory.items[i].it_id == ITEM_RESURRECT_COIN ||
            s->inventory.items[i].it_id == ITEM_RESURRECT_TOKEN) {
            if (s->inventory.items[i].count > 0) {
                return (int)s->inventory.items[i].it_id;
            }
        }
    }
    return 0;
}

/* Consume resurrect item from inventory */
static int player_consume_resurrect_item(ClientSession* s, uint32_t item_id) {
    if (!s) return 0;

    for (int i = 0; i < s->inventory.item_count; i++) {
        if (s->inventory.items[i].it_id == item_id &&
            s->inventory.items[i].count > 0) {
            s->inventory.items[i].count--;
            return 1;
        }
    }
    return 0;
}

/* Resurrect player with specified HP percentage */
ResurrectResult player_resurrect(ClientSession* s, DungeonInstance* d,
                                         uint8_t hp_percent) {
    if (!s) return RESURRECT_DUNGEON_FAILED;

    /* Check if already alive */
    if (s->current_hp > 0) {
        return RESURRECT_ALREADY_ALIVE;
    }

    /* Check for free resurrect first (once per dungeon) */
    if (d && !d->free_resurrect_used) {
        d->free_resurrect_used = 1;
        s->current_hp = s->combat_stats.max_hp * hp_percent / 100;
        if (s->current_hp < 1) s->current_hp = 1;
        return RESURRECT_OK;
    }

    /* Check for resurrect coin */
    int item_id = player_has_resurrect_item(s);
    if (item_id == 0) {
        return RESURRECT_NO_COINS;
    }

    /* Consume the item */
    if (!player_consume_resurrect_item(s, (uint32_t)item_id)) {
        return RESURRECT_NO_COINS;
    }

    /* Resurrect with HP based on item type */
    if (item_id == ITEM_RESURRECT_TOKEN) {
        /* Premium token - full HP */
        s->current_hp = s->combat_stats.max_hp;
    } else {
        /* Basic coin - partial HP */
        s->current_hp = s->combat_stats.max_hp * hp_percent / 100;
        if (s->current_hp < 1) s->current_hp = 1;
    }

    return RESURRECT_OK;
}

/* Handle dungeon failure when player dies without resurrection */
static void dungeon_fail(DungeonInstance* d) {
    if (!d) return;
    d->active = 0;
    /* Mark dungeon as failed - no rewards */
}

/* Process player damage and check for death */
int player_take_damage(ClientSession* s, DungeonInstance* d,
                               uint32_t damage, uint8_t* died) {
    if (!s || !died) return 0;
    *died = 0;

    /* Check invincibility */
    if (buff_has(s, BUFF_INVINCIBLE)) {
        return 0;  /* No damage taken */
    }

    /* Apply shield absorption */
    damage = process_shield(s, damage);
    if (damage == 0) return 0;

    /* Apply defense reduction */
    uint32_t defense = s->combat_stats.phys_def;
    if (damage > defense) {
        damage -= defense;
    } else {
        damage = damage / 10;
        if (damage < 1) damage = 1;
    }

    /* Apply damage */
    if (s->current_hp > damage) {
        s->current_hp -= damage;
    } else {
        /* Player dies */
        player_die(s);
        *died = 1;
    }

    return (int)damage;
}

/*------------------------------------------------------------------------------
 * Phase 10 Task 4: Anti-Cheat Detection System
 *----------------------------------------------------------------------------*/

/* Anti-cheat violation types */
typedef enum {
    CHEAT_NONE = 0,
    CHEAT_SPEED_HACK = 1,      /* Moving too fast */
    CHEAT_TELEPORT = 2,        /* Position jump too large */
    CHEAT_DAMAGE_HACK = 3,     /* Damage exceeds maximum possible */
    CHEAT_COOLDOWN_HACK = 4,   /* Skills used faster than cooldown */
    CHEAT_POSITION_INVALID = 5 /* Position outside map bounds */
} CheatType;

/* Violation record */
typedef struct {
    CheatType type;
    uint32_t timestamp;
    uint32_t value;         /* Suspicious value */
    uint32_t expected_max;  /* Expected maximum */
} CheatViolation;

/* Anti-cheat state per session */
typedef struct {
    uint16_t last_x;
    uint16_t last_y;
    uint32_t last_move_time;
    uint32_t last_attack_time;
    uint32_t violation_count;
    uint8_t  kick_pending;
} AntiCheatState;

/* Maximum allowed values */
#define MAX_MOVE_SPEED_PPS      800   /* Max pixels per second */
#define MAX_TELEPORT_DISTANCE   500   /* Max instant move distance */
#define MAX_DAMAGE_MULTIPLIER   5     /* Max damage = base * this */
#define MIN_ATTACK_INTERVAL_MS  100   /* Minimum ms between attacks */

/* Check for speed hack */
static CheatType anticheat_check_speed(uint16_t old_x, uint16_t old_y,
                                        uint16_t new_x, uint16_t new_y,
                                        uint32_t time_delta_ms) {
    if (time_delta_ms == 0) time_delta_ms = 1;

    /* Calculate distance moved */
    int dx = (int)new_x - (int)old_x;
    int dy = (int)new_y - (int)old_y;
    uint32_t dist = (uint32_t)(dx * dx + dy * dy);
    dist = (uint32_t)sqrt((double)dist);

    /* Check for teleport (instant large distance) */
    if (dist > MAX_TELEPORT_DISTANCE && time_delta_ms < 100) {
        return CHEAT_TELEPORT;
    }

    /* Calculate speed in pixels per second */
    uint32_t speed_pps = (dist * 1000) / time_delta_ms;
    if (speed_pps > MAX_MOVE_SPEED_PPS) {
        return CHEAT_SPEED_HACK;
    }

    return CHEAT_NONE;
}

/* Check for damage hack */
static CheatType anticheat_check_damage(uint32_t damage_dealt,
                                         uint32_t player_attack,
                                         uint8_t skill_level) {
    /* Calculate maximum theoretical damage */
    uint32_t max_base = player_attack * 3;  /* Highest skill multiplier ~300% */
    max_base += skill_level * 100;          /* Skill level bonus */
    max_base = max_base * 2;                /* Critical hit */
    max_base = max_base * MAX_DAMAGE_MULTIPLIER; /* Safety margin */

    if (damage_dealt > max_base) {
        return CHEAT_DAMAGE_HACK;
    }

    return CHEAT_NONE;
}

/* Check position bounds */
static CheatType anticheat_check_position(uint16_t x, uint16_t y,
                                           uint16_t map_width, uint16_t map_height) {
    if (x > map_width || y > map_height) {
        return CHEAT_POSITION_INVALID;
    }
    return CHEAT_NONE;
}

/* Process movement and check for cheats */
static CheatType anticheat_process_move(ClientSession* s, uint16_t new_x, uint16_t new_y) {
    if (!s) return CHEAT_NONE;

    uint32_t current_time = (uint32_t)time(NULL) * 1000;  /* Convert to ms */

    /* Get previous position (simplified - use dungeon if active) */
    uint16_t old_x = 0, old_y = 0;
    uint32_t last_time = 0;

    if (s->dungeon.active) {
        /* Use dungeon instance for tracking */
        old_x = (uint16_t)(s->dungeon.dungeon_id & 0xFFFF);  /* Placeholder */
        old_y = (uint16_t)((s->dungeon.dungeon_id >> 16) & 0xFFFF);
        last_time = s->dungeon.start_time * 1000;
    }

    /* Skip first move (no reference point) */
    if (last_time == 0) {
        return CHEAT_NONE;
    }

    uint32_t time_delta = current_time - last_time;
    if (time_delta < 10) time_delta = 10;  /* Minimum time delta */

    CheatType result = anticheat_check_speed(old_x, old_y, new_x, new_y, time_delta);

    return result;
}

/* Log cheat violation and take action */
static void anticheat_report_violation(ClientSession* s, CheatType cheat,
                                        uint32_t suspicious_value) {
    if (!s || cheat == CHEAT_NONE) return;

    /* Log the violation - in production would write to DB */
    const char* cheat_names[] = {
        "NONE", "SPEED_HACK", "TELEPORT", "DAMAGE_HACK",
        "COOLDOWN_HACK", "POSITION_INVALID"
    };

    printf("[ANTICHEAT] Account %u (%s) - Violation: %s, Value: %u\n",
           s->account_id, s->account_name,
           cheat_names[cheat], suspicious_value);

    /* Increment violation count (would trigger kick after threshold) */
}

/*------------------------------------------------------------------------------
 * Phase 10 Task 5: Dungeon Timer System
 *----------------------------------------------------------------------------*/

/* Timer state */
typedef enum {
    TIMER_NORMAL = 0,      /* Normal gameplay */
    TIMER_WARNING = 1,     /* Time running low (20% remaining) */
    TIMER_CRITICAL = 2,    /* Very low time (10% remaining) */
    TIMER_EXPIRED = 3      /* Time's up */
} TimerState;

/* Time limits by difficulty (in seconds) */
static const uint32_t g_dungeon_time_limits[] = {
    600,   /* Difficulty 0 (Normal): 10 minutes */
    540,   /* Difficulty 1 (Adventure): 9 minutes */
    480,   /* Difficulty 2 (Warrior): 8 minutes */
    420,   /* Difficulty 3 (King): 7 minutes */
    300    /* Difficulty 4 (Hell): 5 minutes */
};

/* Get time limit for dungeon difficulty */
static uint32_t dungeon_get_time_limit(uint8_t difficulty) {
    if (difficulty > 4) difficulty = 4;
    return g_dungeon_time_limits[difficulty];
}

/* Calculate remaining time */
static uint32_t dungeon_get_remaining_time(DungeonInstance* d) {
    if (!d || !d->active) return 0;

    uint32_t current_time = (uint32_t)time(NULL);
    uint32_t elapsed = current_time - d->start_time;

    if (elapsed >= d->time_limit) {
        return 0;
    }
    return d->time_limit - elapsed;
}

/* Get timer state based on remaining time */
static TimerState dungeon_get_timer_state(DungeonInstance* d) {
    if (!d || !d->active) return TIMER_EXPIRED;

    uint32_t remaining = dungeon_get_remaining_time(d);
    if (remaining == 0) {
        return TIMER_EXPIRED;
    }

    uint32_t warning_threshold = d->time_limit * 20 / 100;  /* 20% */
    uint32_t critical_threshold = d->time_limit * 10 / 100; /* 10% */

    if (remaining <= critical_threshold) {
        return TIMER_CRITICAL;
    }
    if (remaining <= warning_threshold) {
        return TIMER_WARNING;
    }
    return TIMER_NORMAL;
}

/* Check and apply timeout effects */
typedef struct {
    TimerState state;
    uint32_t remaining_seconds;
    uint8_t  boss_enraged;        /* Boss enters enrage at TIMER_CRITICAL */
    uint8_t  reward_penalty;      /* Reward reduction % (0-100) */
} TimerCheckResult;

static TimerCheckResult dungeon_check_timer(DungeonInstance* d) {
    TimerCheckResult result;
    memset(&result, 0, sizeof(result));

    if (!d || !d->active) {
        result.state = TIMER_EXPIRED;
        return result;
    }

    result.remaining_seconds = dungeon_get_remaining_time(d);
    result.state = dungeon_get_timer_state(d);

    switch (result.state) {
        case TIMER_NORMAL:
            result.reward_penalty = 0;
            result.boss_enraged = 0;
            break;

        case TIMER_WARNING:
            /* Minor reward penalty */
            result.reward_penalty = 10;
            result.boss_enraged = 0;
            break;

        case TIMER_CRITICAL:
            /* Bosses enrage, moderate reward penalty */
            result.reward_penalty = 25;
            result.boss_enraged = 1;
            break;

        case TIMER_EXPIRED:
            /* Dungeon fails, no rewards */
            result.reward_penalty = 100;
            result.boss_enraged = 1;
            break;
    }

    return result;
}

/* Apply boss enrage when timer critical */
static void dungeon_apply_timer_enrage(DungeonInstance* d) {
    if (!d || !d->active) return;

    for (int i = 0; i < d->monster_count; i++) {
        MonsterInfo* m = &d->monsters[i];
        if (!m->alive) continue;

        /* Check if this is a boss (simplified: high HP monsters) */
        if (m->max_hp > 10000 && !m->enraged) {
            m->enraged = 1;
            m->attack_damage = m->attack_damage * 150 / 100;
            m->attack_cooldown = 0;
        }
    }
}

/* Process timer tick - call this every second */
static TimerCheckResult dungeon_tick_timer(DungeonInstance* d) {
    TimerCheckResult result = dungeon_check_timer(d);

    /* Apply effects based on state */
    if (result.state == TIMER_CRITICAL && result.boss_enraged) {
        dungeon_apply_timer_enrage(d);
    }

    if (result.state == TIMER_EXPIRED) {
        dungeon_fail(d);
    }

    return result;
}

/* Initialize dungeon timer on entry */
void dungeon_init_timer(DungeonInstance* d, uint8_t difficulty) {
    if (!d) return;
    d->start_time = (uint32_t)time(NULL);
    d->time_limit = dungeon_get_time_limit(difficulty);
}

/* Process all monster AI for a dungeon tick - returns total damage to player */
DungeonTickResult dungeon_tick_monsters_v2(DungeonInstance* d, ClientSession* s,
                                           uint16_t player_x, uint16_t player_y) {
    DungeonTickResult tick_result;
    memset(&tick_result, 0, sizeof(tick_result));

    if (!d || !s) return tick_result;

    uint32_t current_time = (uint32_t)time(NULL);

    /* Check timer first */
    TimerCheckResult timer = dungeon_tick_timer(d);
    if (timer.state == TIMER_EXPIRED) {
        return tick_result;  /* Dungeon failed */
    }

    for (int i = 0; i < d->monster_count; i++) {
        MonsterInfo* m = &d->monsters[i];
        if (!m->alive) continue;

        /* Run enhanced AI with skill selection */
        MonsterAttackResult attack = monster_update_ai_v2(m, player_x, player_y,
                                                          s->character_id, current_time);

        /* Process attack result */
        if (attack.skill_used > 0 && attack.damage > 0) {
            tick_result.attacks_count++;
            tick_result.total_damage += attack.damage;

            /* Apply debuff if any */
            if (attack.debuff_type > 0 && !tick_result.debuff_applied) {
                tick_result.debuff_applied = 1;
                tick_result.debuff_type = attack.debuff_type;
                tick_result.debuff_duration = attack.debuff_duration;

                /* Add debuff to player: buff_add(s, buff_id, source_id, value, duration_ms) */
                buff_add(s, attack.debuff_type, m->monster_id,
                         attack.debuff_value, attack.debuff_duration * 1000);
            }

            printf("[DUNGEON] Monster %u attacks! Skill=%d Damage=%u%s\n",
                   m->monster_id, attack.skill_used - 1, attack.damage,
                   attack.aoe_radius > 0 ? " (AOE)" : "");
        }
    }

    return tick_result;
}

/* Legacy wrapper for compatibility */
static int dungeon_tick_monsters(DungeonInstance* d, uint16_t player_x, uint16_t player_y,
                                  uint32_t player_charac_no, uint8_t* attack_flags) {
    uint32_t current_time = (uint32_t)time(NULL);
    int attacks_pending = 0;

    for (int i = 0; i < d->monster_count; i++) {
        int result = monster_update_ai(&d->monsters[i], player_x, player_y,
                                       player_charac_no, current_time);
        if (result == 2) {
            if (attack_flags && attacks_pending < 8) {
                attack_flags[attacks_pending] = (uint8_t)i;
                attacks_pending++;
            }
        }
    }

    return attacks_pending;
}

/* Helper: Check if room is cleared */
int dungeon_room_cleared(DungeonInstance* d) {
    for (int i = 0; i < d->monster_count; i++) {
        if (d->monsters[i].alive) {
            return 0;
        }
    }
    return 1;
}




