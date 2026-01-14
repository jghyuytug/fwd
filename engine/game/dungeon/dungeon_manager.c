/**
 * @file dungeon_manager.c
 * @brief Dungeon instance management implementation
 */

#include "dungeon_manager.h"
#include <string.h>
#include <stdlib.h>

static const char* difficulty_names[] = {
    "NORMAL",
    "HARD",
    "EXPERT",
    "MASTER",
    "HELL"
};

static const char* state_names[] = {
    "WAITING",
    "LOADING",
    "RUNNING",
    "BOSS",
    "CLEAR",
    "FAILED"
};

static const char* grade_names[] = {
    "F", "D", "C", "B", "A", "S", "SS", "SSS"
};

/* Difficulty multipliers */
static const int exp_multipliers[] = { 100, 120, 150, 200, 300 };
static const int gold_multipliers[] = { 100, 130, 170, 220, 350 };

int dungeon_manager_init(DungeonManager* manager)
{
    if (!manager) {
        return ENGINE_ERROR_PARAM;
    }

    memset(manager, 0, sizeof(DungeonManager));
    manager->next_instance_id = 1;

    return ENGINE_OK;
}

void dungeon_manager_cleanup(DungeonManager* manager)
{
    int i;

    if (!manager) return;

    for (i = 0; i < DUNGEON_MAX_INSTANCES; i++) {
        if (manager->instances[i].active) {
            manager->instances[i].active = 0;
        }
    }

    memset(manager, 0, sizeof(DungeonManager));
}

DungeonInstance* dungeon_create(DungeonManager* manager, u32 dungeon_id,
                                DungeonDifficulty difficulty)
{
    int i;
    DungeonInstance* inst;

    if (!manager) return NULL;
    if (difficulty >= DUNGEON_DIFF_MAX) return NULL;

    /* Find free slot */
    for (i = 0; i < DUNGEON_MAX_INSTANCES; i++) {
        if (!manager->instances[i].active) {
            inst = &manager->instances[i];
            break;
        }
    }

    if (i >= DUNGEON_MAX_INSTANCES) {
        return NULL;  /* No free slots */
    }

    /* Initialize instance */
    memset(inst, 0, sizeof(DungeonInstance));

    inst->instance_id = manager->next_instance_id++;
    inst->dungeon_id = dungeon_id;
    inst->difficulty = difficulty;
    inst->state = DUNGEON_STATE_WAITING;
    inst->active = 1;

    /* Setup rooms based on difficulty */
    inst->room_count = 3 + difficulty;  /* 3-7 rooms based on difficulty */
    if (inst->room_count > DUNGEON_MAX_ROOMS) {
        inst->room_count = DUNGEON_MAX_ROOMS;
    }

    /* Initialize rooms */
    for (i = 0; i < inst->room_count; i++) {
        inst->rooms[i].room_id = (u8)i;
        inst->rooms[i].state = (i == 0) ? ROOM_STATE_OPEN : ROOM_STATE_LOCKED;
        inst->rooms[i].monster_count = 3 + i;  /* More monsters in later rooms */
        inst->rooms[i].monsters_alive = inst->rooms[i].monster_count;
        inst->rooms[i].is_boss_room = (i == inst->room_count - 1) ? 1 : 0;
    }

    inst->current_room = 0;
    manager->instance_count++;

    return inst;
}

void dungeon_destroy(DungeonManager* manager, DungeonInstance* instance)
{
    if (!manager || !instance) return;
    if (!instance->active) return;

    instance->active = 0;
    manager->instance_count--;
}

DungeonInstance* dungeon_get(DungeonManager* manager, u32 instance_id)
{
    int i;

    if (!manager) return NULL;

    for (i = 0; i < DUNGEON_MAX_INSTANCES; i++) {
        if (manager->instances[i].active &&
            manager->instances[i].instance_id == instance_id) {
            return &manager->instances[i];
        }
    }

    return NULL;
}

int dungeon_add_player(DungeonInstance* instance, u32 player_id)
{
    if (!instance) return ENGINE_ERROR_PARAM;
    if (instance->player_count >= DUNGEON_MAX_PLAYERS) {
        return ENGINE_ERROR_FULL;
    }

    instance->player_ids[instance->player_count] = player_id;
    instance->player_count++;
    instance->players_alive++;

    return ENGINE_OK;
}

