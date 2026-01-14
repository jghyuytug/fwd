/**
 * Quest Module - Implementation (Main entry point)
 *
 * Handles quest/mission system, progress tracking, and rewards
 * Integrates all quest subsystems: config, manager, trigger, reward
 */

#include "quest_interface.h"
#include "../include/quest/quest_config.h"
#include "../include/quest/quest_manager.h"
#include "../include/quest/quest_trigger.h"
#include "../include/quest/quest_reward.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/* Quest module global state */
static struct {
    int initialized;
    int max_active_quests;

    // Statistics
    struct {
        unsigned int total_quests_accepted;
        unsigned int total_quests_completed;
        unsigned int total_quests_abandoned;
        unsigned int total_quests_failed;
        unsigned int total_rewards_claimed;
        unsigned int daily_quests_reset;
        unsigned int weekly_quests_reset;
    } stats;
} g_quest_state = {0};

/**
 * Initialize Quest Module
 */
int Quest_Initialize(int max_active_quests)
{
    if (g_quest_state.initialized) {
        fprintf(stderr, "[Quest] Module already initialized\n");
        return ERR_INVALID_STATE;
    }

    if (max_active_quests <= 0 || max_active_quests > 10000) {
        fprintf(stderr, "[Quest] Invalid max active quests: %d\n", max_active_quests);
        return ERR_INVALID_PARAMETER;
    }

    printf("[Quest] Initializing quest module...\n");

    g_quest_state.max_active_quests = max_active_quests;

    /* Initialize subsystems */
    if (QuestConfig_Initialize() != ERR_SUCCESS) {
        printf("[Quest] ERROR: Failed to initialize quest config\n");
        return ERR_INITIALIZATION_FAILED;
    }

    if (QuestManager_Initialize(NULL) != ERR_SUCCESS) {
        printf("[Quest] ERROR: Failed to initialize quest manager\n");
        QuestConfig_Cleanup();
        return ERR_INITIALIZATION_FAILED;
    }

    if (QuestTrigger_Initialize() != ERR_SUCCESS) {
        printf("[Quest] ERROR: Failed to initialize quest trigger\n");
        QuestManager_Cleanup();
        QuestConfig_Cleanup();
        return ERR_INITIALIZATION_FAILED;
    }

    if (QuestReward_Initialize() != ERR_SUCCESS) {
        printf("[Quest] ERROR: Failed to initialize quest reward\n");
        QuestTrigger_Cleanup();
        QuestManager_Cleanup();
        QuestConfig_Cleanup();
        return ERR_INITIALIZATION_FAILED;
    }

    g_quest_state.initialized = 1;

    printf("[Quest] Module initialized. Max active quests: %d\n", max_active_quests);
    printf("[Quest] All subsystems initialized: config, manager, trigger, reward\n");

    return ERR_SUCCESS;
}

/**
 * Cleanup Quest Module
 */
void Quest_Cleanup()
{
    if (!g_quest_state.initialized) {
        return;
    }

    printf("[Quest] Cleaning up quest module...\n");
    printf("[Quest] Statistics:\n");
    printf("  - Total Quests Accepted: %u\n", g_quest_state.stats.total_quests_accepted);
    printf("  - Total Quests Completed: %u\n", g_quest_state.stats.total_quests_completed);
    printf("  - Total Quests Abandoned: %u\n", g_quest_state.stats.total_quests_abandoned);
    printf("  - Total Quests Failed: %u\n", g_quest_state.stats.total_quests_failed);
    printf("  - Total Rewards Claimed: %u\n", g_quest_state.stats.total_rewards_claimed);
    printf("  - Daily Quests Reset: %u\n", g_quest_state.stats.daily_quests_reset);
    printf("  - Weekly Quests Reset: %u\n", g_quest_state.stats.weekly_quests_reset);

    /* Cleanup subsystems */
    QuestReward_Cleanup();
    QuestTrigger_Cleanup();
    QuestManager_Cleanup();
    QuestConfig_Cleanup();

    memset(&g_quest_state, 0, sizeof(g_quest_state));

    printf("[Quest] Module cleanup complete\n");
}

/**
 * Accept Quest
 */
