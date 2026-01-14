/**
 * Quest Trigger System - Implementation
 * Purpose: Listen to game events and update quest progress automatically
 *
 * Phase: 6.2 - Quest System Implementation
 * Created: 2025-11-23
 */

#include "../../include/quest/quest_trigger.h"
#include "../../include/quest/quest_manager.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_TRIGGERS 100

/* Global state */
static struct {
    int initialized;
    QuestTrigger triggers[MAX_TRIGGERS];
    int trigger_count;
    int next_trigger_id;

    /* Statistics */
    struct {
        unsigned int triggers_registered;
        unsigned int triggers_unregistered;
        unsigned int monster_kills;
        unsigned int items_obtained;
        unsigned int npc_talks;
        unsigned int dungeons_completed;
        unsigned int levels_reached;
        unsigned int items_used;
    } stats;
} g_trigger_state = {0};

/**
 * Initialize quest trigger system
 */
int QuestTrigger_Initialize(void)
{
    if (g_trigger_state.initialized) {
        printf("[QuestTrigger] Already initialized\n");
        return ERR_INVALID_STATE;
    }

    printf("[QuestTrigger] Initializing quest trigger system...\n");

    memset(&g_trigger_state, 0, sizeof(g_trigger_state));
    g_trigger_state.next_trigger_id = 1;
    g_trigger_state.initialized = 1;

    printf("[QuestTrigger] Trigger system initialized (max %d triggers)\n", MAX_TRIGGERS);

    return ERR_SUCCESS;
}

/**
 * Register a quest trigger
 */
int QuestTrigger_Register(ObjectiveType objective_type, int target_id, QuestTriggerCallback callback)
{
    if (!g_trigger_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!callback) {
        return ERR_INVALID_PARAMETER;
    }

    if (g_trigger_state.trigger_count >= MAX_TRIGGERS) {
        printf("[QuestTrigger] ERROR: Trigger storage full\n");
        return ERR_QUEST_TRIGGER_LIMIT;
    }

    /* Create new trigger */
    QuestTrigger* trigger = &g_trigger_state.triggers[g_trigger_state.trigger_count];
    trigger->trigger_id = g_trigger_state.next_trigger_id++;
    trigger->objective_type = objective_type;
    trigger->target_id = target_id;
    trigger->callback = callback;
    trigger->active = 1;

    g_trigger_state.trigger_count++;
    g_trigger_state.stats.triggers_registered++;

    printf("[QuestTrigger] Registered trigger %d (type=%d, target=%d)\n",
           trigger->trigger_id, objective_type, target_id);

    return trigger->trigger_id;
}

/**
 * Unregister a quest trigger
 */
