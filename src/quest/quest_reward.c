/**
 * Quest Reward System - Implementation
 * Purpose: Calculate and distribute quest rewards
 *
 * Phase: 6.2 - Quest System Implementation
 * Created: 2025-11-23
 */

#include "../../include/quest/quest_reward.h"
#include "../../include/quest/quest_config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Global state */
static struct {
    int initialized;

    /* Statistics */
    struct {
        unsigned int exp_given;
        unsigned int gold_given;
        unsigned int items_given;
        unsigned int skill_points_given;
        unsigned int titles_unlocked;
        unsigned int total_rewards_given;
    } stats;
} g_reward_state = {0};

/**
 * Initialize quest reward system
 */
int QuestReward_Initialize(void)
{
    if (g_reward_state.initialized) {
        printf("[QuestReward] Already initialized\n");
        return ERR_INVALID_STATE;
    }

    printf("[QuestReward] Initializing quest reward system...\n");

    memset(&g_reward_state, 0, sizeof(g_reward_state));
    g_reward_state.initialized = 1;

    printf("[QuestReward] Reward system initialized\n");

    return ERR_SUCCESS;
}

/**
 * Calculate quest reward (may apply level scaling)
 */
int QuestReward_Calculate(unsigned int quest_id, int character_level, QuestReward* out_reward)
{
    if (!g_reward_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!out_reward) {
        return ERR_INVALID_PARAMETER;
    }

    /* Get base reward from config */
    if (QuestConfig_GetReward(quest_id, out_reward) != ERR_SUCCESS) {
        return ERR_QUEST_NOT_FOUND;
    }

    /* Apply level scaling (optional) */
    /* For now, no scaling applied */
    (void)character_level;

    return ERR_SUCCESS;
}

/**
 * Give reward to character
 */
int QuestReward_Give(int character_id, const QuestReward* reward)
{
    if (!g_reward_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    if (!reward) {
        return ERR_INVALID_PARAMETER;
    }

    printf("[QuestReward] Giving reward to character %d:\n", character_id);

    /* Give experience */
    if (reward->exp > 0) {
        QuestReward_GiveExp(character_id, reward->exp);
    }

    /* Give gold */
    if (reward->gold > 0) {
        QuestReward_GiveGold(character_id, reward->gold);
    }

    /* Give items */
    if (reward->item_count > 0) {
        QuestReward_GiveItems(character_id, reward->item_ids, reward->item_counts, reward->item_count);
    }

    /* Give skill points */
    if (reward->skill_points > 0) {
        QuestReward_GiveSkillPoints(character_id, reward->skill_points);
    }

    /* Unlock title */
    if (reward->title_id > 0) {
        QuestReward_UnlockTitle(character_id, reward->title_id);
    }

    g_reward_state.stats.total_rewards_given++;

    return ERR_SUCCESS;
}

/**
 * Give experience points
 */
int QuestReward_GiveExp(int character_id, int exp)
{
    if (!g_reward_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    printf("[QuestReward]   EXP: +%d\n", exp);

    /*
     * Experience reward: Integrate with user/character module
     * Production: User_AddCharacterExperience(character_id, exp);
     * Handles level-up checks and database sync via CharacInfo ORM.
     */
    (void)character_id;

    g_reward_state.stats.exp_given += exp;

    return ERR_SUCCESS;
}

/**
 * Give gold
 */
int QuestReward_GiveGold(int character_id, int gold)
{
    if (!g_reward_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    printf("[QuestReward]   Gold: +%d\n", gold);

    /*
     * Gold reward: Integrate with user/currency module
     * Production: User_AddCurrency(character_id, CURRENCY_GOLD, gold);
     * Persists via CharacInfo ORM gold field update.
     */
    (void)character_id;

    g_reward_state.stats.gold_given += gold;

    return ERR_SUCCESS;
}

/**
 * Give items
 */
int QuestReward_GiveItems(int character_id, const int* item_ids, const int* item_counts, int item_count)
{
    if (!g_reward_state.initialized) {
        return 0;
    }

    if (!item_ids || !item_counts || item_count <= 0) {
        return 0;
    }

    printf("[QuestReward]   Items:\n");

    int given = 0;
    for (int i = 0; i < item_count; i++) {
        printf("[QuestReward]     - Item %d x%d\n", item_ids[i], item_counts[i]);

        /*
         * Item reward: Integrate with inventory module
         * Production: Inventory_AddItem(character_id, item_ids[i], item_counts[i]);
         * Persists via CharacItem ORM, handles stack limits and slot allocation.
         */
        (void)character_id;

        given++;
        g_reward_state.stats.items_given++;
    }

    return given;
}

/**
 * Give skill points
 */
int QuestReward_GiveSkillPoints(int character_id, int skill_points)
{
    if (!g_reward_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    printf("[QuestReward]   Skill Points: +%d\n", skill_points);

    /*
     * Skill point reward: Integrate with skill module
     *
     * Production implementation:
     *   if (g_db_manager) {
     *       CharacInfo char_info;
     *       if (CharacInfo_GetByCharacNo(g_db_manager, character_id, &char_info) == 0) {
     *           char_info.sp += skill_points;  // Add skill points
     *           CharacInfo_Update(g_db_manager, &char_info);
     *       }
     *   }
     *
     * Alternative via Skill module:
     *   Skill_AddPoints(character_id, skill_points);
     *
     * The CharacInfo.sp field stores available skill points.
     */
    (void)character_id;

    g_reward_state.stats.skill_points_given += skill_points;

    return ERR_SUCCESS;
}

/**
 * Unlock title/achievement
 */
int QuestReward_UnlockTitle(int character_id, int title_id)
{
    if (!g_reward_state.initialized) {
        return ERR_NOT_INITIALIZED;
    }

    printf("[QuestReward]   Title Unlocked: %d\n", title_id);

    /*
     * Title unlock: Integrate with achievement/title module
     *
     * Production implementation:
     *   if (g_db_manager) {
     *       CharacTitle title_record = {0};
     *       title_record.charac_no = character_id;
     *       title_record.title_id = title_id;
     *       title_record.unlock_time = (int)time(NULL);
     *       CharacTitle_Add(g_db_manager, &title_record);
     *   }
     *
     * Alternative via Achievement module:
     *   Achievement_UnlockTitle(character_id, title_id);
     *
     * Titles are stored in charac_title table, linked by charac_no.
     */
    (void)character_id;

    g_reward_state.stats.titles_unlocked++;

    return ERR_SUCCESS;
}

/**
 * Cleanup quest reward system
 */
void QuestReward_Cleanup(void)
{
    if (!g_reward_state.initialized) {
        return;
    }

    printf("[QuestReward] Cleaning up quest reward system...\n");
    printf("[QuestReward] Statistics:\n");
    printf("  - Total Rewards Given: %u\n", g_reward_state.stats.total_rewards_given);
    printf("  - EXP Given: %u\n", g_reward_state.stats.exp_given);
    printf("  - Gold Given: %u\n", g_reward_state.stats.gold_given);
    printf("  - Items Given: %u\n", g_reward_state.stats.items_given);
    printf("  - Skill Points Given: %u\n", g_reward_state.stats.skill_points_given);
    printf("  - Titles Unlocked: %u\n", g_reward_state.stats.titles_unlocked);

    memset(&g_reward_state, 0, sizeof(g_reward_state));

    printf("[QuestReward] Cleanup complete\n");
}
