/**
 * Quick Party Matching System - Implementation
 * Purpose: Automatic party matching for dungeons
 *
 * Phase: 6.3 - Party System Implementation
 * Created: 2025-11-23
 */

#include "../../include/party/quick_party.h"
#include "../../include/party/party_manager.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/* Database Integration */
#include "../../include/database/headers/db_connection_manager.h"
#include "../../src/orm/core_game/taiwan_cain/headers/charac_info_orm.h"

/* Database manager for character lookups */
static DBConnectionManager* g_quick_party_db_manager = NULL;

/**
 * Set database manager for quick party system
 */
void QuickParty_SetDatabaseManager(DBConnectionManager* db_manager) {
    g_quick_party_db_manager = db_manager;
}

/**
 * Determine job role from job class
 * DPS: Fighter, Gunner, Mage, Thief
 * TANK: Knight, Grappler
 * HEALER: Priest
 * SUPPORT: Enchantress, Launcher
 */
static JobRole DetermineJobRole(int job_class) {
    switch (job_class) {
        case 0:  /* Slayer */
        case 1:  /* Fighter */
        case 3:  /* Gunner */
        case 4:  /* Mage */
        case 5:  /* Priest - Female */
        case 13: /* Thief */
            return JOB_ROLE_DPS;
        case 2:  /* Female Gunner */
        case 7:  /* Launcher variants */
            return JOB_ROLE_SUPPORT;
        case 6:  /* Knight */
        case 8:  /* Grappler */
            return JOB_ROLE_TANK;
        case 9:  /* Priest - Male */
        case 14: /* Healer */
            return JOB_ROLE_HEALER;
        default:
            return JOB_ROLE_DPS;
    }
}

/* Maximum queue size */
#define MAX_QUEUE_SIZE 5000

/* Matching parameters */
#define MAX_LEVEL_DIFFERENCE 5      // ±5 levels for matching
#define QUEUE_TIMEOUT_SECONDS 180   // 3 minutes before relaxing requirements
#define PARTY_SIZE_TARGET 4         // Target party size

/* Global state */
static struct {
    int initialized;

    /* Match queue */
    MatchRequest* queue;
    int queue_size;
    int max_queue_size;

    /* Statistics */
    struct {
        unsigned int total_queued;
        unsigned int total_matched;
        unsigned int total_timeouts;
        unsigned int average_wait_time;
    } stats;
} g_quick_party_state = {0};

/**
 * Initialize quick party system
 */
int QuickParty_Initialize(void)
{
    if (g_quick_party_state.initialized) {
        printf("[QuickParty] Already initialized\n");
        return ERR_INVALID_STATE;
    }

    printf("[QuickParty] Initializing quick party system...\n");

    /* Allocate queue */
    g_quick_party_state.max_queue_size = MAX_QUEUE_SIZE;
    g_quick_party_state.queue = (MatchRequest*)malloc(sizeof(MatchRequest) * MAX_QUEUE_SIZE);

    if (!g_quick_party_state.queue) {
        printf("[QuickParty] ERROR: Failed to allocate queue\n");
        return ERR_MEMORY_ALLOCATION_FAILED;
    }

    memset(g_quick_party_state.queue, 0, sizeof(MatchRequest) * MAX_QUEUE_SIZE);
    g_quick_party_state.queue_size = 0;
    g_quick_party_state.initialized = 1;

    printf("[QuickParty] Quick party system initialized (max queue: %d)\n", MAX_QUEUE_SIZE);

    return ERR_SUCCESS;
}

/**
 * Find character in queue
 */
static int FindInQueue(int character_id)
{
    for (int i = 0; i < g_quick_party_state.queue_size; i++) {
        if (g_quick_party_state.queue[i].active &&
            g_quick_party_state.queue[i].character_id == character_id) {
            return i;
        }
    }
    return -1;
}

/**
 * Enter matchmaking queue
 */
