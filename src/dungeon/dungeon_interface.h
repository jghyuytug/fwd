/**
 * Dungeon Module - Public Interface
 * Purpose: Dungeon instance management, monster spawning, clearance tracking
 *
 * Components:
 * - Dungeon instance creation and management
 * - Monster spawning and wave control
 * - Clearance tracking and rewards
 * - Difficulty scaling
 * - Boss encounter management
 */

#ifndef SRC_DUNGEON_DUNGEON_INTERFACE_H_
#define SRC_DUNGEON_DUNGEON_INTERFACE_H_

#include <common/defs.h>
#include <common/error_codes.h>

/**
 * Dungeon difficulty levels
 */
typedef enum {
    DIFFICULTY_NORMAL = 0,
    DIFFICULTY_EXPERT = 1,
    DIFFICULTY_MASTER = 2,
    DIFFICULTY_KING = 3,
    DIFFICULTY_SLAYER = 4
} DungeonDifficulty;

/**
 * Dungeon states
 */
typedef enum {
    DUNGEON_STATE_IDLE = 0,
    DUNGEON_STATE_ACTIVE = 1,
    DUNGEON_STATE_BOSS_FIGHT = 2,
    DUNGEON_STATE_CLEARED = 3,
    DUNGEON_STATE_FAILED = 4
} DungeonState;

/**
 * Monster types
 */
typedef enum {
    MONSTER_TYPE_NORMAL = 0,
    MONSTER_TYPE_ELITE = 1,
    MONSTER_TYPE_BOSS = 2,
    MONSTER_TYPE_NAMED = 3
} MonsterType;

/**
 * Dungeon instance structure
 */
typedef struct {
    unsigned int dungeon_id;
    unsigned int dungeon_template_id;
    DungeonDifficulty difficulty;
    DungeonState state;
    unsigned int owner_user_id;
    unsigned int party_id;
    unsigned int start_time;
    unsigned int clear_time;
    int time_limit_seconds;
    int monsters_killed;
    int total_monsters;
    int is_solo;
} DungeonInstance;

/**
 * Monster spawn information
 */
typedef struct {
    unsigned int monster_id;
    unsigned int monster_template_id;
    MonsterType monster_type;
    unsigned int dungeon_id;
    int spawn_x;
    int spawn_y;
    int spawn_z;
    int health;
    int max_health;
    int level;
    int is_alive;
} MonsterInfo;

/**
 * Dungeon clearance record
 */
typedef struct {
    unsigned int user_id;
    unsigned int dungeon_template_id;
    DungeonDifficulty difficulty;
    unsigned int clear_count;
    unsigned int fastest_time;
    unsigned int last_clear_time;
} DungeonClearance;

/**
 * Initialize Dungeon Module
 *
 * Sets up dungeon instance management system
 *
 * Parameters:
 *   max_instances: Maximum simultaneous dungeon instances
 *   max_monsters: Maximum monsters per instance
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Dungeon_Initialize(int max_instances, int max_monsters);

/**
 * Cleanup Dungeon Module
 *
 * Releases dungeon system resources
 */
void Dungeon_Cleanup();

/**
 * Create Dungeon Instance
 *
 * Creates a new dungeon instance for user/party
 *
 * Parameters:
 *   template_id: Dungeon template ID
 *   owner_id: User ID who created the dungeon
 *   party_id: Party ID (0 for solo)
 *   difficulty: Dungeon difficulty level
 *
 * Returns: Dungeon instance ID on success, negative error code on failure
 */
int Dungeon_CreateInstance(unsigned int template_id,
                           unsigned int owner_id,
                           unsigned int party_id,
                           DungeonDifficulty difficulty);

