/**
 * @file dungeon_manager.h
 * @brief Dungeon instance management
 *
 * Manages dungeon instances, rooms, and progression.
 */

#ifndef ENGINE_GAME_DUNGEON_DUNGEON_MANAGER_H
#define ENGINE_GAME_DUNGEON_DUNGEON_MANAGER_H

#include "../../core/types.h"
#include "../entity/entity.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DUNGEON_MAX_ROOMS       10
#define DUNGEON_MAX_MONSTERS    32
#define DUNGEON_MAX_PLAYERS     4
#define DUNGEON_MAX_INSTANCES   256
#define DUNGEON_TIME_LIMIT      1800000     /* 30 minutes */

/* Dungeon difficulty */
typedef enum {
    DUNGEON_DIFF_NORMAL = 0,
    DUNGEON_DIFF_HARD,
    DUNGEON_DIFF_EXPERT,
    DUNGEON_DIFF_MASTER,
    DUNGEON_DIFF_HELL,
    DUNGEON_DIFF_MAX
} DungeonDifficulty;

/* Dungeon state */
typedef enum {
    DUNGEON_STATE_WAITING = 0,  /* Waiting for players */
    DUNGEON_STATE_LOADING,      /* Loading resources */
    DUNGEON_STATE_RUNNING,      /* In progress */
    DUNGEON_STATE_BOSS,         /* Boss room */
    DUNGEON_STATE_CLEAR,        /* Cleared */
    DUNGEON_STATE_FAILED,       /* Failed (time/wipe) */
    DUNGEON_STATE_MAX
} DungeonState;

/* Room state */
typedef enum {
    ROOM_STATE_LOCKED = 0,      /* Not accessible yet */
    ROOM_STATE_OPEN,            /* Accessible */
    ROOM_STATE_ACTIVE,          /* Players inside */
    ROOM_STATE_CLEARED,         /* All monsters dead */
    ROOM_STATE_MAX
} RoomState;

/* Dungeon room */
typedef struct {
    u8          room_id;
    RoomState   state;
    u8          monster_count;
    u8          monsters_alive;
    u8          is_boss_room;
    u32         monster_ids[DUNGEON_MAX_MONSTERS];
} DungeonRoom;

/* Dungeon reward */
typedef struct {
    u32         exp;
    u32         gold;
    u16         item_ids[16];
    u8          item_counts[16];
    u8          item_count;
} DungeonReward;

/* Dungeon grade */
typedef enum {
    DUNGEON_GRADE_F = 0,
    DUNGEON_GRADE_D,
    DUNGEON_GRADE_C,
    DUNGEON_GRADE_B,
    DUNGEON_GRADE_A,
    DUNGEON_GRADE_S,
    DUNGEON_GRADE_SS,
    DUNGEON_GRADE_SSS,
    DUNGEON_GRADE_MAX
} DungeonGrade;

/* Dungeon instance */
typedef struct {
    u32                 instance_id;
    u32                 dungeon_id;
    DungeonDifficulty   difficulty;
    DungeonState        state;

    /* Players */
    u32                 player_ids[DUNGEON_MAX_PLAYERS];
    u8                  player_count;
    u8                  players_alive;

    /* Rooms */
    DungeonRoom         rooms[DUNGEON_MAX_ROOMS];
    u8                  room_count;
    u8                  current_room;

    /* Progress */
    u32                 start_time;
    u32                 clear_time;
    u32                 total_damage;
    u32                 total_kills;
    u32                 total_deaths;
    u32                 combo_count;
    u32                 max_combo;

    /* Scoring */
    DungeonGrade        grade;
    u32                 score;

    /* Rewards */
    DungeonReward       reward;

    /* Flags */
    u8                  active;
    u8                  is_pvp;
} DungeonInstance;

/* Dungeon manager */
typedef struct {
    DungeonInstance     instances[DUNGEON_MAX_INSTANCES];
    int                 instance_count;
    u32                 next_instance_id;
} DungeonManager;

/**
 * Initialize dungeon manager
 * @param manager Manager instance
 * @return ENGINE_OK on success
 */
int dungeon_manager_init(DungeonManager* manager);

/**
 * Cleanup dungeon manager
 * @param manager Manager instance
 */
void dungeon_manager_cleanup(DungeonManager* manager);

/**
 * Create dungeon instance
 * @param manager    Manager instance
 * @param dungeon_id Dungeon template ID
 * @param difficulty Difficulty level
 * @return Instance pointer, or NULL on failure
 */
DungeonInstance* dungeon_create(DungeonManager* manager, u32 dungeon_id,
                                DungeonDifficulty difficulty);

/**
 * Destroy dungeon instance
 * @param manager   Manager instance
 * @param instance  Instance to destroy
 */
void dungeon_destroy(DungeonManager* manager, DungeonInstance* instance);

/**
 * Get dungeon by ID
 * @param manager     Manager instance
 * @param instance_id Instance ID
 * @return Instance pointer, or NULL
 */
DungeonInstance* dungeon_get(DungeonManager* manager, u32 instance_id);

/**
 * Add player to dungeon
 * @param instance   Dungeon instance
 * @param player_id  Player entity ID
 * @return ENGINE_OK on success
 */
int dungeon_add_player(DungeonInstance* instance, u32 player_id);

/**
 * Remove player from dungeon
 * @param instance   Dungeon instance
 * @param player_id  Player entity ID
 */
void dungeon_remove_player(DungeonInstance* instance, u32 player_id);

/**
 * Start dungeon
 * @param instance     Dungeon instance
 * @param current_time Current time
 * @return ENGINE_OK on success
 */
int dungeon_start(DungeonInstance* instance, u32 current_time);

/**
 * Update dungeon state
 * @param instance     Dungeon instance
 * @param current_time Current time
 */
void dungeon_update(DungeonInstance* instance, u32 current_time);

/**
 * Notify monster killed
 * @param instance   Dungeon instance
 * @param room_id    Room ID
 * @param monster_id Monster ID
 */
void dungeon_on_monster_kill(DungeonInstance* instance, u8 room_id,
                             u32 monster_id);

/**
 * Notify player died
 * @param instance  Dungeon instance
 * @param player_id Player ID
 */
void dungeon_on_player_death(DungeonInstance* instance, u32 player_id);

/**
 * Move to next room
 * @param instance Dungeon instance
 * @return ENGINE_OK on success
 */
int dungeon_next_room(DungeonInstance* instance);

/**
 * Clear dungeon (all rooms completed)
 * @param instance     Dungeon instance
 * @param current_time Current time
 */
void dungeon_clear(DungeonInstance* instance, u32 current_time);

/**
 * Calculate grade and score
 * @param instance Dungeon instance
 */
void dungeon_calculate_grade(DungeonInstance* instance);

/**
 * Generate rewards
 * @param instance Dungeon instance
 */
void dungeon_generate_rewards(DungeonInstance* instance);

/**
 * Get difficulty name
 * @param difficulty Difficulty level
 * @return Difficulty name string
 */
const char* dungeon_difficulty_name(DungeonDifficulty difficulty);

/**
 * Get state name
 * @param state Dungeon state
 * @return State name string
 */
const char* dungeon_state_name(DungeonState state);

/**
 * Get grade name
 * @param grade Dungeon grade
 * @return Grade name string
 */
const char* dungeon_grade_name(DungeonGrade grade);

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_GAME_DUNGEON_DUNGEON_MANAGER_H */