void dungeon_remove_player(DungeonInstance* instance, u32 player_id)
{
    int i;

    if (!instance) return;

    for (i = 0; i < instance->player_count; i++) {
        if (instance->player_ids[i] == player_id) {
            /* Shift remaining players */
            while (i < instance->player_count - 1) {
                instance->player_ids[i] = instance->player_ids[i + 1];
                i++;
            }
            instance->player_count--;
            break;
        }
    }

    /* If no players left, mark as failed */
    if (instance->player_count == 0) {
        instance->state = DUNGEON_STATE_FAILED;
    }
}

int dungeon_start(DungeonInstance* instance, u32 current_time)
{
    if (!instance) return ENGINE_ERROR_PARAM;
    if (instance->player_count == 0) return ENGINE_ERROR;

    instance->state = DUNGEON_STATE_RUNNING;
    instance->start_time = current_time;
    instance->rooms[0].state = ROOM_STATE_ACTIVE;

    return ENGINE_OK;
}

void dungeon_update(DungeonInstance* instance, u32 current_time)
{
    u32 elapsed;

    if (!instance || !instance->active) return;

    /* Check time limit */
    if (instance->state == DUNGEON_STATE_RUNNING ||
        instance->state == DUNGEON_STATE_BOSS) {
        elapsed = current_time - instance->start_time;
        if (elapsed >= DUNGEON_TIME_LIMIT) {
            instance->state = DUNGEON_STATE_FAILED;
            return;
        }
    }

    /* Check for all players dead */
    if (instance->players_alive == 0 &&
        (instance->state == DUNGEON_STATE_RUNNING ||
         instance->state == DUNGEON_STATE_BOSS)) {
        instance->state = DUNGEON_STATE_FAILED;
    }
}

void dungeon_on_monster_kill(DungeonInstance* instance, u8 room_id,
                             u32 monster_id)
{
    DungeonRoom* room;

    (void)monster_id;

    if (!instance) return;
    if (room_id >= instance->room_count) return;

    room = &instance->rooms[room_id];

    if (room->monsters_alive > 0) {
        room->monsters_alive--;
        instance->total_kills++;

        /* Update combo */
        instance->combo_count++;
        if (instance->combo_count > instance->max_combo) {
            instance->max_combo = instance->combo_count;
        }
    }

    /* Check if room cleared */
    if (room->monsters_alive == 0) {
        room->state = ROOM_STATE_CLEARED;

        /* Unlock next room or finish dungeon */
        if (room_id < instance->room_count - 1) {
            instance->rooms[room_id + 1].state = ROOM_STATE_OPEN;
        } else {
            /* Last room cleared */
            if (room->is_boss_room) {
                instance->state = DUNGEON_STATE_CLEAR;
            }
        }
    }
}

void dungeon_on_player_death(DungeonInstance* instance, u32 player_id)
{
    int i;

    if (!instance) return;

    /* Verify player is in this dungeon */
    for (i = 0; i < instance->player_count; i++) {
        if (instance->player_ids[i] == player_id) {
            instance->players_alive--;
            instance->total_deaths++;

            /* Reset combo on death */
            instance->combo_count = 0;
            break;
        }
    }
}

int dungeon_next_room(DungeonInstance* instance)
{
    DungeonRoom* current;
    DungeonRoom* next;

    if (!instance) return ENGINE_ERROR_PARAM;

    if (instance->current_room >= instance->room_count - 1) {
        return ENGINE_ERROR;  /* Already at last room */
    }

    current = &instance->rooms[instance->current_room];
    if (current->state != ROOM_STATE_CLEARED) {
        return ENGINE_ERROR;  /* Current room not cleared */
    }

    instance->current_room++;
    next = &instance->rooms[instance->current_room];

    if (next->state != ROOM_STATE_OPEN) {
        return ENGINE_ERROR;  /* Next room not unlocked */
    }

    next->state = ROOM_STATE_ACTIVE;

    /* Check if entering boss room */
    if (next->is_boss_room) {
        instance->state = DUNGEON_STATE_BOSS;
    }

    return ENGINE_OK;
}

void dungeon_clear(DungeonInstance* instance, u32 current_time)
{
    if (!instance) return;

    instance->state = DUNGEON_STATE_CLEAR;
    instance->clear_time = current_time - instance->start_time;

    dungeon_calculate_grade(instance);
    dungeon_generate_rewards(instance);
}

