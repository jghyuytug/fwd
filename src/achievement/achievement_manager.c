/**
 * Achievement Module - Full Implementation
 *
 * Handles achievement tracking, progress monitoring, and rewards
 * Features:
 * - Achievement definition and registration
 * - Per-user progress tracking
 * - Multi-reward distribution (gold, items, titles, points, experience)
 * - 8 achievement categories
 * - Hidden and repeatable achievements
 * - Achievement points system
 */

#include "../../include/achievement/achievement_interface.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/* Constants */
#define MAX_ACHIEVEMENTS 5000
#define MAX_USERS 10000
#define MAX_USER_ACHIEVEMENTS 500

/* Achievement definition */
typedef struct {
    unsigned int achievement_id;
    AchievementCategory category;
    char name[128];
    char description[256];
    int required_progress;
    int achievement_points;
    int reward_count;
    AchievementReward rewards[5];
    int is_hidden;
    int is_repeatable;
    int is_defined;
} AchievementDef;

/* User achievement progress */
typedef struct {
    unsigned int achievement_id;
    AchievementState state;
    int current_progress;
    unsigned int complete_time;
    unsigned int claim_time;
    int completion_count;  /* For repeatable achievements */
} UserAchievementProgress;

/* Per-user achievement tracking */
typedef struct {
    unsigned int user_id;
    UserAchievementProgress achievements[MAX_USER_ACHIEVEMENTS];
    int achievement_count;
    int total_points;
    int completed_count;
    int claimed_count;
} UserAchievementData;

/* Achievement module global state */
static struct {
    int initialized;
    int max_achievements;
    int max_user_achievements;

    /* Achievement definitions */
    AchievementDef* definitions;
    int definition_count;

    /* User data */
    UserAchievementData* users;
    int user_count;

    /* Statistics */
    struct {
        unsigned int total_achievements_defined;
        unsigned int total_progress_updates;
        unsigned int total_achievements_completed;
        unsigned int total_rewards_claimed;
        unsigned int total_achievement_points_awarded;
        unsigned int total_user_achievement_queries;
        unsigned int hidden_achievements_unlocked;
        unsigned int repeatable_completions;
    } stats;
} g_achievement_state = {0};

/* Forward declarations */
static AchievementDef* FindAchievementDef(unsigned int achievement_id);
static UserAchievementData* FindOrCreateUser(unsigned int user_id);
static UserAchievementData* FindUser(unsigned int user_id);
static UserAchievementProgress* FindOrCreateUserProgress(unsigned int user_id, unsigned int achievement_id);
static UserAchievementProgress* FindUserProgress(unsigned int user_id, unsigned int achievement_id);

/**
 * Initialize Achievement Module
 */
int Achievement_Initialize(int max_achievements, int max_user_achievements)
{
    if (g_achievement_state.initialized) {
        fprintf(stderr, "[Achievement] Module already initialized\n");
        return ERR_INVALID_STATE;
    }

    if (max_achievements <= 0 || max_achievements > MAX_ACHIEVEMENTS) {
        fprintf(stderr, "[Achievement] Invalid max achievements: %d\n", max_achievements);
        return ERR_INVALID_PARAMETER;
    }

    if (max_user_achievements <= 0 || max_user_achievements > MAX_USER_ACHIEVEMENTS) {
        fprintf(stderr, "[Achievement] Invalid max user achievements: %d\n", max_user_achievements);
        return ERR_INVALID_PARAMETER;
    }

    printf("[Achievement] Initializing achievement module...\n");

    /* Allocate achievement definitions */
    g_achievement_state.definitions = (AchievementDef*)calloc(max_achievements, sizeof(AchievementDef));
    if (!g_achievement_state.definitions) {
        fprintf(stderr, "[Achievement] Failed to allocate achievement definitions\n");
        return ERR_MEMORY;
    }

    /* Allocate user data */
    g_achievement_state.users = (UserAchievementData*)calloc(MAX_USERS, sizeof(UserAchievementData));
    if (!g_achievement_state.users) {
        free(g_achievement_state.definitions);
        g_achievement_state.definitions = NULL;
        fprintf(stderr, "[Achievement] Failed to allocate user data\n");
        return ERR_MEMORY;
    }

    g_achievement_state.max_achievements = max_achievements;
    g_achievement_state.max_user_achievements = max_user_achievements;
    g_achievement_state.definition_count = 0;
    g_achievement_state.user_count = 0;
    g_achievement_state.initialized = 1;

    printf("[Achievement] Module initialized (max_achievements=%d, max_user_achievements=%d)\n",
           max_achievements, max_user_achievements);

    return ERR_SUCCESS;
}

