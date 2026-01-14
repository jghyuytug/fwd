/**
 * Quest Manager Module - Implementation
 * Purpose: Manage quest state (accept, update, complete, submit, abandon)
 *
 * Phase: 6.2 - Quest System Implementation
 * Created: 2025-11-23
 * Updated: 2025-11-24 - Configuration abstraction (Stage 1)
 */

#include "../../include/quest/quest_manager.h"
#include "../../include/quest/quest_config.h"
#include "../../include/config/game_config.h"
#include "../orm/core_game/taiwan_cain/headers/new_charac_quest_orm.h"
#include "../orm/core_game/taiwan_cain/headers/charac_info_orm.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/* ✅ UPDATED (2025-11-24): Removed hardcoded MAX_ACTIVE_QUESTS_PER_CHARACTER
 * Now using QuestConfig_GetLimits()->max_active_quests_per_character */
#define MAX_QUEST_INSTANCES 10000

/* Global state */
static struct {
    int initialized;
    DBConnectionManager* db_manager;

    /* In-memory quest instance storage */
    QuestInstance* instances;
    int instance_count;
    unsigned int next_instance_id;

    /* Statistics */
    struct {
        unsigned int quests_accepted;
        unsigned int quests_completed;
        unsigned int quests_abandoned;
        unsigned int progress_updates;
    } stats;
} g_manager_state = {0};

/**
 * Initialize quest manager
 */
int QuestManager_Initialize(DBConnectionManager* db_manager)
{
    if (g_manager_state.initialized) {
        printf("[QuestManager] Already initialized\n");
        return ERR_INVALID_STATE;
    }

    printf("[QuestManager] Initializing quest manager...\n");

    g_manager_state.db_manager = db_manager;

    /* Allocate instance storage */
    g_manager_state.instances = (QuestInstance*)malloc(
        sizeof(QuestInstance) * MAX_QUEST_INSTANCES
    );

    if (!g_manager_state.instances) {
        printf("[QuestManager] ERROR: Failed to allocate instance storage\n");
        return ERR_MEMORY_ALLOCATION_FAILED;
    }

    g_manager_state.instance_count = 0;
    g_manager_state.next_instance_id = 1;
    g_manager_state.initialized = 1;

    printf("[QuestManager] Quest manager initialized (max %d instances)\n",
           MAX_QUEST_INSTANCES);

    return ERR_SUCCESS;
}

/**
 * Set database manager
 */
void QuestManager_SetDatabaseManager(DBConnectionManager* db_manager)
{
    g_manager_state.db_manager = db_manager;
    if (db_manager) {
        printf("[QuestManager] Database manager configured\n");
    }
}

/**
 * Find instance index by ID
 */
static int FindInstanceIndex(unsigned int quest_instance_id)
{
    for (int i = 0; i < g_manager_state.instance_count; i++) {
        if (g_manager_state.instances[i].quest_instance_id == quest_instance_id) {
            return i;
        }
    }
    return -1;
}

/**
 * Accept a quest
 */
int QuestManager_Accept(int character_id, unsigned int quest_template_id, QuestInstance* out_instance)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Check if instance storage is full */
    if (g_manager_state.instance_count >= MAX_QUEST_INSTANCES) {
        printf("[QuestManager] ERROR: Instance storage full\n");
        return ERR_QUEST_STORAGE_FULL;
    }

    /* Load quest template */
    QuestTemplate template;
    if (QuestConfig_LoadTemplate(quest_template_id, &template) != ERR_SUCCESS) {
        printf("[QuestManager] ERROR: Quest template %u not found\n", quest_template_id);
        return ERR_QUEST_NOT_FOUND;
    }

    /* Check prerequisites */
    if (!QuestConfig_CheckPrerequisites(character_id, quest_template_id)) {
        printf("[QuestManager] Prerequisites not met for quest %u\n", quest_template_id);
        return ERR_QUEST_PREREQUISITES_NOT_MET;
    }

    /* Count active quests for this character */
    int active_count = 0;
    for (int i = 0; i < g_manager_state.instance_count; i++) {
        if (g_manager_state.instances[i].character_id == (unsigned int)character_id &&
            g_manager_state.instances[i].state != QUEST_STATE_CLAIMED) {
            active_count++;
        }
    }

    /* ✅ UPDATED (2025-11-24): Use configuration instead of hardcoded limit */
    const QuestLimitsConfig* limits = GameConfig_GetQuestLimits();
    if (active_count >= limits->max_active_quests_per_character) {
        printf("[QuestManager] Character %d has too many active quests (%d, max=%d)\n",
               character_id, active_count, limits->max_active_quests_per_character);
        return ERR_QUEST_LIMIT_REACHED;
    }

    /* Create new quest instance */
    QuestInstance* instance = &g_manager_state.instances[g_manager_state.instance_count];
    memset(instance, 0, sizeof(QuestInstance));

    instance->quest_instance_id = g_manager_state.next_instance_id++;
    instance->character_id = character_id;
    instance->quest_template_id = quest_template_id;
    instance->state = QUEST_STATE_ACCEPTED;
    instance->accept_time = (unsigned int)time(NULL);
    instance->complete_time = 0;

    /* Set expiration time if quest has duration */
    if (template.duration_minutes > 0) {
        instance->expire_time = instance->accept_time + (template.duration_minutes * 60);
    } else {
        instance->expire_time = 0;
    }

    /* Initialize objective progress */
    instance->objective_count = template.objective_count;
    for (int i = 0; i < instance->objective_count; i++) {
        instance->objective_progress[i] = 0;
        instance->objective_completed[i] = 0;
    }

    g_manager_state.instance_count++;
    g_manager_state.stats.quests_accepted++;

    printf("[QuestManager] Quest %u accepted by character %d (instance ID: %u)\n",
           quest_template_id, character_id, instance->quest_instance_id);

    /* Return instance info */
    if (out_instance) {
        memcpy(out_instance, instance, sizeof(QuestInstance));
    }

    return (int)instance->quest_instance_id;
}

