/**
 * @file game_integration.c
 * @brief Game Systems Integration Layer - Implementation
 *
 * Connects Combat, Dungeon, Quest, Achievement, PvP, Guild systems
 * through event-driven architecture.
 *
 * @version 1.0
 * @date 2025-11-26
 */

#include "game_integration.h"
#include "../events/events_interface.h"
#include "../../include/quest/quest_manager.h"
#include "../../include/achievement/achievement_interface.h"
#include "../../include/dungeon/dungeon_interface.h"
#include "../../include/pvp/pvp_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*==============================================================================
 * Achievement Type -> Achievement ID Mapping
 * These IDs should match the achievement definitions loaded at startup
 *============================================================================*/

/* Achievement IDs for common triggers (should match loaded definitions) */
#define ACHIEVEMENT_ID_FIRST_KILL           1001
#define ACHIEVEMENT_ID_KILL_100_MONSTERS    1002
#define ACHIEVEMENT_ID_KILL_1000_MONSTERS   1003
#define ACHIEVEMENT_ID_CRITICAL_MASTER      1010
#define ACHIEVEMENT_ID_FIRST_DUNGEON        2001
#define ACHIEVEMENT_ID_DUNGEON_MASTER       2002
#define ACHIEVEMENT_ID_SOLO_WARRIOR         2010
#define ACHIEVEMENT_ID_SPEED_RUNNER         2020
#define ACHIEVEMENT_ID_HIGH_DIFFICULTY      2030
#define ACHIEVEMENT_ID_PVP_FIRST_WIN        3001
#define ACHIEVEMENT_ID_PVP_10_WINS          3002
#define ACHIEVEMENT_ID_PVP_VETERAN          3010
#define ACHIEVEMENT_ID_QUEST_BEGINNER       4001
#define ACHIEVEMENT_ID_QUEST_MASTER         4002
#define ACHIEVEMENT_ID_ITEM_COLLECTOR       5001

/*==============================================================================
 * Module State
 *============================================================================*/

typedef struct {
    int initialized;

    /* Listener IDs for cleanup */
    int listener_combat_kill;
    int listener_dungeon_clear;
    int listener_dungeon_fail;
    int listener_quest_complete;
    int listener_pvp_end;
    int listener_item_acquire;

    /* Statistics */
    struct {
        unsigned int total_events_processed;
        unsigned int quest_updates_triggered;
        unsigned int achievement_checks_triggered;
        unsigned int dungeon_updates_triggered;
        unsigned int pvp_updates_triggered;
    } stats;
} IntegrationState;

static IntegrationState g_state = {0};

/*==============================================================================
 * Forward Declarations - Event Handlers
 *============================================================================*/

static void OnMonsterKilled(GameEvent* event, void* context);
static void OnDungeonCleared(GameEvent* event, void* context);
static void OnDungeonFailed(GameEvent* event, void* context);
static void OnQuestCompleted(GameEvent* event, void* context);
static void OnPvPMatchEnd(GameEvent* event, void* context);
static void OnItemAcquired(GameEvent* event, void* context);

/*==============================================================================
 * Quest Integration Helpers
 *============================================================================*/

/**
 * Update quest progress for monster kill objectives
 */
static void UpdateQuestProgressForKill(unsigned int character_id, unsigned int monster_type_id)
{
    /* Call QuestManager to update progress for KILL objectives */
    int updated = QuestManager_UpdateProgress(
        character_id,
        OBJECTIVE_KILL_MONSTER,  /* From quest_interface.h */
        monster_type_id,
        1  /* increment by 1 */
    );

    if (updated > 0) {
        g_state.stats.quest_updates_triggered++;
        printf("[Integration] Quest progress updated: %d quests for character %u (killed monster %u)\n",
               updated, character_id, monster_type_id);
    }
}

/**
 * Update quest progress for dungeon clear objectives
 */
static void UpdateQuestProgressForDungeonClear(unsigned int character_id, unsigned int dungeon_type_id)
{
    int updated = QuestManager_UpdateProgress(
        character_id,
        OBJECTIVE_CLEAR_DUNGEON,
        dungeon_type_id,
        1
    );

    if (updated > 0) {
        g_state.stats.quest_updates_triggered++;
        printf("[Integration] Quest progress updated: %d quests for character %u (cleared dungeon %u)\n",
               updated, character_id, dungeon_type_id);
    }
}