/**
 * Destroy Dungeon Instance
 *
 * Destroys a dungeon instance and cleanup resources
 *
 * Parameters:
 *   dungeon_id: Dungeon instance ID
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Dungeon_DestroyInstance(unsigned int dungeon_id);

/**
 * Enter Dungeon
 *
 * User enters a dungeon instance
 *
 * Parameters:
 *   dungeon_id: Dungeon instance ID
 *   user_id: User ID entering the dungeon
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Dungeon_Enter(unsigned int dungeon_id, unsigned int user_id);

/**
 * Exit Dungeon
 *
 * User exits a dungeon instance
 *
 * Parameters:
 *   dungeon_id: Dungeon instance ID
 *   user_id: User ID exiting the dungeon
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Dungeon_Exit(unsigned int dungeon_id, unsigned int user_id);

/**
 * Spawn Monster
 *
 * Spawns a monster in dungeon instance
 *
 * Parameters:
 *   dungeon_id: Dungeon instance ID
 *   monster_template_id: Monster template ID
 *   monster_type: Monster type
 *   x, y, z: Spawn coordinates
 *
 * Returns: Monster ID on success, negative error code on failure
 */
int Dungeon_SpawnMonster(unsigned int dungeon_id,
                         unsigned int monster_template_id,
                         MonsterType monster_type,
                         int x, int y, int z);

/**
 * Kill Monster
 *
 * Marks monster as killed
 *
 * Parameters:
 *   monster_id: Monster ID
 *   killer_user_id: User who killed the monster
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Dungeon_KillMonster(unsigned int monster_id, unsigned int killer_user_id);

/**
 * Start Boss Fight
 *
 * Initiates boss encounter in dungeon
 *
 * Parameters:
 *   dungeon_id: Dungeon instance ID
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Dungeon_StartBossFight(unsigned int dungeon_id);

/**
 * Complete Dungeon
 *
 * Marks dungeon as cleared and process rewards
 *
 * Parameters:
 *   dungeon_id: Dungeon instance ID
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Dungeon_Complete(unsigned int dungeon_id);

/**
 * Fail Dungeon
 *
 * Marks dungeon as failed
 *
 * Parameters:
 *   dungeon_id: Dungeon instance ID
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Dungeon_Fail(unsigned int dungeon_id);

/**
 * Get Dungeon Instance Info
 *
 * Retrieves dungeon instance information
 *
 * Parameters:
 *   dungeon_id: Dungeon instance ID
 *   instance: Output instance structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Dungeon_GetInstanceInfo(unsigned int dungeon_id, DungeonInstance* instance);

/**
 * Get Dungeon Clearance Record
 *
 * Retrieves user's clearance record for dungeon
 *
 * Parameters:
 *   user_id: User ID
 *   template_id: Dungeon template ID
 *   clearance: Output clearance structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Dungeon_GetClearanceRecord(unsigned int user_id,
                               unsigned int template_id,
                               DungeonClearance* clearance);

/**
 * Update Dungeon State
 *
 * Processes dungeon tick, checks time limits, monster waves
 *
 * Parameters:
 *   delta_time_ms: Time elapsed since last update (milliseconds)
 *
 * Returns: Number of dungeons processed
 */
int Dungeon_Update(int delta_time_ms);

/**
 * Get Active Dungeons Count
 *
 * Returns number of active dungeon instances
 */
int Dungeon_GetActiveCount();

/**
 * Get Monsters in Dungeon
 *
 * Retrieves list of monsters in dungeon instance
 *
 * Parameters:
 *   dungeon_id: Dungeon instance ID
 *   monsters: Output array of monster info
 *   max_monsters: Maximum monsters to retrieve
 *
 * Returns: Number of monsters in dungeon
 */
int Dungeon_GetMonsters(unsigned int dungeon_id,
                        MonsterInfo* monsters,
                        int max_monsters);

/**
 * Get Remaining Time
 *
 * Gets remaining time for dungeon instance
 *
 * Parameters:
 *   dungeon_id: Dungeon instance ID
 *
 * Returns: Remaining time in seconds, negative if expired
 */
int Dungeon_GetRemainingTime(unsigned int dungeon_id);

#endif // SRC_DUNGEON_DUNGEON_INTERFACE_H_