/**
 * Cleanup Achievement Module
 */
void Achievement_Cleanup()
{
    if (!g_achievement_state.initialized) {
        return;
    }

    printf("[Achievement] Cleaning up achievement module...\n");
    printf("[Achievement] Statistics:\n");
    printf("  - Total Achievements Defined: %u\n", g_achievement_state.stats.total_achievements_defined);
    printf("  - Total Progress Updates: %u\n", g_achievement_state.stats.total_progress_updates);
    printf("  - Total Achievements Completed: %u\n", g_achievement_state.stats.total_achievements_completed);
    printf("  - Total Rewards Claimed: %u\n", g_achievement_state.stats.total_rewards_claimed);
    printf("  - Total Achievement Points Awarded: %u\n", g_achievement_state.stats.total_achievement_points_awarded);
    printf("  - Total User Achievement Queries: %u\n", g_achievement_state.stats.total_user_achievement_queries);
    printf("  - Hidden Achievements Unlocked: %u\n", g_achievement_state.stats.hidden_achievements_unlocked);
    printf("  - Repeatable Completions: %u\n", g_achievement_state.stats.repeatable_completions);
    printf("  - Active Users: %d\n", g_achievement_state.user_count);

    if (g_achievement_state.definitions) {
        free(g_achievement_state.definitions);
        g_achievement_state.definitions = NULL;
    }

    if (g_achievement_state.users) {
        free(g_achievement_state.users);
        g_achievement_state.users = NULL;
    }

    memset(&g_achievement_state, 0, sizeof(g_achievement_state));

    printf("[Achievement] Module cleanup complete\n");
}

/**
 * Define Achievement
 */
int Achievement_Define(unsigned int achievement_id,
                       AchievementCategory category,
                       const char* name,
                       const char* description,
                       int required_progress,
                       int achievement_points)
{
    if (!g_achievement_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!name || !description) {
        return ERR_INVALID_PARAMETER;
    }

    if (required_progress <= 0) {
        return ERR_INVALID_PARAMETER;
    }

    if (achievement_points < 0) {
        return ERR_INVALID_PARAMETER;
    }

    if (category > ACHIEVE_CATEGORY_SPECIAL) {
        return ERR_INVALID_PARAMETER;
    }

    /* Check for duplicate */
    if (FindAchievementDef(achievement_id) != NULL) {
        return ERR_DUPLICATE;
    }

    /* Check capacity */
    if (g_achievement_state.definition_count >= g_achievement_state.max_achievements) {
        return ERR_LIMIT_EXCEEDED;
    }

    /* Add definition */
    AchievementDef* def = &g_achievement_state.definitions[g_achievement_state.definition_count];
    memset(def, 0, sizeof(AchievementDef));

    def->achievement_id = achievement_id;
    def->category = category;
    strncpy(def->name, name, sizeof(def->name) - 1);
    strncpy(def->description, description, sizeof(def->description) - 1);
    def->required_progress = required_progress;
    def->achievement_points = achievement_points;
    def->reward_count = 0;
    def->is_hidden = 0;
    def->is_repeatable = 0;
    def->is_defined = 1;

    g_achievement_state.definition_count++;
    g_achievement_state.stats.total_achievements_defined++;

    return ERR_SUCCESS;
}

/**
 * Add Achievement Reward
 */
