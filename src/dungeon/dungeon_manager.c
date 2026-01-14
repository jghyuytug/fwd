/**
 * @file dungeon_manager.c
 * @brief Dungeon Instance Manager - Full Implementation
 *
 * Phase: 6.8 - Dungeon System Implementation
 * Date: 2025-11-25
 *
 * Implements:
 * - Dungeon instance storage and lifecycle
 * - Monster spawning and tracking
 * - User entry/exit tracking
 * - Clearance record management
 * - Time limit and state tracking
 */

#include "dungeon_interface.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/* ========================================================================
 * CONFIGURATION CONSTANTS
 * ======================================================================== */

#define DEFAULT_MAX_INSTANCES       1000
#define DEFAULT_MAX_MONSTERS        50000
#define MAX_USERS_PER_DUNGEON       4
#define MAX_CLEARANCE_RECORDS       100000
#define INSTANCE_ID_BASE            10000

/* Default time limits per difficulty (seconds) */
static const int g_default_time_limits[] = {
    600,    /* NORMAL  - 10 minutes */
    540,    /* EXPERT  - 9 minutes */
    480,    /* MASTER  - 8 minutes */
    420,    /* KING    - 7 minutes */
    360     /* SLAYER  - 6 minutes */
};

/* Monster count multipliers per difficulty */
static const float g_monster_multipliers[] = {
    1.0f,   /* NORMAL */
    1.2f,   /* EXPERT */
    1.4f,   /* MASTER */
    1.6f,   /* KING */
    2.0f    /* SLAYER */
};

/* ========================================================================
 * INTERNAL DATA STRUCTURES
 * ======================================================================== */

/**
 * Extended dungeon instance with internal tracking
 */
typedef struct {
    DungeonInstance base;
    int in_use;
    unsigned int users_in_dungeon[MAX_USERS_PER_DUNGEON];
    int user_count;
    unsigned int elapsed_time_ms;
} DungeonInstanceInternal;

/**
 * Extended monster info with internal tracking
 */
typedef struct {
    MonsterInfo base;
    int in_use;
} MonsterInfoInternal;

/**
 * Clearance record entry
 */
typedef struct {
    DungeonClearance record;
    int in_use;
} ClearanceEntry;

/**
 * Dungeon module state
 */
typedef struct {
    int initialized;
    int max_instances;
    int max_monsters;

    /* Storage arrays */
    DungeonInstanceInternal* instances;
    MonsterInfoInternal* monsters;
    ClearanceEntry* clearance_records;
    int clearance_count;

    /* ID generators */
    unsigned int next_dungeon_id;
    unsigned int next_monster_id;

    /* Statistics */
    struct {
        unsigned int total_instances_created;
        unsigned int total_instances_destroyed;
        unsigned int total_monsters_spawned;
        unsigned int total_monsters_killed;
        unsigned int total_dungeons_completed;
        unsigned int total_dungeons_failed;
        unsigned int total_time_expired;
    } stats;
} DungeonModuleState;

static DungeonModuleState g_state = {0};

/* ========================================================================
 * INTERNAL HELPERS
 * ======================================================================== */

/**
 * Get current timestamp in seconds
 */
static unsigned int get_current_time(void) {
    return (unsigned int)time(NULL);
}

/**
 * Find free instance slot
 */
static int find_free_instance_slot(void) {
    for (int i = 0; i < g_state.max_instances; i++) {
        if (!g_state.instances[i].in_use) {
            return i;
        }
    }
    return -1;
}

/**
 * Find instance by ID
 */
static DungeonInstanceInternal* find_instance(unsigned int dungeon_id) {
    for (int i = 0; i < g_state.max_instances; i++) {
        if (g_state.instances[i].in_use &&
            g_state.instances[i].base.dungeon_id == dungeon_id) {
            return &g_state.instances[i];
        }
    }
    return NULL;
}

/**
 * Find free monster slot
 */
static int find_free_monster_slot(void) {
    for (int i = 0; i < g_state.max_monsters; i++) {
        if (!g_state.monsters[i].in_use) {
            return i;
        }
    }
    return -1;
}

/**
 * Find monster by ID
 */
static MonsterInfoInternal* find_monster(unsigned int monster_id) {
    for (int i = 0; i < g_state.max_monsters; i++) {
        if (g_state.monsters[i].in_use &&
            g_state.monsters[i].base.monster_id == monster_id) {
            return &g_state.monsters[i];
        }
    }
    return NULL;
}