/**
 * Update quest progress for item collection objectives
 */
static void UpdateQuestProgressForItem(unsigned int character_id, unsigned int item_id, int quantity)
{
    int updated = QuestManager_UpdateProgress(
        character_id,
        OBJECTIVE_COLLECT_ITEM,
        item_id,
        quantity
    );

    if (updated > 0) {
        g_state.stats.quest_updates_triggered++;
        printf("[Integration] Quest progress updated: %d quests for character %u (collected item %u x%d)\n",
               updated, character_id, item_id, quantity);
    }
}

/*==============================================================================
 * Achievement Integration Helpers
 *============================================================================*/

/**
 * Check achievements for monster kill
 */
static void CheckAchievementsForKill(unsigned int character_id, unsigned int monster_type_id, int is_critical)
{
    (void)monster_type_id;  /* May be used for specific monster achievements */

    /* Update kill count achievements */
    Achievement_UpdateProgress(character_id, ACHIEVEMENT_ID_FIRST_KILL, 1);
    Achievement_UpdateProgress(character_id, ACHIEVEMENT_ID_KILL_100_MONSTERS, 1);
    Achievement_UpdateProgress(character_id, ACHIEVEMENT_ID_KILL_1000_MONSTERS, 1);

    if (is_critical) {
        Achievement_UpdateProgress(character_id, ACHIEVEMENT_ID_CRITICAL_MASTER, 1);
    }

    g_state.stats.achievement_checks_triggered++;
}

/**
 * Check achievements for dungeon clear
 */
static void CheckAchievementsForDungeonClear(
    unsigned int character_id,
    unsigned int dungeon_type_id,
    int difficulty,
    int clear_time_seconds,
    int is_solo)
{
    (void)dungeon_type_id;

    /* General dungeon clear achievements */
    Achievement_UpdateProgress(character_id, ACHIEVEMENT_ID_FIRST_DUNGEON, 1);
    Achievement_UpdateProgress(character_id, ACHIEVEMENT_ID_DUNGEON_MASTER, 1);

    /* Solo clear achievement */
    if (is_solo) {
        Achievement_UpdateProgress(character_id, ACHIEVEMENT_ID_SOLO_WARRIOR, 1);
    }

    /* Speed clear achievement (under 3 minutes) */
    if (clear_time_seconds < 180) {
        Achievement_UpdateProgress(character_id, ACHIEVEMENT_ID_SPEED_RUNNER, 1);
    }

    /* High difficulty achievement */
    if (difficulty >= 3) {  /* MASTER or higher */
        Achievement_UpdateProgress(character_id, ACHIEVEMENT_ID_HIGH_DIFFICULTY, 1);
    }

    g_state.stats.achievement_checks_triggered++;
}

/**
 * Check achievements for PvP
 */
static void CheckAchievementsForPvP(unsigned int winner_id, unsigned int loser_id)
{
    /* Winner gets PvP win achievement progress */
    Achievement_UpdateProgress(winner_id, ACHIEVEMENT_ID_PVP_FIRST_WIN, 1);
    Achievement_UpdateProgress(winner_id, ACHIEVEMENT_ID_PVP_10_WINS, 1);

    /* Both get PvP participation (veteran) */
    Achievement_UpdateProgress(winner_id, ACHIEVEMENT_ID_PVP_VETERAN, 1);
    Achievement_UpdateProgress(loser_id, ACHIEVEMENT_ID_PVP_VETERAN, 1);

    g_state.stats.achievement_checks_triggered += 2;
}

/*==============================================================================
 * Event Handlers
 *============================================================================*/

/**
 * Handler: Monster killed in combat
 */