int QuickParty_EnterQueue(int character_id, DungeonType dungeon_type, int dungeon_id)
{
    if (!g_quick_party_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Check if already in queue */
    if (FindInQueue(character_id) >= 0) {
        printf("[QuickParty] Character %d is already in queue\n", character_id);
        return ERR_PARTY_ALREADY_IN_QUEUE;
    }

    /* Check if already in a party */
    if (PartyManager_GetCharacterParty(character_id) != 0) {
        printf("[QuickParty] Character %d is already in a party\n", character_id);
        return ERR_PARTY_ALREADY_IN_PARTY;
    }

    /* Check queue capacity */
    if (g_quick_party_state.queue_size >= g_quick_party_state.max_queue_size) {
        printf("[QuickParty] ERROR: Queue is full\n");
        return ERR_PARTY_QUEUE_FULL;
    }

    /* Add to queue */
    MatchRequest* request = &g_quick_party_state.queue[g_quick_party_state.queue_size];
    request->character_id = character_id;

    /* Get character data from database */
    request->level = 60;      /* Default level */
    request->job_class = 0;   /* Default job class */
    request->job_role = JOB_ROLE_DPS;  /* Default role */

    if (g_quick_party_db_manager) {
        CharacInfo charac_info;
        if (CharacInfo_GetByCharacNo(g_quick_party_db_manager, character_id, &charac_info) == 0) {
            request->level = charac_info.lev;
            request->job_class = charac_info.job;
            request->job_role = DetermineJobRole(charac_info.job);
        }
    }

    request->dungeon_type = dungeon_type;
    request->dungeon_id = dungeon_id;
    request->queue_time = (unsigned int)time(NULL);
    request->active = 1;

    g_quick_party_state.queue_size++;
    g_quick_party_state.stats.total_queued++;

    printf("[QuickParty] Character %d entered queue (position: %d, dungeon type: %d)\n",
           character_id, g_quick_party_state.queue_size, dungeon_type);

    return ERR_SUCCESS;
}

/**
 * Leave matchmaking queue
 */
int QuickParty_LeaveQueue(int character_id)
{
    if (!g_quick_party_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Find character in queue */
    int index = FindInQueue(character_id);
    if (index < 0) {
        return ERR_PARTY_NOT_IN_QUEUE;
    }

    /* Remove from queue (shift remaining entries) */
    for (int i = index; i < g_quick_party_state.queue_size - 1; i++) {
        g_quick_party_state.queue[i] = g_quick_party_state.queue[i + 1];
    }

    g_quick_party_state.queue_size--;

    printf("[QuickParty] Character %d left queue\n", character_id);

    return ERR_SUCCESS;
}

/**
 * Check if two requests can be matched
 */
static int CanMatch(const MatchRequest* req1, const MatchRequest* req2, unsigned int current_time)
{
    /* Check dungeon type */
    if (req1->dungeon_type != req2->dungeon_type) {
        return 0;
    }

    /* Check specific dungeon ID (if specified) */
    if (req1->dungeon_id != 0 && req2->dungeon_id != 0 && req1->dungeon_id != req2->dungeon_id) {
        return 0;
    }

    /* Check level difference */
    int level_diff = abs(req1->level - req2->level);
    unsigned int wait_time = current_time - req1->queue_time;

    /* Relax level requirements after timeout */
    int max_level_diff = (wait_time > QUEUE_TIMEOUT_SECONDS) ? (MAX_LEVEL_DIFFERENCE * 2) : MAX_LEVEL_DIFFERENCE;

    if (level_diff > max_level_diff) {
        return 0;
    }

    return 1;
}

/**
 * Process matchmaking
 */
int QuickParty_ProcessMatching(MatchResult* out_matches, int max_matches)
{
    if (!g_quick_party_state.initialized) {
        return 0;
    }

    if (!out_matches || max_matches <= 0) {
        return 0;
    }

    unsigned int current_time = (unsigned int)time(NULL);
    int match_count = 0;

    /* Simple matching algorithm: group first N compatible players */
    int processed[MAX_QUEUE_SIZE] = {0};

    for (int i = 0; i < g_quick_party_state.queue_size && match_count < max_matches; i++) {
        if (processed[i] || !g_quick_party_state.queue[i].active) {
            continue;
        }

        /* Start a new match group */
        MatchResult* match = &out_matches[match_count];
        match->member_count = 0;
        match->member_ids[match->member_count++] = g_quick_party_state.queue[i].character_id;
        processed[i] = 1;

        /* Find compatible teammates */
        for (int j = i + 1; j < g_quick_party_state.queue_size && match->member_count < PARTY_SIZE_TARGET; j++) {
            if (processed[j] || !g_quick_party_state.queue[j].active) {
                continue;
            }

            if (CanMatch(&g_quick_party_state.queue[i], &g_quick_party_state.queue[j], current_time)) {
                match->member_ids[match->member_count++] = g_quick_party_state.queue[j].character_id;
                processed[j] = 1;
            }
        }

        /* Only create party if we have at least 2 members */
        if (match->member_count >= 2) {
            /* Create party */
            int party_id = PartyManager_Create(match->member_ids[0], PARTY_TYPE_QUICK, NULL, NULL);

            if (party_id > 0) {
                match->party_id = party_id;

                /* Add other members to party */
                for (int k = 1; k < match->member_count; k++) {
                    /* Simulate invitation and acceptance */
                    PartyManager_Invite(party_id, match->member_ids[0], match->member_ids[k]);
                    PartyManager_AcceptInvite(party_id, match->member_ids[k]);
                }

                match_count++;
                g_quick_party_state.stats.total_matched += match->member_count;

                printf("[QuickParty] Match found! Party %d created with %d members\n",
                       party_id, match->member_count);
            }
        } else {
            /* Not enough members, undo processing */
            for (int k = 0; k < match->member_count; k++) {
                for (int m = 0; m < g_quick_party_state.queue_size; m++) {
                    if (g_quick_party_state.queue[m].character_id == match->member_ids[k]) {
                        processed[m] = 0;
                        break;
                    }
                }
            }
        }
    }

    /* Remove matched players from queue */
    int new_size = 0;
    for (int i = 0; i < g_quick_party_state.queue_size; i++) {
        if (!processed[i] && g_quick_party_state.queue[i].active) {
            if (new_size != i) {
                g_quick_party_state.queue[new_size] = g_quick_party_state.queue[i];
            }
            new_size++;
        }
    }
    g_quick_party_state.queue_size = new_size;

    return match_count;
}

/**
 * Get current queue position
 */
int QuickParty_GetQueuePosition(int character_id)
{
    if (!g_quick_party_state.initialized) {
        return 0;
    }

    int index = FindInQueue(character_id);
    return (index >= 0) ? (index + 1) : 0;
}

/**
 * Get estimated wait time
 */
int QuickParty_GetEstimatedWaitTime(int character_id)
{
    if (!g_quick_party_state.initialized) {
        return -1;
    }

    int index = FindInQueue(character_id);
    if (index < 0) {
        return -1;
    }

    /* Simple estimation: position * 10 seconds */
    return (index + 1) * 10;
}

/**
 * Get current queue size
 */
int QuickParty_GetQueueSize(void)
{
    if (!g_quick_party_state.initialized) {
        return 0;
    }

    return g_quick_party_state.queue_size;
}

/**
 * Cleanup quick party system
 */
void QuickParty_Cleanup(void)
{
    if (!g_quick_party_state.initialized) {
        return;
    }

    printf("[QuickParty] Cleaning up quick party system...\n");
    printf("[QuickParty] Statistics:\n");
    printf("  - Total Queued: %u\n", g_quick_party_state.stats.total_queued);
    printf("  - Total Matched: %u\n", g_quick_party_state.stats.total_matched);
    printf("  - Total Timeouts: %u\n", g_quick_party_state.stats.total_timeouts);

    if (g_quick_party_state.queue) {
        free(g_quick_party_state.queue);
        g_quick_party_state.queue = NULL;
    }

    g_quick_party_state.queue_size = 0;
    g_quick_party_state.max_queue_size = 0;
    g_quick_party_state.initialized = 0;

    printf("[QuickParty] Cleanup complete\n");
}
