/**
 * @file game_integration.h
 * @brief Game Systems Integration Layer
 *
 * Connects all game systems through the event system:
 * - Combat → Dungeon (monster kills)
 * - Combat → Quest (kill objectives)
 * - Combat → Achievement (combat achievements)
 * - Dungeon → Quest (dungeon clear objectives)
 * - Dungeon → Achievement (dungeon achievements)
 * - Guild → Achievement (guild achievements)
 * - PvP → Achievement (PvP achievements)
 *
 * @version 1.0
 * @date 2025-11-26
 */

#ifndef SRC_INTEGRATION_GAME_INTEGRATION_H_
#define SRC_INTEGRATION_GAME_INTEGRATION_H_

#include "../events/events_interface.h"

/*==============================================================================
 * Extended Event Types for Integration
 *============================================================================*/

/**
 * Combat-specific event data
 */
typedef struct {
    unsigned int attacker_id;       /* Character who dealt damage */
    unsigned int target_id;         /* Target (monster/player) */
    unsigned int monster_type_id;   /* Monster template ID (if monster) */
    unsigned int dungeon_id;        /* Dungeon instance ID (0 if not in dungeon) */
    int damage_dealt;               /* Total damage dealt */
    int is_kill;                    /* 1 if target was killed */
    int is_critical;                /* 1 if critical hit */
    int skill_id;                   /* Skill used (0 if basic attack) */
} CombatEventData;

/**
 * Dungeon-specific event data
 */
typedef struct {
    unsigned int dungeon_id;        /* Dungeon instance ID */
    unsigned int dungeon_type_id;   /* Dungeon template ID */
    unsigned int character_id;      /* Character involved */
    int difficulty;                 /* Difficulty level */
    int clear_time_seconds;         /* Time to clear (for clear events) */
    int monsters_killed;            /* Total monsters killed */
    int is_solo;                    /* 1 if solo clear */
} DungeonEventData;

/**
 * Quest-specific event data
 */
typedef struct {
    unsigned int character_id;
    unsigned int quest_id;
    unsigned int quest_template_id;
    int objective_index;            /* Which objective was updated */
    int progress_increment;         /* How much progress was made */
    int is_complete;                /* 1 if quest is now complete */
} QuestEventData;

/**
 * PvP-specific event data
 */
typedef struct {
    unsigned int winner_id;
    unsigned int loser_id;
    unsigned int match_id;
    int match_type;                 /* 1v1, 3v3, etc. */
    int rating_change_winner;
    int rating_change_loser;
    int duration_seconds;
} PvPEventData;

/**
 * Achievement unlock event data
 */
typedef struct {
    unsigned int character_id;
    unsigned int achievement_id;
    unsigned int achievement_type;
    int reward_gold;
    int reward_exp;
} AchievementEventData;

/*==============================================================================
 * Integration Event Types (Extended from events_interface.h)
 *============================================================================*/

typedef enum {
    /* Combat events (100-199) */
    INTEGRATION_EVENT_MONSTER_KILLED = 100,
    INTEGRATION_EVENT_PLAYER_KILLED = 101,
    INTEGRATION_EVENT_DAMAGE_DEALT = 102,
    INTEGRATION_EVENT_CRITICAL_HIT = 103,
    INTEGRATION_EVENT_COMBO_ACHIEVED = 104,

    /* Dungeon events (200-299) */
    INTEGRATION_EVENT_DUNGEON_ENTERED = 200,
    INTEGRATION_EVENT_DUNGEON_CLEARED = 201,
    INTEGRATION_EVENT_DUNGEON_FAILED = 202,
    INTEGRATION_EVENT_BOSS_KILLED = 203,
    INTEGRATION_EVENT_DUNGEON_TIMEOUT = 204,

    /* Quest events (300-399) */
    INTEGRATION_EVENT_QUEST_ACCEPTED = 300,
    INTEGRATION_EVENT_QUEST_PROGRESS = 301,
    INTEGRATION_EVENT_QUEST_COMPLETED = 302,
    INTEGRATION_EVENT_QUEST_ABANDONED = 303,

    /* PvP events (400-499) */
    INTEGRATION_EVENT_PVP_MATCH_START = 400,
    INTEGRATION_EVENT_PVP_MATCH_END = 401,
    INTEGRATION_EVENT_PVP_RANK_CHANGED = 402,

    /* Achievement events (500-599) */
    INTEGRATION_EVENT_ACHIEVEMENT_UNLOCKED = 500,
    INTEGRATION_EVENT_ACHIEVEMENT_PROGRESS = 501,

    /* Guild events (600-699) */
    INTEGRATION_EVENT_GUILD_CREATED = 600,
    INTEGRATION_EVENT_GUILD_LEVEL_UP = 601,
    INTEGRATION_EVENT_GUILD_MEMBER_JOINED = 602,

    /* Item events (700-799) */
    INTEGRATION_EVENT_ITEM_ACQUIRED = 700,
    INTEGRATION_EVENT_ITEM_USED = 701,
    INTEGRATION_EVENT_ITEM_ENHANCED = 702,

    /* Trade events (800-899) */
    INTEGRATION_EVENT_TRADE_COMPLETED = 800,
    INTEGRATION_EVENT_TRADE_CANCELLED = 801
} IntegrationEventType;