static void OnMonsterKilled(GameEvent* event, void* context)
{
    (void)context;  /* Unused */

    if (!event || !event->event_data) {
        return;
    }

    CombatEventData* data = (CombatEventData*)event->event_data;

    printf("[Integration] Monster killed event: attacker=%u, monster_type=%u, dungeon=%u\n",
           data->attacker_id, data->monster_type_id, data->dungeon_id);

    /* 1. Update quest progress */
    UpdateQuestProgressForKill(data->attacker_id, data->monster_type_id);

    /* 2. Check achievements */
    CheckAchievementsForKill(data->attacker_id, data->monster_type_id, data->is_critical);

    /* 3. Update dungeon monster count if in dungeon */
    if (data->dungeon_id > 0) {
        DungeonManager_KillMonster(data->dungeon_id, data->target_id);
        g_state.stats.dungeon_updates_triggered++;
    }

    g_state.stats.total_events_processed++;
}

/**
 * Handler: Dungeon cleared
 */
static void OnDungeonCleared(GameEvent* event, void* context)
{
    (void)context;

    if (!event || !event->event_data) {
        return;
    }

    DungeonEventData* data = (DungeonEventData*)event->event_data;

    printf("[Integration] Dungeon cleared event: dungeon_type=%u, character=%u, difficulty=%d\n",
           data->dungeon_type_id, data->character_id, data->difficulty);

    /* 1. Update quest progress */
    UpdateQuestProgressForDungeonClear(data->character_id, data->dungeon_type_id);

    /* 2. Check achievements */
    CheckAchievementsForDungeonClear(
        data->character_id,
        data->dungeon_type_id,
        data->difficulty,
        data->clear_time_seconds,
        data->is_solo
    );

    g_state.stats.total_events_processed++;
}

/**
 * Handler: Dungeon failed
 */
static void OnDungeonFailed(GameEvent* event, void* context)
{
    (void)context;

    if (!event || !event->event_data) {
        return;
    }

    DungeonEventData* data = (DungeonEventData*)event->event_data;

    printf("[Integration] Dungeon failed event: dungeon_type=%u, character=%u\n",
           data->dungeon_type_id, data->character_id);

    /* Could track failure statistics or unlock "try again" type achievements */
    g_state.stats.total_events_processed++;
}

/**
 * Handler: Quest completed
 */
static void OnQuestCompleted(GameEvent* event, void* context)
{
    (void)context;

    if (!event || !event->event_data) {
        return;
    }

    QuestEventData* data = (QuestEventData*)event->event_data;

    printf("[Integration] Quest completed event: quest=%u, character=%u\n",
           data->quest_template_id, data->character_id);

    /* Check quest-related achievements */
    Achievement_UpdateProgress(data->character_id, ACHIEVEMENT_ID_QUEST_BEGINNER, 1);
    Achievement_UpdateProgress(data->character_id, ACHIEVEMENT_ID_QUEST_MASTER, 1);
    g_state.stats.achievement_checks_triggered++;

    g_state.stats.total_events_processed++;
}

/**
 * Handler: PvP match ended
 */
static void OnPvPMatchEnd(GameEvent* event, void* context)
{
    (void)context;

    if (!event || !event->event_data) {
        return;
    }

    PvPEventData* data = (PvPEventData*)event->event_data;

    printf("[Integration] PvP match ended: winner=%u, loser=%u, type=%d\n",
           data->winner_id, data->loser_id, data->match_type);

    /* Check PvP achievements */
    CheckAchievementsForPvP(data->winner_id, data->loser_id);

    g_state.stats.pvp_updates_triggered++;
    g_state.stats.total_events_processed++;
}

/**
 * Handler: Item acquired
 */
static void OnItemAcquired(GameEvent* event, void* context)
{
    (void)context;

    if (!event || !event->event_data) {
        return;
    }

    /* Generic item data - using target_id as item_id and creator_id as character_id */
    unsigned int character_id = event->creator_id;
    unsigned int item_id = event->target_id;

    printf("[Integration] Item acquired event: character=%u, item=%u\n",
           character_id, item_id);

    /* Update quest progress for collection objectives */
    UpdateQuestProgressForItem(character_id, item_id, 1);

    /* Check item collection achievements */
    Achievement_UpdateProgress(character_id, ACHIEVEMENT_ID_ITEM_COLLECTOR, 1);
    g_state.stats.achievement_checks_triggered++;

    g_state.stats.total_events_processed++;
}

/*==============================================================================
 * Public API Implementation
 *============================================================================*/