/**
 * Find or create clearance record
 */
static ClearanceEntry* find_or_create_clearance(unsigned int user_id,
                                                  unsigned int template_id) {
    /* Search existing */
    for (int i = 0; i < g_state.clearance_count; i++) {
        if (g_state.clearance_records[i].in_use &&
            g_state.clearance_records[i].record.user_id == user_id &&
            g_state.clearance_records[i].record.dungeon_template_id == template_id) {
            return &g_state.clearance_records[i];
        }
    }

    /* Create new if space available */
    if (g_state.clearance_count < MAX_CLEARANCE_RECORDS) {
        ClearanceEntry* entry = &g_state.clearance_records[g_state.clearance_count];
        memset(entry, 0, sizeof(ClearanceEntry));
        entry->in_use = 1;
        entry->record.user_id = user_id;
        entry->record.dungeon_template_id = template_id;
        g_state.clearance_count++;
        return entry;
    }

    return NULL;
}

/**
 * Count alive monsters in dungeon
 */
static int count_alive_monsters(unsigned int dungeon_id) {
    int count = 0;
    for (int i = 0; i < g_state.max_monsters; i++) {
        if (g_state.monsters[i].in_use &&
            g_state.monsters[i].base.dungeon_id == dungeon_id &&
            g_state.monsters[i].base.is_alive) {
            count++;
        }
    }
    return count;
}

/**
 * Destroy all monsters in dungeon
 */
static void destroy_dungeon_monsters(unsigned int dungeon_id) {
    for (int i = 0; i < g_state.max_monsters; i++) {
        if (g_state.monsters[i].in_use &&
            g_state.monsters[i].base.dungeon_id == dungeon_id) {
            g_state.monsters[i].in_use = 0;
        }
    }
}

/* ========================================================================
 * PUBLIC API IMPLEMENTATION
 * ======================================================================== */

int Dungeon_Initialize(int max_instances, int max_monsters) {
    if (g_state.initialized) {
        fprintf(stderr, "[Dungeon] Module already initialized\n");
        return ERR_INVALID_STATE;
    }

    if (max_instances <= 0) {
        max_instances = DEFAULT_MAX_INSTANCES;
    }
    if (max_instances > 10000) {
        fprintf(stderr, "[Dungeon] Invalid max instances: %d (max 10000)\n", max_instances);
        return ERR_INVALID_PARAMETER;
    }

    if (max_monsters <= 0) {
        max_monsters = DEFAULT_MAX_MONSTERS;
    }
    if (max_monsters > 100000) {
        fprintf(stderr, "[Dungeon] Invalid max monsters: %d (max 100000)\n", max_monsters);
        return ERR_INVALID_PARAMETER;
    }

    printf("[Dungeon] Initializing dungeon manager...\n");

    /* Allocate instance storage */
    g_state.instances = (DungeonInstanceInternal*)calloc(
        max_instances, sizeof(DungeonInstanceInternal));
    if (!g_state.instances) {
        fprintf(stderr, "[Dungeon] Failed to allocate instance storage\n");
        return ERR_MEMORY;
    }

    /* Allocate monster storage */
    g_state.monsters = (MonsterInfoInternal*)calloc(
        max_monsters, sizeof(MonsterInfoInternal));
    if (!g_state.monsters) {
        free(g_state.instances);
        g_state.instances = NULL;
        fprintf(stderr, "[Dungeon] Failed to allocate monster storage\n");
        return ERR_MEMORY;
    }

    /* Allocate clearance records */
    g_state.clearance_records = (ClearanceEntry*)calloc(
        MAX_CLEARANCE_RECORDS, sizeof(ClearanceEntry));
    if (!g_state.clearance_records) {
        free(g_state.instances);
        free(g_state.monsters);
        g_state.instances = NULL;
        g_state.monsters = NULL;
        fprintf(stderr, "[Dungeon] Failed to allocate clearance storage\n");
        return ERR_MEMORY;
    }

    g_state.max_instances = max_instances;
    g_state.max_monsters = max_monsters;
    g_state.next_dungeon_id = INSTANCE_ID_BASE;
    g_state.next_monster_id = 1;
    g_state.clearance_count = 0;
    memset(&g_state.stats, 0, sizeof(g_state.stats));
    g_state.initialized = 1;

    printf("[Dungeon] Manager initialized. Max instances: %d, Max monsters: %d\n",
           max_instances, max_monsters);

    return ERR_SUCCESS;
}

