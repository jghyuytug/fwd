/**
 * Quest Module - Public Interface
 * Purpose: Quest/Mission management, progression tracking, rewards
 *
 * Components:
 * - Quest assignment and tracking
 * - Quest objective management
 * - Quest completion and rewards
 * - Daily/Weekly quest rotation
 * - Achievement integration
 */

#ifndef SRC_QUEST_QUEST_INTERFACE_H_
#define SRC_QUEST_QUEST_INTERFACE_H_

#include <common/defs.h>
#include <common/error_codes.h>

/**
 * Quest types
 */
typedef enum {
    QUEST_TYPE_MAIN = 0,        // Main story quest
    QUEST_TYPE_SIDE = 1,        // Side quest
    QUEST_TYPE_DAILY = 2,       // Daily quest
    QUEST_TYPE_WEEKLY = 3,      // Weekly quest
    QUEST_TYPE_EVENT = 4,       // Event quest
    QUEST_TYPE_ACHIEVEMENT = 5, // Achievement quest
    QUEST_TYPE_REPEATABLE = 6   // Repeatable quest
} QuestType;

/**
 * Quest states
 */
typedef enum {
    QUEST_STATE_AVAILABLE = 0,   // Quest is available
    QUEST_STATE_ACCEPTED = 1,    // Quest accepted
    QUEST_STATE_IN_PROGRESS = 2, // Quest in progress
    QUEST_STATE_COMPLETED = 3,   // Quest objectives completed
    QUEST_STATE_CLAIMED = 4,     // Quest rewards claimed
    QUEST_STATE_FAILED = 5,      // Quest failed
    QUEST_STATE_EXPIRED = 6      // Quest expired
} QuestState;

/**
 * Quest objective types
 */
typedef enum {
    OBJECTIVE_KILL_MONSTER = 0,    // Kill X monsters
    OBJECTIVE_COLLECT_ITEM = 1,    // Collect X items
    OBJECTIVE_REACH_LEVEL = 2,     // Reach level X
    OBJECTIVE_COMPLETE_DUNGEON = 3,// Complete dungeon X times
    OBJECTIVE_TALK_TO_NPC = 4,     // Talk to NPC
    OBJECTIVE_USE_ITEM = 5,        // Use item X times
    OBJECTIVE_EQUIP_ITEM = 6,      // Equip item
    OBJECTIVE_CLEAR_AREA = 7       // Clear area
} ObjectiveType;

/**
 * Quest objective structure
 */
typedef struct {
    unsigned int objective_id;
    ObjectiveType objective_type;
    unsigned int target_id;        // Monster ID, Item ID, etc.
    int required_count;            // How many needed
    int current_count;             // Current progress
    int is_completed;              // 1 if completed
} QuestObjective;

/**
 * Quest information structure
 */
typedef struct {
    unsigned int quest_id;
    unsigned int quest_template_id;
    QuestType quest_type;
    QuestState quest_state;
    unsigned int user_id;
    unsigned int accept_time;
    unsigned int complete_time;
    unsigned int expire_time;
    int min_level;
    int max_level;
    int objective_count;
    QuestObjective objectives[10];  // Up to 10 objectives per quest
} QuestInfo;

/**
 * Quest statistics structure
 */
typedef struct {
    unsigned int total_quests_completed;
    unsigned int daily_quests_completed;
    unsigned int weekly_quests_completed;
    unsigned int main_quests_completed;
    unsigned int side_quests_completed;
    unsigned int quests_failed;
    unsigned int total_rewards_claimed;
} QuestStats;

/**
 * Initialize Quest Module
 *
 * Sets up quest management system
 *
 * Parameters:
 *   max_active_quests: Maximum active quests per user
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Quest_Initialize(int max_active_quests);

/**
 * Cleanup Quest Module
 *
 * Releases quest system resources
 */
void Quest_Cleanup();

/**
 * Accept Quest
 *
 * User accepts a quest
 *
 * Parameters:
 *   user_id: User ID
 *   quest_template_id: Quest template ID
 *
 * Returns: Quest ID on success, negative error code on failure
 */
int Quest_Accept(unsigned int user_id, unsigned int quest_template_id);

/**
 * Abandon Quest
 *
 * User abandons a quest
 *
 * Parameters:
 *   quest_id: Quest ID
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Quest_Abandon(unsigned int quest_id);

/**
 * Update Quest Progress
 *
 * Updates quest objective progress
 *
 * Parameters:
 *   user_id: User ID
 *   objective_type: Type of objective
 *   target_id: Target ID (monster, item, etc.)
 *   count: Progress amount
 *
 * Returns: Number of quests updated
 */
int Quest_UpdateProgress(unsigned int user_id,
                         ObjectiveType objective_type,
                         unsigned int target_id,
                         int count);

/**
 * Complete Quest
 *
 * Marks quest as completed
 *
 * Parameters:
 *   quest_id: Quest ID
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Quest_Complete(unsigned int quest_id);

/**
 * Claim Quest Rewards
 *
 * Claims quest rewards
 *
 * Parameters:
 *   quest_id: Quest ID
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Quest_ClaimRewards(unsigned int quest_id);

/**
 * Get Quest Info
 *
 * Retrieves quest information
 *
 * Parameters:
 *   quest_id: Quest ID
 *   quest_info: Output quest structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Quest_GetInfo(unsigned int quest_id, QuestInfo* quest_info);

/**
 * Get User Quests
 *
 * Retrieves list of user's active quests
 *
 * Parameters:
 *   user_id: User ID
 *   quests: Output array of quest info
 *   max_quests: Maximum quests to retrieve
 *
 * Returns: Number of active quests
 */
int Quest_GetUserQuests(unsigned int user_id,
                        QuestInfo* quests,
                        int max_quests);

/**
 * Get Available Quests
 *
 * Gets list of quests available to user
 *
 * Parameters:
 *   user_id: User ID
 *   quests: Output array of quest info
 *   max_quests: Maximum quests to retrieve
 *
 * Returns: Number of available quests
 */
int Quest_GetAvailableQuests(unsigned int user_id,
                             QuestInfo* quests,
                             int max_quests);

/**
 * Get Quest Statistics
 *
 * Retrieves quest statistics for user
 *
 * Parameters:
 *   user_id: User ID
 *   stats: Output statistics structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Quest_GetStatistics(unsigned int user_id, QuestStats* stats);

/**
 * Reset Daily Quests
 *
 * Resets daily quests for all users
 *
 * Returns: Number of quests reset
 */
int Quest_ResetDailyQuests();

/**
 * Reset Weekly Quests
 *
 * Resets weekly quests for all users
 *
 * Returns: Number of quests reset
 */
int Quest_ResetWeeklyQuests();

/**
 * Update Quest State
 *
 * Processes quest tick, checks expiration
 *
 * Parameters:
 *   delta_time_ms: Time elapsed since last update (milliseconds)
 *
 * Returns: Number of quests processed
 */
int Quest_Update(int delta_time_ms);

#endif // SRC_QUEST_QUEST_INTERFACE_H_
