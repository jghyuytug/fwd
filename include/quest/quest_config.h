/**
 * Quest Configuration Module - Header
 * Purpose: Load and manage quest templates, rewards, and prerequisites
 *
 * Phase: 6.2 - Quest System Implementation
 * Created: 2025-11-23
 */

#ifndef INCLUDE_QUEST_QUEST_CONFIG_H_
#define INCLUDE_QUEST_QUEST_CONFIG_H_

#include <common/defs.h>
#include "../../src/quest/quest_interface.h"

/**
 * Quest reward structure
 */
typedef struct {
    int exp;                    // Experience points
    int gold;                   // Gold reward
    int skill_points;           // Skill points
    int item_ids[10];          // Item IDs (up to 10 items)
    int item_counts[10];       // Item counts
    int item_count;            // Number of items
    int title_id;              // Title/Achievement ID
} QuestReward;

/**
 * Quest prerequisite structure
 */
typedef struct {
    int min_level;             // Minimum level required
    int max_level;             // Maximum level allowed (0 = no limit)
    int required_quest_id;     // Required prerequisite quest ID (0 = none)
    int required_job_class;    // Required job class (0 = any)
    int required_reputation;   // Required reputation level
} QuestPrerequisite;

/**
 * Quest template structure
 */
typedef struct {
    unsigned int quest_id;                 // Quest template ID
    char quest_name[128];                  // Quest name
    char quest_description[512];           // Quest description
    QuestType quest_type;                  // Quest type
    int duration_minutes;                  // Duration in minutes (0 = no limit)

    // Objectives
    int objective_count;                   // Number of objectives
    ObjectiveType objective_types[10];     // Objective types
    int objective_targets[10];             // Target IDs
    int objective_counts[10];              // Required counts

    // Rewards
    QuestReward reward;                    // Quest reward

    // Prerequisites
    QuestPrerequisite prerequisite;        // Quest prerequisites
} QuestTemplate;

/**
 * Initialize quest configuration system
 * @return 0 on success
 */
int QuestConfig_Initialize(void);

/**
 * Load quest template from database
 * @param quest_id Quest template ID
 * @param out_template Output quest template
 * @return 0 on success
 */
int QuestConfig_LoadTemplate(unsigned int quest_id, QuestTemplate* out_template);

/**
 * Load all quest templates
 * @param templates Output array of templates
 * @param max_count Maximum templates to load
 * @return Number of templates loaded
 */
int QuestConfig_LoadAllTemplates(QuestTemplate* templates, int max_count);

/**
 * Check if player meets quest prerequisites
 * @param character_id Character ID
 * @param quest_id Quest template ID
 * @return 1 if prerequisites met, 0 otherwise
 */
int QuestConfig_CheckPrerequisites(int character_id, unsigned int quest_id);

/**
 * Get quest reward information
 * @param quest_id Quest template ID
 * @param out_reward Output reward structure
 * @return 0 on success
 */
int QuestConfig_GetReward(unsigned int quest_id, QuestReward* out_reward);

/**
 * Cleanup quest configuration system
 */
void QuestConfig_Cleanup(void);

#endif /* INCLUDE_QUEST_QUEST_CONFIG_H_ */