void Dungeon_Cleanup(void) {
    if (!g_state.initialized) {
        return;
    }

    printf("[Dungeon] Cleaning up dungeon manager...\n");
    printf("[Dungeon] Final Statistics:\n");
    printf("  - Instances Created:   %u\n", g_state.stats.total_instances_created);
    printf("  - Instances Destroyed: %u\n", g_state.stats.total_instances_destroyed);
    printf("  - Monsters Spawned:    %u\n", g_state.stats.total_monsters_spawned);
    printf("  - Monsters Killed:     %u\n", g_state.stats.total_monsters_killed);
    printf("  - Dungeons Completed:  %u\n", g_state.stats.total_dungeons_completed);
    printf("  - Dungeons Failed:     %u\n", g_state.stats.total_dungeons_failed);
    printf("  - Time Expired:        %u\n", g_state.stats.total_time_expired);

    /* Free storage */
    if (g_state.instances) {
        free(g_state.instances);
        g_state.instances = NULL;
    }
    if (g_state.monsters) {
        free(g_state.monsters);
        g_state.monsters = NULL;
    }
    if (g_state.clearance_records) {
        free(g_state.clearance_records);
        g_state.clearance_records = NULL;
    }

    memset(&g_state, 0, sizeof(g_state));
    printf("[Dungeon] Manager cleanup complete\n");
}