int Quest_Accept(unsigned int user_id, unsigned int quest_template_id)
{
    if (!g_quest_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Call quest manager to accept quest */
    int result = QuestManager_Accept((int)user_id, quest_template_id, NULL);

    if (result > 0) {
        g_quest_state.stats.total_quests_accepted++;
    }

    return result;
}

/**
 * Abandon Quest
 */
int Quest_Abandon(unsigned int quest_id)
{
    if (!g_quest_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    int result = QuestManager_Abandon(quest_id);

    if (result == ERR_SUCCESS) {
        g_quest_state.stats.total_quests_abandoned++;
    }

    return result;
}

/**
 * Update Quest Progress
 */
int Quest_UpdateProgress(unsigned int user_id,
                         ObjectiveType objective_type,
                         unsigned int target_id,
                         int count)
{
    if (!g_quest_state.initialized) {
        return 0;
    }

    /* Call quest manager to update progress */
    return QuestManager_UpdateProgress((int)user_id, objective_type, (int)target_id, count);
}

/**
 * Complete Quest
 */
int Quest_Complete(unsigned int quest_id)
{
    if (!g_quest_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Check if quest is completed */
    if (!QuestManager_CheckCompletion(quest_id)) {
        return ERR_QUEST_NOT_COMPLETED;
    }

    g_quest_state.stats.total_quests_completed++;

    return ERR_SUCCESS;
}

/**
 * Claim Quest Rewards
 */
int Quest_ClaimRewards(unsigned int quest_id)
{
    if (!g_quest_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Submit quest and get reward */
    QuestReward reward;
    int result = QuestManager_Submit(quest_id, &reward);

    if (result == ERR_SUCCESS) {
        /* Give reward to character */
        QuestInstance instance;
        if (QuestManager_GetInstance(quest_id, &instance) == ERR_SUCCESS) {
            QuestReward_Give((int)instance.character_id, &reward);
        }

        g_quest_state.stats.total_rewards_claimed++;
    }

    return result;
}

/**
 * Get Quest Info
 */
int Quest_GetInfo(unsigned int quest_id, QuestInfo* quest_info)
{
    if (!g_quest_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!quest_info) {
        return ERR_INVALID_PARAMETER;
    }

    /* Get quest instance */
    QuestInstance instance;
    int result = QuestManager_GetInstance(quest_id, &instance);

    if (result != ERR_SUCCESS) {
        memset(quest_info, 0, sizeof(QuestInfo));
        return result;
    }

    /* Fill quest info */
    memset(quest_info, 0, sizeof(QuestInfo));
    quest_info->quest_id = instance.quest_instance_id;
    quest_info->quest_template_id = instance.quest_template_id;
    quest_info->quest_state = instance.state;
    quest_info->user_id = instance.character_id;
    quest_info->accept_time = instance.accept_time;
    quest_info->complete_time = instance.complete_time;
    quest_info->expire_time = instance.expire_time;
    quest_info->objective_count = instance.objective_count;

    for (int i = 0; i < instance.objective_count; i++) {
        quest_info->objectives[i].current_count = instance.objective_progress[i];
        quest_info->objectives[i].is_completed = instance.objective_completed[i];
    }

    return ERR_SUCCESS;
}

/**
 * Get User Quests
 */
int Quest_GetUserQuests(unsigned int user_id,
                        QuestInfo* quests,
                        int max_quests)
{
    if (!g_quest_state.initialized) {
        return 0;
    }

    if (!quests || max_quests <= 0) {
        return 0;
    }

    /* Get active quest instances */
    QuestInstance instances[50];
    int count = QuestManager_GetActiveQuests((int)user_id, instances,
                                             (max_quests < 50) ? max_quests : 50);

    /* Convert instances to QuestInfo */
    for (int i = 0; i < count; i++) {
        Quest_GetInfo(instances[i].quest_instance_id, &quests[i]);
    }

    return count;
}

/**
 * Get Available Quests
 */
int Quest_GetAvailableQuests(unsigned int user_id,
                             QuestInfo* quests,
                             int max_quests)
{
    if (!g_quest_state.initialized) {
        return 0;
    }

    if (!quests || max_quests <= 0) {
        return 0;
    }

    /* Load all available quest templates */
    QuestTemplate templates[100];
    int template_count = QuestConfig_LoadAllTemplates(templates, 100);

    /* Filter templates based on prerequisites */
    int available_count = 0;
    for (int i = 0; i < template_count && available_count < max_quests; i++) {
        if (QuestConfig_CheckPrerequisites((int)user_id, templates[i].quest_id)) {
            /* Fill quest info from template */
            memset(&quests[available_count], 0, sizeof(QuestInfo));
            quests[available_count].quest_template_id = templates[i].quest_id;
            quests[available_count].quest_type = templates[i].quest_type;
            quests[available_count].quest_state = QUEST_STATE_AVAILABLE;
            quests[available_count].min_level = templates[i].prerequisite.min_level;
            quests[available_count].max_level = templates[i].prerequisite.max_level;
            quests[available_count].objective_count = templates[i].objective_count;

            for (int j = 0; j < templates[i].objective_count; j++) {
                quests[available_count].objectives[j].objective_type = templates[i].objective_types[j];
                quests[available_count].objectives[j].target_id = templates[i].objective_targets[j];
                quests[available_count].objectives[j].required_count = templates[i].objective_counts[j];
            }

            available_count++;
        }
    }

    return available_count;
}

/**
 * Get Quest Statistics
 */
int Quest_GetStatistics(unsigned int user_id, QuestStats* stats)
{
    if (!g_quest_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!stats) {
        return ERR_INVALID_PARAMETER;
    }

    /*
     * Per-user quest statistics: Query from charac_quest table via ORM
     *
     * Production: Use CharacQuest_GetByCharacNo() and aggregate:
     * - COUNT(*) WHERE state = COMPLETED for total_quests_completed
     * - COUNT(*) WHERE state = FAILED for quests_failed
     * - SUM(reward_claimed) for total_rewards_claimed
     *
     * For now, return global statistics as placeholder.
     */
    (void)user_id;

    memset(stats, 0, sizeof(QuestStats));
    stats->total_quests_completed = g_quest_state.stats.total_quests_completed;
    stats->quests_failed = g_quest_state.stats.total_quests_failed;
    stats->total_rewards_claimed = g_quest_state.stats.total_rewards_claimed;

    return ERR_SUCCESS;
}

/**
 * Reset Daily Quests (Stub)
 */
int Quest_ResetDailyQuests()
{
    if (!g_quest_state.initialized) {
        return 0;
    }

    g_quest_state.stats.daily_quests_reset++;

    return 0;
}

/**
 * Reset Weekly Quests (Stub)
 */
int Quest_ResetWeeklyQuests()
{
    if (!g_quest_state.initialized) {
        return 0;
    }

    g_quest_state.stats.weekly_quests_reset++;

    return 0;
}

/**
 * Update Quest State (Stub)
 */
int Quest_Update(int delta_time_ms)
{
    if (!g_quest_state.initialized) {
        return 0;
    }

    (void)delta_time_ms;

    return 0;
}