/**
 * Update quest progress
 */
int QuestManager_UpdateProgress(int character_id, ObjectiveType objective_type,
                                 int target_id, int increment)
{
    if (!g_manager_state.initialized) {
        return 0;
    }

    int quests_updated = 0;

    /* Find all active quests for this character */
    for (int i = 0; i < g_manager_state.instance_count; i++) {
        QuestInstance* instance = &g_manager_state.instances[i];

        /* Skip if not this character or not in progress */
        if (instance->character_id != (unsigned int)character_id ||
            instance->state != QUEST_STATE_ACCEPTED) {
            continue;
        }

        /* Load quest template */
        QuestTemplate template;
        if (QuestConfig_LoadTemplate(instance->quest_template_id, &template) != ERR_SUCCESS) {
            continue;
        }

        /* Check each objective */
        int updated = 0;
        for (int j = 0; j < instance->objective_count; j++) {
            /* Skip if already completed */
            if (instance->objective_completed[j]) {
                continue;
            }

            /* Check if objective matches */
            if (template.objective_types[j] == objective_type &&
                template.objective_targets[j] == target_id) {

                /* Update progress */
                instance->objective_progress[j] += increment;

                /* Clamp to required count */
                if (instance->objective_progress[j] > template.objective_counts[j]) {
                    instance->objective_progress[j] = template.objective_counts[j];
                }

                /* Check if objective completed */
                if (instance->objective_progress[j] >= template.objective_counts[j]) {
                    instance->objective_completed[j] = 1;
                    printf("[QuestManager] Objective %d completed for quest instance %u\n",
                           j, instance->quest_instance_id);
                }

                updated = 1;
            }
        }

        if (updated) {
            quests_updated++;
            g_manager_state.stats.progress_updates++;

            printf("[QuestManager] Progress updated for quest instance %u (type=%d, target=%d, +%d)\n",
                   instance->quest_instance_id, objective_type, target_id, increment);
        }
    }

    return quests_updated;
}

/**
 * Check if quest is completed
 */
int QuestManager_CheckCompletion(unsigned int quest_instance_id)
{
    if (!g_manager_state.initialized) {
        return 0;
    }

    /* Find instance */
    int index = FindInstanceIndex(quest_instance_id);
    if (index < 0) {
        return 0;
    }

    QuestInstance* instance = &g_manager_state.instances[index];

    /* Check all objectives */
    for (int i = 0; i < instance->objective_count; i++) {
        if (!instance->objective_completed[i]) {
            return 0;  /* Not all objectives completed */
        }
    }

    /* All objectives completed */
    if (instance->state == QUEST_STATE_ACCEPTED) {
        instance->state = QUEST_STATE_COMPLETED;
        instance->complete_time = (unsigned int)time(NULL);
        printf("[QuestManager] Quest instance %u completed\n", quest_instance_id);
    }

    return 1;
}

/**
 * Submit (turn in) a completed quest
 */