/*==============================================================================
 * API Functions
 *============================================================================*/

/**
 * Initialize game integration system
 * Registers all event listeners for cross-system communication
 *
 * @return 0 on success, error code on failure
 */
int GameIntegration_Initialize(void);

/**
 * Cleanup game integration system
 */
void GameIntegration_Cleanup(void);

/**
 * Notify: Monster was killed in combat
 * Triggers: Quest progress, Dungeon tracking, Achievement check
 *
 * @param killer_id Character who killed the monster
 * @param monster_type_id Type of monster killed
 * @param dungeon_id Dungeon ID (0 if not in dungeon)
 * @param damage_dealt Total damage dealt
 * @param skill_id Skill used for kill (0 if basic attack)
 * @return 0 on success
 */
int GameIntegration_NotifyMonsterKilled(
    unsigned int killer_id,
    unsigned int monster_type_id,
    unsigned int dungeon_id,
    int damage_dealt,
    int skill_id
);

/**
 * Notify: Dungeon was cleared
 * Triggers: Quest progress, Achievement check, Rewards
 *
 * @param dungeon_id Dungeon instance ID
 * @param dungeon_type_id Dungeon type
 * @param character_id Character who cleared
 * @param difficulty Difficulty level
 * @param clear_time_seconds Time to clear
 * @param monsters_killed Monsters killed count
 * @param is_solo Was this a solo clear
 * @return 0 on success
 */
int GameIntegration_NotifyDungeonCleared(
    unsigned int dungeon_id,
    unsigned int dungeon_type_id,
    unsigned int character_id,
    int difficulty,
    int clear_time_seconds,
    int monsters_killed,
    int is_solo
);

/**
 * Notify: PvP match ended
 * Triggers: Achievement check, Ranking update
 *
 * @param winner_id Winner character ID
 * @param loser_id Loser character ID
 * @param match_type Match type (1v1, 3v3, etc.)
 * @param duration_seconds Match duration
 * @return 0 on success
 */
int GameIntegration_NotifyPvPMatchEnd(
    unsigned int winner_id,
    unsigned int loser_id,
    int match_type,
    int duration_seconds
);

/**
 * Notify: Item was acquired
 * Triggers: Quest progress (collect objectives), Achievement check
 *
 * @param character_id Character who acquired item
 * @param item_id Item template ID
 * @param quantity Amount acquired
 * @param source How item was obtained (0=drop, 1=quest, 2=trade, 3=shop)
 * @return 0 on success
 */
int GameIntegration_NotifyItemAcquired(
    unsigned int character_id,
    unsigned int item_id,
    int quantity,
    int source
);

/**
 * Notify: Quest completed
 * Triggers: Achievement check, Follow-up quests
 *
 * @param character_id Character who completed quest
 * @param quest_id Quest template ID
 * @return 0 on success
 */
int GameIntegration_NotifyQuestCompleted(
    unsigned int character_id,
    unsigned int quest_id
);

/**
 * Notify: Guild level up
 * Triggers: Achievement check, Member notifications
 *
 * @param guild_id Guild ID
 * @param new_level New guild level
 * @return 0 on success
 */
int GameIntegration_NotifyGuildLevelUp(
    unsigned int guild_id,
    int new_level
);

/**
 * Process all pending integration events
 * Should be called periodically (e.g., in main game loop)
 *
 * @return Number of events processed
 */
int GameIntegration_ProcessEvents(void);

/**
 * Get integration statistics
 *
 * @param total_events Output: total events processed
 * @param quest_updates Output: quest progress updates triggered
 * @param achievement_checks Output: achievement checks triggered
 */
void GameIntegration_GetStatistics(
    unsigned int* total_events,
    unsigned int* quest_updates,
    unsigned int* achievement_checks
);

#endif /* SRC_INTEGRATION_GAME_INTEGRATION_H_ */