int Dungeon_CreateInstance(unsigned int template_id,
                           unsigned int owner_id,
                           unsigned int party_id,
                           DungeonDifficulty difficulty) {
    if (!g_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (difficulty > DIFFICULTY_SLAYER) {
        return ERR_INVALID_PARAMETER;
    }

    int slot = find_free_instance_slot();
    if (slot < 0) {
        fprintf(stderr, "[Dungeon] No free instance slots available\n");
        return ERR_LIMIT_EXCEEDED;
    }

    DungeonInstanceInternal* inst = &g_state.instances[slot];
    memset(inst, 0, sizeof(DungeonInstanceInternal));

    unsigned int dungeon_id = g_state.next_dungeon_id++;

    inst->base.dungeon_id = dungeon_id;
    inst->base.dungeon_template_id = template_id;
    inst->base.difficulty = difficulty;
    inst->base.state = DUNGEON_STATE_IDLE;
    inst->base.owner_user_id = owner_id;
    inst->base.party_id = party_id;
    inst->base.start_time = 0;  /* Set when first user enters */
    inst->base.clear_time = 0;
    inst->base.time_limit_seconds = g_default_time_limits[difficulty];
    inst->base.monsters_killed = 0;
    inst->base.total_monsters = 0;
    inst->base.is_solo = (party_id == 0) ? 1 : 0;
    inst->in_use = 1;
    inst->user_count = 0;
    inst->elapsed_time_ms = 0;

    g_state.stats.total_instances_created++;

    printf("[Dungeon] Created instance %u (template=%u, owner=%u, difficulty=%d)\n",
           dungeon_id, template_id, owner_id, difficulty);

    return (int)dungeon_id;
}

int Dungeon_DestroyInstance(unsigned int dungeon_id) {
    if (!g_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    DungeonInstanceInternal* inst = find_instance(dungeon_id);
    if (!inst) {
        return ERR_NOT_FOUND;
    }

    /* Destroy all monsters in this dungeon */
    destroy_dungeon_monsters(dungeon_id);

    inst->in_use = 0;
    g_state.stats.total_instances_destroyed++;

    printf("[Dungeon] Destroyed instance %u\n", dungeon_id);

    return ERR_SUCCESS;
}

int Dungeon_Enter(unsigned int dungeon_id, unsigned int user_id) {
    if (!g_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    DungeonInstanceInternal* inst = find_instance(dungeon_id);
    if (!inst) {
        return ERR_NOT_FOUND;
    }

    /* Check if dungeon is still active */
    if (inst->base.state == DUNGEON_STATE_CLEARED ||
        inst->base.state == DUNGEON_STATE_FAILED) {
        return ERR_INVALID_STATE;
    }

    /* Check if already in dungeon */
    for (int i = 0; i < inst->user_count; i++) {
        if (inst->users_in_dungeon[i] == user_id) {
            return ERR_DUPLICATE;
        }
    }

    /* Check capacity */
    if (inst->user_count >= MAX_USERS_PER_DUNGEON) {
        return ERR_LIMIT_EXCEEDED;
    }

    /* Add user */
    inst->users_in_dungeon[inst->user_count++] = user_id;

    /* Start timer on first entry */
    if (inst->base.state == DUNGEON_STATE_IDLE && inst->user_count == 1) {
        inst->base.state = DUNGEON_STATE_ACTIVE;
        inst->base.start_time = get_current_time();
        printf("[Dungeon] Instance %u activated (first user %u entered)\n",
               dungeon_id, user_id);
    }

    printf("[Dungeon] User %u entered instance %u (%d users now)\n",
           user_id, dungeon_id, inst->user_count);

    return ERR_SUCCESS;
}

int Dungeon_Exit(unsigned int dungeon_id, unsigned int user_id) {
    if (!g_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    DungeonInstanceInternal* inst = find_instance(dungeon_id);
    if (!inst) {
        return ERR_NOT_FOUND;
    }

    /* Find and remove user */
    int found = 0;
    for (int i = 0; i < inst->user_count; i++) {
        if (inst->users_in_dungeon[i] == user_id) {
            /* Shift remaining users */
            for (int j = i; j < inst->user_count - 1; j++) {
                inst->users_in_dungeon[j] = inst->users_in_dungeon[j + 1];
            }
            inst->user_count--;
            found = 1;
            break;
        }
    }

    if (!found) {
        return ERR_NOT_FOUND;
    }

    printf("[Dungeon] User %u exited instance %u (%d users remaining)\n",
           user_id, dungeon_id, inst->user_count);

    /* Auto-fail if all users left during active dungeon */
    if (inst->user_count == 0 &&
        (inst->base.state == DUNGEON_STATE_ACTIVE ||
         inst->base.state == DUNGEON_STATE_BOSS_FIGHT)) {
        Dungeon_Fail(dungeon_id);
    }

    return ERR_SUCCESS;
}

int Dungeon_SpawnMonster(unsigned int dungeon_id,
                         unsigned int monster_template_id,
                         MonsterType monster_type,
                         int x, int y, int z) {
    if (!g_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    DungeonInstanceInternal* inst = find_instance(dungeon_id);
    if (!inst) {
        return ERR_NOT_FOUND;
    }

    int slot = find_free_monster_slot();
    if (slot < 0) {
        fprintf(stderr, "[Dungeon] No free monster slots available\n");
        return ERR_LIMIT_EXCEEDED;
    }

    MonsterInfoInternal* mon = &g_state.monsters[slot];
    memset(mon, 0, sizeof(MonsterInfoInternal));

    unsigned int monster_id = g_state.next_monster_id++;

    mon->base.monster_id = monster_id;
    mon->base.monster_template_id = monster_template_id;
    mon->base.monster_type = monster_type;
    mon->base.dungeon_id = dungeon_id;
    mon->base.spawn_x = x;
    mon->base.spawn_y = y;
    mon->base.spawn_z = z;
    mon->base.is_alive = 1;

    /* Set health based on monster type and difficulty */
    int base_health = 1000;
    switch (monster_type) {
        case MONSTER_TYPE_NORMAL: base_health = 1000; break;
        case MONSTER_TYPE_ELITE:  base_health = 5000; break;
        case MONSTER_TYPE_BOSS:   base_health = 50000; break;
        case MONSTER_TYPE_NAMED:  base_health = 10000; break;
    }
    float multiplier = g_monster_multipliers[inst->base.difficulty];
    mon->base.max_health = (int)(base_health * multiplier);
    mon->base.health = mon->base.max_health;
    mon->base.level = 1 + (inst->base.difficulty * 10);

    mon->in_use = 1;
    inst->base.total_monsters++;
    g_state.stats.total_monsters_spawned++;

    return (int)monster_id;
}

int Dungeon_KillMonster(unsigned int monster_id, unsigned int killer_user_id) {
    if (!g_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    MonsterInfoInternal* mon = find_monster(monster_id);
    if (!mon) {
        return ERR_NOT_FOUND;
    }

    if (!mon->base.is_alive) {
        return ERR_INVALID_STATE;
    }

    mon->base.is_alive = 0;
    mon->base.health = 0;

    /* Update dungeon stats */
    DungeonInstanceInternal* inst = find_instance(mon->base.dungeon_id);
    if (inst) {
        inst->base.monsters_killed++;

        printf("[Dungeon] Monster %u killed by user %u in instance %u (%d/%d)\n",
               monster_id, killer_user_id, mon->base.dungeon_id,
               inst->base.monsters_killed, inst->base.total_monsters);

        /* Check if boss was killed */
        if (mon->base.monster_type == MONSTER_TYPE_BOSS) {
            /* Check if all monsters are dead */
            int alive = count_alive_monsters(mon->base.dungeon_id);
            if (alive == 0) {
                printf("[Dungeon] All monsters killed, dungeon %u completing\n",
                       mon->base.dungeon_id);
                Dungeon_Complete(mon->base.dungeon_id);
            }
        }
    }

    g_state.stats.total_monsters_killed++;

    return ERR_SUCCESS;
}

int Dungeon_StartBossFight(unsigned int dungeon_id) {
    if (!g_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    DungeonInstanceInternal* inst = find_instance(dungeon_id);
    if (!inst) {
        return ERR_NOT_FOUND;
    }

    if (inst->base.state != DUNGEON_STATE_ACTIVE) {
        return ERR_INVALID_STATE;
    }

    inst->base.state = DUNGEON_STATE_BOSS_FIGHT;
    printf("[Dungeon] Boss fight started in instance %u\n", dungeon_id);

    return ERR_SUCCESS;
}

int Dungeon_Complete(unsigned int dungeon_id) {
    if (!g_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    DungeonInstanceInternal* inst = find_instance(dungeon_id);
    if (!inst) {
        return ERR_NOT_FOUND;
    }

    if (inst->base.state == DUNGEON_STATE_CLEARED ||
        inst->base.state == DUNGEON_STATE_FAILED) {
        return ERR_INVALID_STATE;
    }

    inst->base.state = DUNGEON_STATE_CLEARED;
    inst->base.clear_time = get_current_time();

    unsigned int clear_duration = inst->base.clear_time - inst->base.start_time;

    printf("[Dungeon] Instance %u CLEARED! Duration: %u seconds\n",
           dungeon_id, clear_duration);

    /* Update clearance records for all users in dungeon */
    for (int i = 0; i < inst->user_count; i++) {
        unsigned int user_id = inst->users_in_dungeon[i];
        ClearanceEntry* entry = find_or_create_clearance(
            user_id, inst->base.dungeon_template_id);

        if (entry) {
            entry->record.clear_count++;
            entry->record.last_clear_time = inst->base.clear_time;
            entry->record.difficulty = inst->base.difficulty;

            /* Update fastest time */
            if (entry->record.fastest_time == 0 ||
                clear_duration < entry->record.fastest_time) {
                entry->record.fastest_time = clear_duration;
            }

            printf("[Dungeon] Updated clearance for user %u: %u clears, best=%us\n",
                   user_id, entry->record.clear_count, entry->record.fastest_time);
        }
    }

    g_state.stats.total_dungeons_completed++;

    return ERR_SUCCESS;
}

int Dungeon_Fail(unsigned int dungeon_id) {
    if (!g_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    DungeonInstanceInternal* inst = find_instance(dungeon_id);
    if (!inst) {
        return ERR_NOT_FOUND;
    }

    if (inst->base.state == DUNGEON_STATE_CLEARED ||
        inst->base.state == DUNGEON_STATE_FAILED) {
        return ERR_INVALID_STATE;
    }

    inst->base.state = DUNGEON_STATE_FAILED;
    printf("[Dungeon] Instance %u FAILED\n", dungeon_id);

    g_state.stats.total_dungeons_failed++;

    return ERR_SUCCESS;
}

int Dungeon_GetInstanceInfo(unsigned int dungeon_id, DungeonInstance* instance) {
    if (!g_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!instance) {
        return ERR_INVALID_PARAMETER;
    }

    DungeonInstanceInternal* inst = find_instance(dungeon_id);
    if (!inst) {
        return ERR_NOT_FOUND;
    }

    memcpy(instance, &inst->base, sizeof(DungeonInstance));

    return ERR_SUCCESS;
}

int Dungeon_GetClearanceRecord(unsigned int user_id,
                               unsigned int template_id,
                               DungeonClearance* clearance) {
    if (!g_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!clearance) {
        return ERR_INVALID_PARAMETER;
    }

    /* Search for existing record */
    for (int i = 0; i < g_state.clearance_count; i++) {
        if (g_state.clearance_records[i].in_use &&
            g_state.clearance_records[i].record.user_id == user_id &&
            g_state.clearance_records[i].record.dungeon_template_id == template_id) {
            memcpy(clearance, &g_state.clearance_records[i].record,
                   sizeof(DungeonClearance));
            return ERR_SUCCESS;
        }
    }

    /* No record found - return empty */
    memset(clearance, 0, sizeof(DungeonClearance));
    clearance->user_id = user_id;
    clearance->dungeon_template_id = template_id;

    return ERR_SUCCESS;
}

int Dungeon_Update(int delta_time_ms) {
    if (!g_state.initialized) {
        return 0;
    }

    int dungeons_processed = 0;
    unsigned int current_time = get_current_time();

    for (int i = 0; i < g_state.max_instances; i++) {
        DungeonInstanceInternal* inst = &g_state.instances[i];

        if (!inst->in_use) {
            continue;
        }

        /* Only process active dungeons */
        if (inst->base.state != DUNGEON_STATE_ACTIVE &&
            inst->base.state != DUNGEON_STATE_BOSS_FIGHT) {
            continue;
        }

        inst->elapsed_time_ms += delta_time_ms;
        dungeons_processed++;

        /* Check time limit */
        if (inst->base.start_time > 0) {
            unsigned int elapsed_seconds = current_time - inst->base.start_time;
            if ((int)elapsed_seconds >= inst->base.time_limit_seconds) {
                printf("[Dungeon] Instance %u time expired (%ds limit)\n",
                       inst->base.dungeon_id, inst->base.time_limit_seconds);
                Dungeon_Fail(inst->base.dungeon_id);
                g_state.stats.total_time_expired++;
            }
        }
    }

    return dungeons_processed;
}

int Dungeon_GetActiveCount(void) {
    if (!g_state.initialized) {
        return 0;
    }

    int count = 0;
    for (int i = 0; i < g_state.max_instances; i++) {
        if (g_state.instances[i].in_use &&
            (g_state.instances[i].base.state == DUNGEON_STATE_ACTIVE ||
             g_state.instances[i].base.state == DUNGEON_STATE_BOSS_FIGHT)) {
            count++;
        }
    }

    return count;
}

int Dungeon_GetMonsters(unsigned int dungeon_id,
                        MonsterInfo* monsters,
                        int max_monsters) {
    if (!g_state.initialized) {
        return 0;
    }

    if (!monsters || max_monsters <= 0) {
        return 0;
    }

    int count = 0;
    for (int i = 0; i < g_state.max_monsters && count < max_monsters; i++) {
        if (g_state.monsters[i].in_use &&
            g_state.monsters[i].base.dungeon_id == dungeon_id) {
            memcpy(&monsters[count], &g_state.monsters[i].base, sizeof(MonsterInfo));
            count++;
        }
    }

    return count;
}

int Dungeon_GetRemainingTime(unsigned int dungeon_id) {
    if (!g_state.initialized) {
        return -1;
    }

    DungeonInstanceInternal* inst = find_instance(dungeon_id);
    if (!inst) {
        return -1;
    }

    /* If not started, return full time */
    if (inst->base.start_time == 0) {
        return inst->base.time_limit_seconds;
    }

    unsigned int current_time = get_current_time();
    unsigned int elapsed = current_time - inst->base.start_time;

    int remaining = inst->base.time_limit_seconds - (int)elapsed;

    return (remaining > 0) ? remaining : 0;
}

/* ========================================================================
 * DEBUG/STATISTICS API
 * ======================================================================== */

/**
 * Print dungeon module statistics
 */
void Dungeon_PrintStats(void) {
    if (!g_state.initialized) {
        printf("[Dungeon] Module not initialized\n");
        return;
    }

    printf("\n=== Dungeon Module Statistics ===\n");
    printf("Active Instances:    %d\n", Dungeon_GetActiveCount());
    printf("Max Instances:       %d\n", g_state.max_instances);
    printf("Max Monsters:        %d\n", g_state.max_monsters);
    printf("Clearance Records:   %d\n", g_state.clearance_count);
    printf("\n--- Lifetime Stats ---\n");
    printf("Instances Created:   %u\n", g_state.stats.total_instances_created);
    printf("Instances Destroyed: %u\n", g_state.stats.total_instances_destroyed);
    printf("Monsters Spawned:    %u\n", g_state.stats.total_monsters_spawned);
    printf("Monsters Killed:     %u\n", g_state.stats.total_monsters_killed);
    printf("Dungeons Completed:  %u\n", g_state.stats.total_dungeons_completed);
    printf("Dungeons Failed:     %u\n", g_state.stats.total_dungeons_failed);
    printf("Time Expired:        %u\n", g_state.stats.total_time_expired);
    printf("==================================\n\n");
}