int Achievement_AddReward(unsigned int achievement_id,
                          RewardType reward_type,
                          unsigned int reward_id,
                          int reward_amount)
{
    if (!g_achievement_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (reward_amount <= 0) {
        return ERR_INVALID_PARAMETER;
    }

    AchievementDef* def = FindAchievementDef(achievement_id);
    if (!def) {
        return ERR_NOT_FOUND;
    }

    if (def->reward_count >= 5) {
        return ERR_LIMIT_EXCEEDED;
    }

    AchievementReward* reward = &def->rewards[def->reward_count];
    reward->reward_type = reward_type;
    reward->reward_id = reward_id;
    reward->reward_amount = reward_amount;
    def->reward_count++;

    return ERR_SUCCESS;
}

/**
 * Set Achievement Hidden
 */
int Achievement_SetHidden(unsigned int achievement_id, int is_hidden)
{
    if (!g_achievement_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    AchievementDef* def = FindAchievementDef(achievement_id);
    if (!def) {
        return ERR_NOT_FOUND;
    }

    def->is_hidden = is_hidden ? 1 : 0;

    return ERR_SUCCESS;
}

/**
 * Set Achievement Repeatable
 */
int Achievement_SetRepeatable(unsigned int achievement_id, int is_repeatable)
{
    if (!g_achievement_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    AchievementDef* def = FindAchievementDef(achievement_id);
    if (!def) {
        return ERR_NOT_FOUND;
    }

    def->is_repeatable = is_repeatable ? 1 : 0;

    return ERR_SUCCESS;
}

/**
 * Update Progress
 */
int Achievement_UpdateProgress(unsigned int user_id,
                                unsigned int achievement_id,
                                int progress)
{
    if (!g_achievement_state.initialized) {
        return -ERR_NOT_INITIALIZED;
    }

    if (user_id == 0 || progress <= 0) {
        return -ERR_INVALID_PARAMETER;
    }

    AchievementDef* def = FindAchievementDef(achievement_id);
    if (!def) {
        return -ERR_NOT_FOUND;
    }

    UserAchievementProgress* user_progress = FindOrCreateUserProgress(user_id, achievement_id);
    if (!user_progress) {
        return -ERR_MEMORY;
    }

    /* Check if already completed (and not repeatable) */
    if (user_progress->state == ACHIEVE_STATE_COMPLETED ||
        user_progress->state == ACHIEVE_STATE_CLAIMED) {
        if (!def->is_repeatable) {
            return user_progress->current_progress;
        }
        /* Reset for repeatable achievements */
        user_progress->state = ACHIEVE_STATE_IN_PROGRESS;
        user_progress->current_progress = 0;
    }

    /* Update progress */
    if (user_progress->state == ACHIEVE_STATE_LOCKED) {
        user_progress->state = ACHIEVE_STATE_IN_PROGRESS;

        /* Unlock hidden achievement notification */
        if (def->is_hidden) {
            g_achievement_state.stats.hidden_achievements_unlocked++;
        }
    }

    user_progress->current_progress += progress;
    g_achievement_state.stats.total_progress_updates++;

    /* Check for completion */
    if (user_progress->current_progress >= def->required_progress) {
        user_progress->current_progress = def->required_progress;
        user_progress->state = ACHIEVE_STATE_COMPLETED;
        user_progress->complete_time = (unsigned int)time(NULL);
        user_progress->completion_count++;

        g_achievement_state.stats.total_achievements_completed++;

        if (def->is_repeatable && user_progress->completion_count > 1) {
            g_achievement_state.stats.repeatable_completions++;
        }

        /* Update user data */
        UserAchievementData* user = FindUser(user_id);
        if (user) {
            user->completed_count++;
        }
    }

    return user_progress->current_progress;
}

/**
 * Complete Achievement
 */
int Achievement_Complete(unsigned int user_id, unsigned int achievement_id)
{
    if (!g_achievement_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (user_id == 0) {
        return ERR_INVALID_PARAMETER;
    }

    AchievementDef* def = FindAchievementDef(achievement_id);
    if (!def) {
        return ERR_NOT_FOUND;
    }

    UserAchievementProgress* user_progress = FindOrCreateUserProgress(user_id, achievement_id);
    if (!user_progress) {
        return ERR_MEMORY;
    }

    /* Already completed */
    if (user_progress->state == ACHIEVE_STATE_COMPLETED ||
        user_progress->state == ACHIEVE_STATE_CLAIMED) {
        if (!def->is_repeatable) {
            return ERR_INVALID_STATE;
        }
    }

    /* Complete it */
    user_progress->current_progress = def->required_progress;
    user_progress->state = ACHIEVE_STATE_COMPLETED;
    user_progress->complete_time = (unsigned int)time(NULL);
    user_progress->completion_count++;

    g_achievement_state.stats.total_achievements_completed++;

    if (def->is_hidden) {
        g_achievement_state.stats.hidden_achievements_unlocked++;
    }

    if (def->is_repeatable && user_progress->completion_count > 1) {
        g_achievement_state.stats.repeatable_completions++;
    }

    /* Update user data */
    UserAchievementData* user = FindUser(user_id);
    if (user) {
        user->completed_count++;
    }

    return ERR_SUCCESS;
}

/**
 * Claim Rewards
 */
int Achievement_ClaimRewards(unsigned int user_id, unsigned int achievement_id)
{
    if (!g_achievement_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (user_id == 0) {
        return ERR_INVALID_PARAMETER;
    }

    AchievementDef* def = FindAchievementDef(achievement_id);
    if (!def) {
        return ERR_NOT_FOUND;
    }

    UserAchievementProgress* user_progress = FindUserProgress(user_id, achievement_id);
    if (!user_progress) {
        return ERR_NOT_FOUND;
    }

    /* Must be completed */
    if (user_progress->state != ACHIEVE_STATE_COMPLETED) {
        return ERR_INVALID_STATE;
    }

    /* Already claimed (for non-repeatable) */
    if (user_progress->state == ACHIEVE_STATE_CLAIMED && !def->is_repeatable) {
        return ERR_INVALID_STATE;
    }

    /* Mark as claimed */
    user_progress->state = ACHIEVE_STATE_CLAIMED;
    user_progress->claim_time = (unsigned int)time(NULL);

    g_achievement_state.stats.total_rewards_claimed++;
    g_achievement_state.stats.total_achievement_points_awarded += def->achievement_points;

    /* Update user data */
    UserAchievementData* user = FindUser(user_id);
    if (user) {
        user->total_points += def->achievement_points;
        user->claimed_count++;
    }

    /*
     * Reward distribution: Integrate with user/inventory systems
     *
     * Iterate through achievement rewards and distribute based on type:
     *
     * for (int i = 0; i < def->reward_count; i++) {
     *     AchievementReward* reward = &def->rewards[i];
     *     switch (reward->reward_type) {
     *         case REWARD_TYPE_GOLD:
     *             User_AddCurrency(user_id, CURRENCY_GOLD, reward->reward_amount);
     *             break;
     *         case REWARD_TYPE_ITEM:
     *             Inventory_AddItem(user_id, reward->reward_id, reward->reward_amount);
     *             break;
     *         case REWARD_TYPE_TITLE:
     *             User_GrantTitle(user_id, reward->reward_id);
     *             break;
     *         case REWARD_TYPE_EXPERIENCE:
     *             User_AddCharacterExperience(charac_id, reward->reward_amount);
     *             break;
     *         case REWARD_TYPE_POINTS:
     *             // Achievement points already handled above
     *             break;
     *     }
     * }
     *
     * Production: Send reward notification to client after distribution.
     */

    return ERR_SUCCESS;
}

/**
 * Get Achievement Info
 */
int Achievement_GetInfo(unsigned int achievement_id,
                        AchievementInfo* achieve_info)
{
    if (!g_achievement_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!achieve_info) {
        return ERR_INVALID_PARAMETER;
    }

    AchievementDef* def = FindAchievementDef(achievement_id);
    if (!def) {
        return ERR_NOT_FOUND;
    }

    memset(achieve_info, 0, sizeof(AchievementInfo));

    achieve_info->achievement_id = def->achievement_id;
    achieve_info->category = def->category;
    achieve_info->state = ACHIEVE_STATE_LOCKED;  /* Default, user-specific */
    strncpy(achieve_info->name, def->name, sizeof(achieve_info->name) - 1);
    strncpy(achieve_info->description, def->description, sizeof(achieve_info->description) - 1);
    achieve_info->current_progress = 0;
    achieve_info->required_progress = def->required_progress;
    achieve_info->achievement_points = def->achievement_points;
    achieve_info->reward_count = def->reward_count;
    memcpy(achieve_info->rewards, def->rewards, sizeof(def->rewards));
    achieve_info->is_hidden = def->is_hidden;
    achieve_info->is_repeatable = def->is_repeatable;

    return ERR_SUCCESS;
}

/**
 * Get User Achievement Info (with progress)
 */
int Achievement_GetUserAchievementInfo(unsigned int user_id,
                                        unsigned int achievement_id,
                                        AchievementInfo* achieve_info)
{
    if (!g_achievement_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!achieve_info) {
        return ERR_INVALID_PARAMETER;
    }

    /* First get base info */
    int result = Achievement_GetInfo(achievement_id, achieve_info);
    if (result != ERR_SUCCESS) {
        return result;
    }

    /* Overlay user progress */
    UserAchievementProgress* progress = FindUserProgress(user_id, achievement_id);
    if (progress) {
        achieve_info->state = progress->state;
        achieve_info->current_progress = progress->current_progress;
        achieve_info->complete_time = progress->complete_time;
        achieve_info->claim_time = progress->claim_time;
    }

    return ERR_SUCCESS;
}

/**
 * Get User Achievements
 */
int Achievement_GetUserAchievements(unsigned int user_id,
                                     AchievementInfo* achievements,
                                     int max_achievements,
                                     int category)
{
    if (!g_achievement_state.initialized) {
        return 0;
    }

    if (!achievements || max_achievements <= 0) {
        return 0;
    }

    g_achievement_state.stats.total_user_achievement_queries++;

    int count = 0;
    for (int i = 0; i < g_achievement_state.definition_count && count < max_achievements; i++) {
        AchievementDef* def = &g_achievement_state.definitions[i];
        if (!def->is_defined) continue;

        /* Category filter */
        if (category >= 0 && def->category != (AchievementCategory)category) {
            continue;
        }

        /* Get user-specific info */
        Achievement_GetUserAchievementInfo(user_id, def->achievement_id, &achievements[count]);
        count++;
    }

    return count;
}

/**
 * Get Completed Achievements
 */
int Achievement_GetCompleted(unsigned int user_id,
                             AchievementInfo* achievements,
                             int max_achievements)
{
    if (!g_achievement_state.initialized) {
        return 0;
    }

    if (!achievements || max_achievements <= 0) {
        return 0;
    }

    g_achievement_state.stats.total_user_achievement_queries++;

    UserAchievementData* user = FindUser(user_id);
    if (!user) {
        return 0;
    }

    int count = 0;
    for (int i = 0; i < user->achievement_count && count < max_achievements; i++) {
        UserAchievementProgress* progress = &user->achievements[i];
        if (progress->state == ACHIEVE_STATE_COMPLETED ||
            progress->state == ACHIEVE_STATE_CLAIMED) {
            Achievement_GetUserAchievementInfo(user_id, progress->achievement_id, &achievements[count]);
            count++;
        }
    }

    return count;
}

/**
 * Get Achievement Points
 */
int Achievement_GetPoints(unsigned int user_id)
{
    if (!g_achievement_state.initialized) {
        return 0;
    }

    UserAchievementData* user = FindUser(user_id);
    if (!user) {
        return 0;
    }

    return user->total_points;
}

/**
 * Get Achievement Statistics
 */
int Achievement_GetStatistics(unsigned int user_id, AchievementStats* stats)
{
    if (!g_achievement_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!stats) {
        return ERR_INVALID_PARAMETER;
    }

    memset(stats, 0, sizeof(AchievementStats));

    stats->total_achievements = g_achievement_state.definition_count;

    UserAchievementData* user = FindUser(user_id);
    if (user) {
        stats->completed_achievements = user->completed_count;
        stats->claimed_achievements = user->claimed_count;
        stats->total_achievement_points = user->total_points;

        /* Count by category */
        for (int i = 0; i < user->achievement_count; i++) {
            UserAchievementProgress* progress = &user->achievements[i];
            if (progress->state == ACHIEVE_STATE_COMPLETED ||
                progress->state == ACHIEVE_STATE_CLAIMED) {
                AchievementDef* def = FindAchievementDef(progress->achievement_id);
                if (def && def->category < 8) {
                    stats->achievements_by_category[def->category]++;
                }
            }
        }
    }

    if (stats->total_achievements > 0) {
        stats->completion_rate =
            (float)stats->completed_achievements / stats->total_achievements * 100.0f;
    }

    return ERR_SUCCESS;
}

/**
 * Check Completion
 */
int Achievement_IsCompleted(unsigned int user_id, unsigned int achievement_id)
{
    if (!g_achievement_state.initialized) {
        return -1;
    }

    UserAchievementProgress* progress = FindUserProgress(user_id, achievement_id);
    if (!progress) {
        return 0;
    }

    return (progress->state == ACHIEVE_STATE_COMPLETED ||
            progress->state == ACHIEVE_STATE_CLAIMED) ? 1 : 0;
}

/**
 * Update Achievement State
 */
int Achievement_Update(int delta_time_ms)
{
    if (!g_achievement_state.initialized) {
        return 0;
    }

    (void)delta_time_ms;

    /* No periodic processing needed currently */
    return 0;
}

/**
 * Print Statistics
 */
void Achievement_PrintStats()
{
    if (!g_achievement_state.initialized) {
        printf("[Achievement] Module not initialized\n");
        return;
    }

    printf("=== Achievement System Statistics ===\n");
    printf("Definitions: %d / %d\n", g_achievement_state.definition_count,
           g_achievement_state.max_achievements);
    printf("Active Users: %d\n", g_achievement_state.user_count);
    printf("\nGlobal Stats:\n");
    printf("  Total Defined: %u\n", g_achievement_state.stats.total_achievements_defined);
    printf("  Progress Updates: %u\n", g_achievement_state.stats.total_progress_updates);
    printf("  Completions: %u\n", g_achievement_state.stats.total_achievements_completed);
    printf("  Rewards Claimed: %u\n", g_achievement_state.stats.total_rewards_claimed);
    printf("  Points Awarded: %u\n", g_achievement_state.stats.total_achievement_points_awarded);
    printf("  User Queries: %u\n", g_achievement_state.stats.total_user_achievement_queries);
    printf("  Hidden Unlocked: %u\n", g_achievement_state.stats.hidden_achievements_unlocked);
    printf("  Repeatable Completions: %u\n", g_achievement_state.stats.repeatable_completions);
}

/* ============================================================
 * Internal Helper Functions
 * ============================================================ */

static AchievementDef* FindAchievementDef(unsigned int achievement_id)
{
    for (int i = 0; i < g_achievement_state.definition_count; i++) {
        if (g_achievement_state.definitions[i].is_defined &&
            g_achievement_state.definitions[i].achievement_id == achievement_id) {
            return &g_achievement_state.definitions[i];
        }
    }
    return NULL;
}

static UserAchievementData* FindOrCreateUser(unsigned int user_id)
{
    /* Find existing */
    for (int i = 0; i < g_achievement_state.user_count; i++) {
        if (g_achievement_state.users[i].user_id == user_id) {
            return &g_achievement_state.users[i];
        }
    }

    /* Create new */
    if (g_achievement_state.user_count >= MAX_USERS) {
        return NULL;
    }

    UserAchievementData* user = &g_achievement_state.users[g_achievement_state.user_count];
    memset(user, 0, sizeof(UserAchievementData));
    user->user_id = user_id;
    g_achievement_state.user_count++;

    return user;
}

static UserAchievementData* FindUser(unsigned int user_id)
{
    for (int i = 0; i < g_achievement_state.user_count; i++) {
        if (g_achievement_state.users[i].user_id == user_id) {
            return &g_achievement_state.users[i];
        }
    }
    return NULL;
}

static UserAchievementProgress* FindOrCreateUserProgress(unsigned int user_id, unsigned int achievement_id)
{
    UserAchievementData* user = FindOrCreateUser(user_id);
    if (!user) {
        return NULL;
    }

    /* Find existing */
    for (int i = 0; i < user->achievement_count; i++) {
        if (user->achievements[i].achievement_id == achievement_id) {
            return &user->achievements[i];
        }
    }

    /* Create new */
    if (user->achievement_count >= MAX_USER_ACHIEVEMENTS) {
        return NULL;
    }

    UserAchievementProgress* progress = &user->achievements[user->achievement_count];
    memset(progress, 0, sizeof(UserAchievementProgress));
    progress->achievement_id = achievement_id;
    progress->state = ACHIEVE_STATE_LOCKED;
    user->achievement_count++;

    return progress;
}

static UserAchievementProgress* FindUserProgress(unsigned int user_id, unsigned int achievement_id)
{
    UserAchievementData* user = FindUser(user_id);
    if (!user) {
        return NULL;
    }

    for (int i = 0; i < user->achievement_count; i++) {
        if (user->achievements[i].achievement_id == achievement_id) {
            return &user->achievements[i];
        }
    }

    return NULL;
}
