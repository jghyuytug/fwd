/**
 * Quest Reward System - Header
 * Purpose: Calculate and distribute quest rewards
 *
 * Phase: 6.2 - Quest System Implementation
 * Created: 2025-11-23
 */

#ifndef INCLUDE_QUEST_QUEST_REWARD_H_
#define INCLUDE_QUEST_QUEST_REWARD_H_

#include <common/defs.h>
#include "quest_config.h"

/**
 * Initialize quest reward system
 * @return 0 on success
 */
int QuestReward_Initialize(void);

/**
 * Calculate quest reward (may apply level scaling)
 * @param quest_id Quest template ID
 * @param character_level Character level
 * @param out_reward Output calculated reward
 * @return 0 on success
 */
int QuestReward_Calculate(unsigned int quest_id, int character_level, QuestReward* out_reward);

/**
 * Give reward to character
 * @param character_id Character ID
 * @param reward Reward to give
 * @return 0 on success
 */
int QuestReward_Give(int character_id, const QuestReward* reward);

/**
 * Give experience points
 * @param character_id Character ID
 * @param exp Experience points
 * @return 0 on success
 */
int QuestReward_GiveExp(int character_id, int exp);

/**
 * Give gold
 * @param character_id Character ID
 * @param gold Gold amount
 * @return 0 on success
 */
int QuestReward_GiveGold(int character_id, int gold);

/**
 * Give items
 * @param character_id Character ID
 * @param item_ids Array of item IDs
 * @param item_counts Array of item counts
 * @param item_count Number of items
 * @return Number of items given
 */
int QuestReward_GiveItems(int character_id, const int* item_ids, const int* item_counts, int item_count);

/**
 * Give skill points
 * @param character_id Character ID
 * @param skill_points Skill points
 * @return 0 on success
 */
int QuestReward_GiveSkillPoints(int character_id, int skill_points);

/**
 * Unlock title/achievement
 * @param character_id Character ID
 * @param title_id Title ID
 * @return 0 on success
 */
int QuestReward_UnlockTitle(int character_id, int title_id);

/**
 * Cleanup quest reward system
 */
void QuestReward_Cleanup(void);

#endif /* INCLUDE_QUEST_QUEST_REWARD_H_ */