void dungeon_calculate_grade(DungeonInstance* instance)
{
    u32 base_score = 0;
    u32 time_bonus = 0;
    u32 combo_bonus = 0;
    u32 death_penalty = 0;
    u32 clear_time_sec;

    if (!instance) return;

    /* Base score from kills */
    base_score = instance->total_kills * 100;

    /* Time bonus (faster = better) */
    clear_time_sec = instance->clear_time / 1000;
    if (clear_time_sec < 180) {        /* Under 3 minutes */
        time_bonus = 5000;
    } else if (clear_time_sec < 300) { /* Under 5 minutes */
        time_bonus = 3000;
    } else if (clear_time_sec < 600) { /* Under 10 minutes */
        time_bonus = 1000;
    }

    /* Combo bonus */
    combo_bonus = instance->max_combo * 50;

    /* Death penalty */
    death_penalty = instance->total_deaths * 500;

    /* Calculate final score */
    instance->score = base_score + time_bonus + combo_bonus;
    if (instance->score > death_penalty) {
        instance->score -= death_penalty;
    } else {
        instance->score = 0;
    }

    /* Difficulty multiplier */
    instance->score = instance->score * (100 + instance->difficulty * 25) / 100;

    /* Determine grade */
    if (instance->score >= 10000) {
        instance->grade = DUNGEON_GRADE_SSS;
    } else if (instance->score >= 8000) {
        instance->grade = DUNGEON_GRADE_SS;
    } else if (instance->score >= 6000) {
        instance->grade = DUNGEON_GRADE_S;
    } else if (instance->score >= 4500) {
        instance->grade = DUNGEON_GRADE_A;
    } else if (instance->score >= 3000) {
        instance->grade = DUNGEON_GRADE_B;
    } else if (instance->score >= 2000) {
        instance->grade = DUNGEON_GRADE_C;
    } else if (instance->score >= 1000) {
        instance->grade = DUNGEON_GRADE_D;
    } else {
        instance->grade = DUNGEON_GRADE_F;
    }
}

void dungeon_generate_rewards(DungeonInstance* instance)
{
    u32 base_exp = 1000;
    u32 base_gold = 500;
    int diff_mult;

    if (!instance) return;

    diff_mult = exp_multipliers[instance->difficulty];

    /* Calculate EXP reward */
    instance->reward.exp = base_exp * instance->room_count * diff_mult / 100;
    instance->reward.exp = instance->reward.exp * (100 + instance->grade * 10) / 100;

    /* Calculate gold reward */
    diff_mult = gold_multipliers[instance->difficulty];
    instance->reward.gold = base_gold * instance->room_count * diff_mult / 100;
    instance->reward.gold = instance->reward.gold * (100 + instance->grade * 10) / 100;

    /* Item rewards would be based on dungeon loot tables */
    instance->reward.item_count = 0;

    /* Add placeholder items based on grade */
    if (instance->grade >= DUNGEON_GRADE_B) {
        instance->reward.item_ids[instance->reward.item_count] = 1001;  /* Material */
        instance->reward.item_counts[instance->reward.item_count] = 1;
        instance->reward.item_count++;
    }
    if (instance->grade >= DUNGEON_GRADE_S) {
        instance->reward.item_ids[instance->reward.item_count] = 2001;  /* Rare material */
        instance->reward.item_counts[instance->reward.item_count] = 1;
        instance->reward.item_count++;
    }
    if (instance->grade >= DUNGEON_GRADE_SSS) {
        instance->reward.item_ids[instance->reward.item_count] = 3001;  /* Epic material */
        instance->reward.item_counts[instance->reward.item_count] = 1;
        instance->reward.item_count++;
    }
}

const char* dungeon_difficulty_name(DungeonDifficulty difficulty)
{
    if (difficulty >= 0 && difficulty < DUNGEON_DIFF_MAX) {
        return difficulty_names[difficulty];
    }
    return "UNKNOWN";
}

const char* dungeon_state_name(DungeonState state)
{
    if (state >= 0 && state < DUNGEON_STATE_MAX) {
        return state_names[state];
    }
    return "UNKNOWN";
}

const char* dungeon_grade_name(DungeonGrade grade)
{
    if (grade >= 0 && grade < DUNGEON_GRADE_MAX) {
        return grade_names[grade];
    }
    return "?";
}