int GameIntegration_Initialize(void)
{
    if (g_state.initialized) {
        fprintf(stderr, "[Integration] Already initialized\n");
        return -1;
    }

    printf("[Integration] Initializing game integration system...\n");

    memset(&g_state, 0, sizeof(IntegrationState));

    /* Register event listeners */

    /* Combat events -> Quest/Achievement/Dungeon */
    g_state.listener_combat_kill = Events_RegisterListener(
        EVENT_TYPE_CUSTOM,  /* Using CUSTOM for integration events */
        OnMonsterKilled,
        NULL
    );

    /* Dungeon events -> Quest/Achievement */
    g_state.listener_dungeon_clear = Events_RegisterListener(
        EVENT_TYPE_DUNGEON_CLEAR,
        OnDungeonCleared,
        NULL
    );

    g_state.listener_dungeon_fail = Events_RegisterListener(
        EVENT_TYPE_DUNGEON_FAIL,
        OnDungeonFailed,
        NULL
    );

    /* Quest events -> Achievement */
    g_state.listener_quest_complete = Events_RegisterListener(
        EVENT_TYPE_QUEST_COMPLETED,
        OnQuestCompleted,
        NULL
    );

    /* PvP events -> Achievement */
    g_state.listener_pvp_end = Events_RegisterListener(
        EVENT_TYPE_PVP_WIN,
        OnPvPMatchEnd,
        NULL
    );

    /* Item events -> Quest */
    g_state.listener_item_acquire = Events_RegisterListener(
        EVENT_TYPE_ITEM_OBTAINED,
        OnItemAcquired,
        NULL
    );

    g_state.initialized = 1;

    printf("[Integration] Game integration system initialized\n");
    printf("[Integration] Registered %d event listeners\n", 6);

    return 0;
}

void GameIntegration_Cleanup(void)
{
    if (!g_state.initialized) {
        return;
    }

    printf("[Integration] Cleaning up game integration system...\n");

    /* Unregister all listeners */
    if (g_state.listener_combat_kill > 0) {
        Events_UnregisterListener(g_state.listener_combat_kill);
    }
    if (g_state.listener_dungeon_clear > 0) {
        Events_UnregisterListener(g_state.listener_dungeon_clear);
    }
    if (g_state.listener_dungeon_fail > 0) {
        Events_UnregisterListener(g_state.listener_dungeon_fail);
    }
    if (g_state.listener_quest_complete > 0) {
        Events_UnregisterListener(g_state.listener_quest_complete);
    }
    if (g_state.listener_pvp_end > 0) {
        Events_UnregisterListener(g_state.listener_pvp_end);
    }
    if (g_state.listener_item_acquire > 0) {
        Events_UnregisterListener(g_state.listener_item_acquire);
    }

    printf("[Integration] Statistics:\n");
    printf("  - Total Events Processed: %u\n", g_state.stats.total_events_processed);
    printf("  - Quest Updates Triggered: %u\n", g_state.stats.quest_updates_triggered);
    printf("  - Achievement Checks: %u\n", g_state.stats.achievement_checks_triggered);
    printf("  - Dungeon Updates: %u\n", g_state.stats.dungeon_updates_triggered);
    printf("  - PvP Updates: %u\n", g_state.stats.pvp_updates_triggered);

    memset(&g_state, 0, sizeof(IntegrationState));
}

int GameIntegration_NotifyMonsterKilled(
    unsigned int killer_id,
    unsigned int monster_type_id,
    unsigned int dungeon_id,
    int damage_dealt,
    int skill_id)
{
    if (!g_state.initialized) {
        return -1;
    }

    /* Allocate event data */
    CombatEventData* data = malloc(sizeof(CombatEventData));
    if (!data) {
        return -1;
    }

    data->attacker_id = killer_id;
    data->target_id = 0;  /* Will be set by caller if needed */
    data->monster_type_id = monster_type_id;
    data->dungeon_id = dungeon_id;
    data->damage_dealt = damage_dealt;
    data->is_kill = 1;
    data->is_critical = 0;
    data->skill_id = skill_id;

    /* Create and queue the event */
    int event_id = Events_CreateEvent(
        EVENT_TYPE_CUSTOM,
        killer_id,
        monster_type_id,
        EVENT_PRIORITY_NORMAL,
        data
    );

    return event_id > 0 ? 0 : -1;
}

