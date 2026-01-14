/**
 * Achievement Module - Public Interface
 * Purpose: Achievement tracking, rewards, and progression system
 *
 * Components:
 * - Achievement definition and tracking
 * - Achievement progress monitoring
 * - Reward distribution
 * - Achievement points system
 * - Title unlocks
 * - Achievement categories
 */

#ifndef INCLUDE_ACHIEVEMENT_ACHIEVEMENT_INTERFACE_H_
#define INCLUDE_ACHIEVEMENT_ACHIEVEMENT_INTERFACE_H_

#include <common/defs.h>
#include <common/error_codes.h>

/**
 * Achievement categories
 */
typedef enum {
    ACHIEVE_CATEGORY_COMBAT = 0,      // Combat achievements
    ACHIEVE_CATEGORY_DUNGEON = 1,     // Dungeon achievements
    ACHIEVE_CATEGORY_PVP = 2,         // PVP achievements
    ACHIEVE_CATEGORY_QUEST = 3,       // Quest achievements
    ACHIEVE_CATEGORY_SOCIAL = 4,      // Social achievements
    ACHIEVE_CATEGORY_COLLECTION = 5,  // Collection achievements
    ACHIEVE_CATEGORY_ECONOMIC = 6,    // Economic achievements
    ACHIEVE_CATEGORY_SPECIAL = 7      // Special achievements
} AchievementCategory;

/**
 * Achievement states
 */
typedef enum {
    ACHIEVE_STATE_LOCKED = 0,         // Not started
    ACHIEVE_STATE_IN_PROGRESS = 1,    // In progress
    ACHIEVE_STATE_COMPLETED = 2,      // Completed
    ACHIEVE_STATE_CLAIMED = 3         // Reward claimed
} AchievementState;

/**
 * Achievement reward types
 */
typedef enum {
    REWARD_TYPE_GOLD = 0,
    REWARD_TYPE_ITEM = 1,
    REWARD_TYPE_TITLE = 2,
    REWARD_TYPE_POINTS = 3,
    REWARD_TYPE_EXPERIENCE = 4
} RewardType;

/**
 * Achievement reward
 */
typedef struct {
    RewardType reward_type;
    unsigned int reward_id;     // Item ID, Title ID, etc.
    int reward_amount;          // Amount/quantity
} AchievementReward;

/**
 * Achievement information
 */
typedef struct {
    unsigned int achievement_id;
    AchievementCategory category;
    AchievementState state;
    char name[128];
    char description[256];
    int current_progress;
    int required_progress;
    int achievement_points;
    int reward_count;
    AchievementReward rewards[5];  // Up to 5 rewards
    unsigned int complete_time;
    unsigned int claim_time;
    int is_hidden;              // Hidden until unlocked
    int is_repeatable;          // Can be completed multiple times
} AchievementInfo;

/**
 * Achievement statistics
 */
typedef struct {
    unsigned int total_achievements;
    unsigned int completed_achievements;
    unsigned int claimed_achievements;
    unsigned int total_achievement_points;
    unsigned int achievements_by_category[8];
    float completion_rate;
} AchievementStats;

/**
 * Initialize Achievement Module
 *
 * Sets up achievement system
 *
 * Parameters:
 *   max_achievements: Maximum achievement definitions
 *   max_user_achievements: Maximum achievements per user
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Achievement_Initialize(int max_achievements, int max_user_achievements);

/**
 * Cleanup Achievement Module
 *
 * Releases achievement system resources
 */
void Achievement_Cleanup();

/**
 * Define Achievement
 *
 * Defines a new achievement
 *
 * Parameters:
 *   achievement_id: Achievement ID
 *   category: Achievement category
 *   name: Achievement name
 *   description: Achievement description
 *   required_progress: Progress required to complete
 *   achievement_points: Points awarded
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Achievement_Define(unsigned int achievement_id,
                       AchievementCategory category,
                       const char* name,
                       const char* description,
                       int required_progress,
                       int achievement_points);

/**
 * Add Achievement Reward
 *
 * Adds a reward to an achievement
 *
 * Parameters:
 *   achievement_id: Achievement ID
 *   reward_type: Type of reward
 *   reward_id: Reward identifier (item ID, etc.)
 *   reward_amount: Reward amount
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Achievement_AddReward(unsigned int achievement_id,
                          RewardType reward_type,
                          unsigned int reward_id,
                          int reward_amount);

/**
 * Update Progress
 *
 * Updates achievement progress for a user
 *
 * Parameters:
 *   user_id: User ID
 *   achievement_id: Achievement ID
 *   progress: Progress amount to add
 *
 * Returns: New progress value, or negative error code
 */
int Achievement_UpdateProgress(unsigned int user_id,
                                unsigned int achievement_id,
                                int progress);

/**
 * Complete Achievement
 *
 * Marks achievement as completed
 *
 * Parameters:
 *   user_id: User ID
 *   achievement_id: Achievement ID
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Achievement_Complete(unsigned int user_id, unsigned int achievement_id);

/**
 * Claim Rewards
 *
 * Claims achievement rewards
 *
 * Parameters:
 *   user_id: User ID
 *   achievement_id: Achievement ID
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Achievement_ClaimRewards(unsigned int user_id, unsigned int achievement_id);

/**
 * Get Achievement Info
 *
 * Retrieves achievement information
 *
 * Parameters:
 *   achievement_id: Achievement ID
 *   achieve_info: Output achievement structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Achievement_GetInfo(unsigned int achievement_id,
                        AchievementInfo* achieve_info);

/**
 * Get User Achievements
 *
 * Retrieves user's achievements
 *
 * Parameters:
 *   user_id: User ID
 *   achievements: Output array of achievement info
 *   max_achievements: Maximum achievements to retrieve
 *   category: Category filter (-1 for all)
 *
 * Returns: Number of achievements retrieved
 */
int Achievement_GetUserAchievements(unsigned int user_id,
                                     AchievementInfo* achievements,
                                     int max_achievements,
                                     int category);

/**
 * Get Completed Achievements
 *
 * Retrieves user's completed achievements
 *
 * Parameters:
 *   user_id: User ID
 *   achievements: Output array of achievement info
 *   max_achievements: Maximum achievements to retrieve
 *
 * Returns: Number of completed achievements
 */
int Achievement_GetCompleted(unsigned int user_id,
                             AchievementInfo* achievements,
                             int max_achievements);

/**
 * Get Achievement Points
 *
 * Gets total achievement points for a user
 *
 * Parameters:
 *   user_id: User ID
 *
 * Returns: Total achievement points
 */
int Achievement_GetPoints(unsigned int user_id);

/**
 * Get Achievement Statistics
 *
 * Retrieves achievement statistics for a user
 *
 * Parameters:
 *   user_id: User ID
 *   stats: Output statistics structure
 *
 * Returns: ERR_SUCCESS on success, error code on failure
 */
int Achievement_GetStatistics(unsigned int user_id, AchievementStats* stats);

/**
 * Check Completion
 *
 * Checks if achievement is completed
 *
 * Parameters:
 *   user_id: User ID
 *   achievement_id: Achievement ID
 *
 * Returns: 1 if completed, 0 if not, negative on error
 */
int Achievement_IsCompleted(unsigned int user_id, unsigned int achievement_id);

/**
 * Update Achievement State
 *
 * Processes achievement tick
 *
 * Parameters:
 *   delta_time_ms: Time elapsed since last update (milliseconds)
 *
 * Returns: Number of achievements processed
 */
int Achievement_Update(int delta_time_ms);

#endif // INCLUDE_ACHIEVEMENT_ACHIEVEMENT_INTERFACE_H_