int QuestManager_Submit(unsigned int quest_instance_id, QuestReward* out_reward)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Find instance */
    int index = FindInstanceIndex(quest_instance_id);
    if (index < 0) {
        return ERR_QUEST_NOT_FOUND;
    }

    QuestInstance* instance = &g_manager_state.instances[index];

    /* Check if quest is completed */
    if (instance->state != QUEST_STATE_COMPLETED) {
        printf("[QuestManager] Quest instance %u is not completed (state=%d)\n",
               quest_instance_id, instance->state);
        return ERR_QUEST_NOT_COMPLETED;
    }

    /* Get base reward from template */
    QuestReward reward;
    if (QuestConfig_GetReward(instance->quest_template_id, &reward) != ERR_SUCCESS) {
        printf("[QuestManager] ERROR: Failed to get reward for quest %u\n",
               instance->quest_template_id);
        return ERR_QUEST_REWARD_FAILED;
    }

    /* ✅ UPDATED (2025-11-24): Apply reward multipliers from game configuration */
    const QuestRewardConfig* reward_config = GameConfig_GetQuestReward();
    reward.exp = (int)(reward.exp * reward_config->exp_multiplier);
    reward.gold = (int)(reward.gold * reward_config->gold_multiplier);

    /* Mark as claimed */
    instance->state = QUEST_STATE_CLAIMED;
    g_manager_state.stats.quests_completed++;

    printf("[QuestManager] Quest instance %u submitted (exp=%d, gold=%d, multipliers=%.1fx/%.1fx)\n",
           quest_instance_id, reward.exp, reward.gold,
           reward_config->exp_multiplier, reward_config->gold_multiplier);

    /* Return reward */
    if (out_reward) {
        memcpy(out_reward, &reward, sizeof(QuestReward));
    }

    /*
     * Reward distribution: Integrate with QuestReward module
     *
     * Production implementation:
     *   QuestReward_GiveExp(instance->character_id, reward.exp);
     *   QuestReward_GiveGold(instance->character_id, reward.gold);
     *   if (reward.item_count > 0) {
     *       QuestReward_GiveItems(instance->character_id, reward.item_ids, reward.item_counts, reward.item_count);
     *   }
     *
     * The caller receives the reward struct in out_reward for UI display.
     */

    return ERR_SUCCESS;
}

/**
 * Abandon a quest
 */
int QuestManager_Abandon(unsigned int quest_instance_id)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    /* Find instance */
    int index = FindInstanceIndex(quest_instance_id);
    if (index < 0) {
        return ERR_QUEST_NOT_FOUND;
    }

    QuestInstance* instance = &g_manager_state.instances[index];

    /* Can't abandon already claimed quests */
    if (instance->state == QUEST_STATE_CLAIMED) {
        return ERR_QUEST_ALREADY_CLAIMED;
    }

    /* Mark as failed */
    instance->state = QUEST_STATE_FAILED;
    g_manager_state.stats.quests_abandoned++;

    printf("[QuestManager] Quest instance %u abandoned by character %u\n",
           quest_instance_id, instance->character_id);

    return ERR_SUCCESS;
}

/**
 * Get quest instance info
 */
int QuestManager_GetInstance(unsigned int quest_instance_id, QuestInstance* out_instance)
{
    if (!g_manager_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!out_instance) {
        return ERR_INVALID_PARAMETER;
    }

    /* Find instance */
    int index = FindInstanceIndex(quest_instance_id);
    if (index < 0) {
        return ERR_QUEST_NOT_FOUND;
    }

    memcpy(out_instance, &g_manager_state.instances[index], sizeof(QuestInstance));
    return ERR_SUCCESS;
}

/**
 * Get all active quests for a character
 */
int QuestManager_GetActiveQuests(int character_id, QuestInstance* instances, int max_count)
{
    if (!g_manager_state.initialized) {
        return 0;
    }

    if (!instances || max_count <= 0) {
        return 0;
    }

    int count = 0;
    for (int i = 0; i < g_manager_state.instance_count && count < max_count; i++) {
        QuestInstance* instance = &g_manager_state.instances[i];

        if (instance->character_id == (unsigned int)character_id &&
            instance->state != QUEST_STATE_CLAIMED &&
            instance->state != QUEST_STATE_FAILED) {
            memcpy(&instances[count], instance, sizeof(QuestInstance));
            count++;
        }
    }

    return count;
}

/**
 * Cleanup quest manager
 */
void QuestManager_Cleanup(void)
{
    if (!g_manager_state.initialized) {
        return;
    }

    printf("[QuestManager] Cleaning up quest manager...\n");
    printf("[QuestManager] Statistics:\n");
    printf("  - Quests Accepted: %u\n", g_manager_state.stats.quests_accepted);
    printf("  - Quests Completed: %u\n", g_manager_state.stats.quests_completed);
    printf("  - Quests Abandoned: %u\n", g_manager_state.stats.quests_abandoned);
    printf("  - Progress Updates: %u\n", g_manager_state.stats.progress_updates);

    if (g_manager_state.instances) {
        free(g_manager_state.instances);
        g_manager_state.instances = NULL;
    }

    g_manager_state.instance_count = 0;
    g_manager_state.next_instance_id = 0;
    g_manager_state.db_manager = NULL;
    g_manager_state.initialized = 0;

    printf("[QuestManager] Cleanup complete\n");
}