int GameIntegration_NotifyDungeonCleared(
    unsigned int dungeon_id,
    unsigned int dungeon_type_id,
    unsigned int character_id,
    int difficulty,
    int clear_time_seconds,
    int monsters_killed,
    int is_solo)
{
    if (!g_state.initialized) {
        return -1;
    }

    DungeonEventData* data = malloc(sizeof(DungeonEventData));
    if (!data) {
        return -1;
    }

    data->dungeon_id = dungeon_id;
    data->dungeon_type_id = dungeon_type_id;
    data->character_id = character_id;
    data->difficulty = difficulty;
    data->clear_time_seconds = clear_time_seconds;
    data->monsters_killed = monsters_killed;
    data->is_solo = is_solo;

    int event_id = Events_CreateEvent(
        EVENT_TYPE_DUNGEON_CLEAR,
        character_id,
        dungeon_type_id,
        EVENT_PRIORITY_HIGH,
        data
    );

    return event_id > 0 ? 0 : -1;
}

int GameIntegration_NotifyPvPMatchEnd(
    unsigned int winner_id,
    unsigned int loser_id,
    int match_type,
    int duration_seconds)
{
    if (!g_state.initialized) {
        return -1;
    }

    PvPEventData* data = malloc(sizeof(PvPEventData));
    if (!data) {
        return -1;
    }

    data->winner_id = winner_id;
    data->loser_id = loser_id;
    data->match_id = 0;
    data->match_type = match_type;
    data->rating_change_winner = 0;
    data->rating_change_loser = 0;
    data->duration_seconds = duration_seconds;

    int event_id = Events_CreateEvent(
        EVENT_TYPE_PVP_WIN,
        winner_id,
        loser_id,
        EVENT_PRIORITY_NORMAL,
        data
    );

    return event_id > 0 ? 0 : -1;
}

int GameIntegration_NotifyItemAcquired(
    unsigned int character_id,
    unsigned int item_id,
    int quantity,
    int source)
{
    (void)quantity;  /* TODO: Use in extended event data */
    (void)source;

    if (!g_state.initialized) {
        return -1;
    }

    int event_id = Events_CreateEvent(
        EVENT_TYPE_ITEM_OBTAINED,
        character_id,
        item_id,
        EVENT_PRIORITY_LOW,
        NULL
    );

    return event_id > 0 ? 0 : -1;
}

int GameIntegration_NotifyQuestCompleted(
    unsigned int character_id,
    unsigned int quest_id)
{
    if (!g_state.initialized) {
        return -1;
    }

    QuestEventData* data = malloc(sizeof(QuestEventData));
    if (!data) {
        return -1;
    }

    data->character_id = character_id;
    data->quest_id = 0;
    data->quest_template_id = quest_id;
    data->objective_index = -1;
    data->progress_increment = 0;
    data->is_complete = 1;

    int event_id = Events_CreateEvent(
        EVENT_TYPE_QUEST_COMPLETED,
        character_id,
        quest_id,
        EVENT_PRIORITY_NORMAL,
        data
    );

    return event_id > 0 ? 0 : -1;
}

int GameIntegration_NotifyGuildLevelUp(
    unsigned int guild_id,
    int new_level)
{
    if (!g_state.initialized) {
        return -1;
    }

    int event_id = Events_CreateEvent(
        EVENT_TYPE_CUSTOM,
        guild_id,
        new_level,
        EVENT_PRIORITY_LOW,
        NULL
    );

    return event_id > 0 ? 0 : -1;
}

int GameIntegration_ProcessEvents(void)
{
    if (!g_state.initialized) {
        return 0;
    }

    /* Process up to 100 events per call */
    return Events_ProcessPending(100);
}

void GameIntegration_GetStatistics(
    unsigned int* total_events,
    unsigned int* quest_updates,
    unsigned int* achievement_checks)
{
    if (total_events) {
        *total_events = g_state.stats.total_events_processed;
    }
    if (quest_updates) {
        *quest_updates = g_state.stats.quest_updates_triggered;
    }
    if (achievement_checks) {
        *achievement_checks = g_state.stats.achievement_checks_triggered;
    }
}
