/**
 * Quest Manager Module - Header
 * Purpose: Manage quest state (accept, update, complete, submit, abandon)
 *
 * Phase: 6.2 - Quest System Implementation
 * Created: 2025-11-23
 */

#ifndef INCLUDE_QUEST_QUEST_MANAGER_H_
#define INCLUDE_QUEST_QUEST_MANAGER_H_

#include <common/defs.h>
#include "quest_config.h"
#include "../../src/quest/quest_interface.h"
#include "../../src/database/headers/db_connection_manager.h"

/**
 * Quest instance (active quest for a character)
 */
typedef struct {
    unsigned int quest_instance_id;    // Unique instance ID
    unsigned int character_id;         // Character ID
    unsigned int quest_template_id;    // Quest template ID
    QuestState state;                  // Current state
    unsigned int accept_time;          // Timestamp when accepted
    unsigned int complete_time;        // Timestamp when completed
    unsigned int expire_time;          // Expiration timestamp (0 = no expiry)

    // Objective progress
    int objective_count;               // Number of objectives
    int objective_progress[10];        // Progress for each objective (current count)
    int objective_completed[10];       // Completion flags (0/1)
} QuestInstance;

/**
 * Initialize quest manager
 * @param db_manager Database connection manager
 * @return 0 on success
 */
int QuestManager_Initialize(DBConnectionManager* db_manager);

/**
 * Accept a quest
 * @param character_id Character ID
 * @param quest_template_id Quest template ID
 * @param out_instance Output quest instance (optional)
 * @return Quest instance ID on success, negative error code on failure
 */
int QuestManager_Accept(int character_id, unsigned int quest_template_id, QuestInstance* out_instance);

/**
 * Update quest progress
 * @param character_id Character ID
 * @param objective_type Objective type
 * @param target_id Target ID (monster ID, item ID, etc.)
 * @param increment Progress increment
 * @return Number of quests updated
 */
int QuestManager_UpdateProgress(int character_id, ObjectiveType objective_type,
                                 int target_id, int increment);

/**
 * Check if quest is completed
 * @param quest_instance_id Quest instance ID
 * @return 1 if completed, 0 otherwise
 */
int QuestManager_CheckCompletion(unsigned int quest_instance_id);

/**
 * Submit (turn in) a completed quest
 * @param quest_instance_id Quest instance ID
 * @param out_reward Output reward structure (optional)
 * @return 0 on success
 */
int QuestManager_Submit(unsigned int quest_instance_id, QuestReward* out_reward);

/**
 * Abandon a quest
 * @param quest_instance_id Quest instance ID
 * @return 0 on success
 */
int QuestManager_Abandon(unsigned int quest_instance_id);

/**
 * Get quest instance info
 * @param quest_instance_id Quest instance ID
 * @param out_instance Output quest instance
 * @return 0 on success
 */
int QuestManager_GetInstance(unsigned int quest_instance_id, QuestInstance* out_instance);

/**
 * Get all active quests for a character
 * @param character_id Character ID
 * @param instances Output array of quest instances
 * @param max_count Maximum number of instances to retrieve
 * @return Number of active quests
 */
int QuestManager_GetActiveQuests(int character_id, QuestInstance* instances, int max_count);

/**
 * Set database manager for quest persistence
 * @param db_manager Database connection manager
 */
void QuestManager_SetDatabaseManager(DBConnectionManager* db_manager);

/**
 * Cleanup quest manager
 */
void QuestManager_Cleanup(void);

#endif /* INCLUDE_QUEST_QUEST_MANAGER_H_ */