int QuestTrigger_Unregister(int trigger_id)
{
    if (!g_trigger_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Find and deactivate trigger */
    for (int i = 0; i < g_trigger_state.trigger_count; i++) {
        if (g_trigger_state.triggers[i].trigger_id == trigger_id) {
            g_trigger_state.triggers[i].active = 0;
            g_trigger_state.stats.triggers_unregistered++;

            printf("[QuestTrigger] Unregistered trigger %d\n", trigger_id);
            return ERR_SUCCESS;
        }
    }

    return ERR_QUEST_TRIGGER_NOT_FOUND;
}

/**
 * Helper: Fire matching triggers
 */
static void FireTriggers(ObjectiveType objective_type, int character_id, int target_id, int count)
{
    for (int i = 0; i < g_trigger_state.trigger_count; i++) {
        QuestTrigger* trigger = &g_trigger_state.triggers[i];

        if (!trigger->active) {
            continue;
        }

        /* Check if trigger matches */
        if (trigger->objective_type == objective_type &&
            (trigger->target_id == 0 || trigger->target_id == target_id)) {

            /* Call callback */
            trigger->callback(character_id, target_id, count);
        }
    }

    /* Also update quest manager directly */
    QuestManager_UpdateProgress(character_id, objective_type, target_id, count);
}

/**
 * Trigger event: Monster killed
 */
void QuestTrigger_OnMonsterKilled(int character_id, int monster_id)
{
    if (!g_trigger_state.initialized) {
        return;
    }

    g_trigger_state.stats.monster_kills++;

    printf("[QuestTrigger] Monster killed: character=%d, monster=%d\n",
           character_id, monster_id);

    FireTriggers(OBJECTIVE_KILL_MONSTER, character_id, monster_id, 1);
}

/**
 * Trigger event: Item obtained
 */
void QuestTrigger_OnItemObtained(int character_id, int item_id, int count)
{
    if (!g_trigger_state.initialized) {
        return;
    }

    g_trigger_state.stats.items_obtained++;

    printf("[QuestTrigger] Item obtained: character=%d, item=%d, count=%d\n",
           character_id, item_id, count);

    FireTriggers(OBJECTIVE_COLLECT_ITEM, character_id, item_id, count);
}

/**
 * Trigger event: NPC talk
 */
void QuestTrigger_OnNPCTalk(int character_id, int npc_id)
{
    if (!g_trigger_state.initialized) {
        return;
    }

    g_trigger_state.stats.npc_talks++;

    printf("[QuestTrigger] NPC talk: character=%d, npc=%d\n",
           character_id, npc_id);

    FireTriggers(OBJECTIVE_TALK_TO_NPC, character_id, npc_id, 1);
}

/**
 * Trigger event: Dungeon completed
 */
void QuestTrigger_OnDungeonComplete(int character_id, int dungeon_id)
{
    if (!g_trigger_state.initialized) {
        return;
    }

    g_trigger_state.stats.dungeons_completed++;

    printf("[QuestTrigger] Dungeon completed: character=%d, dungeon=%d\n",
           character_id, dungeon_id);

    FireTriggers(OBJECTIVE_COMPLETE_DUNGEON, character_id, dungeon_id, 1);
}

/**
 * Trigger event: Level reached
 */
void QuestTrigger_OnLevelReached(int character_id, int level)
{
    if (!g_trigger_state.initialized) {
        return;
    }

    g_trigger_state.stats.levels_reached++;

    printf("[QuestTrigger] Level reached: character=%d, level=%d\n",
           character_id, level);

    FireTriggers(OBJECTIVE_REACH_LEVEL, character_id, level, 1);
}

/**
 * Trigger event: Item used
 */
void QuestTrigger_OnItemUsed(int character_id, int item_id)
{
    if (!g_trigger_state.initialized) {
        return;
    }

    g_trigger_state.stats.items_used++;

    printf("[QuestTrigger] Item used: character=%d, item=%d\n",
           character_id, item_id);

    FireTriggers(OBJECTIVE_USE_ITEM, character_id, item_id, 1);
}

/**
 * Cleanup quest trigger system
 */
void QuestTrigger_Cleanup(void)
{
    if (!g_trigger_state.initialized) {
        return;
    }

    printf("[QuestTrigger] Cleaning up quest trigger system...\n");
    printf("[QuestTrigger] Statistics:\n");
    printf("  - Triggers Registered: %u\n", g_trigger_state.stats.triggers_registered);
    printf("  - Triggers Unregistered: %u\n", g_trigger_state.stats.triggers_unregistered);
    printf("  - Monster Kills: %u\n", g_trigger_state.stats.monster_kills);
    printf("  - Items Obtained: %u\n", g_trigger_state.stats.items_obtained);
    printf("  - NPC Talks: %u\n", g_trigger_state.stats.npc_talks);
    printf("  - Dungeons Completed: %u\n", g_trigger_state.stats.dungeons_completed);
    printf("  - Levels Reached: %u\n", g_trigger_state.stats.levels_reached);
    printf("  - Items Used: %u\n", g_trigger_state.stats.items_used);

    memset(&g_trigger_state, 0, sizeof(g_trigger_state));

    printf("[QuestTrigger] Cleanup complete\n");
}
