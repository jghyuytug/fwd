/**
 * Quest Trigger System - Header
 * Purpose: Listen to game events and update quest progress automatically
 *
 * Phase: 6.2 - Quest System Implementation
 * Created: 2025-11-23
 */

#ifndef INCLUDE_QUEST_QUEST_TRIGGER_H_
#define INCLUDE_QUEST_QUEST_TRIGGER_H_

#include <common/defs.h>
#include "../../src/quest/quest_interface.h"

/**
 * Quest trigger callback function type
 * @param character_id Character ID
 * @param target_id Target ID (monster ID, item ID, etc.)
 * @param count Count/increment
 */
typedef void (*QuestTriggerCallback)(int character_id, int target_id, int count);

/**
 * Quest trigger structure
 */
typedef struct {
    int trigger_id;                   // Unique trigger ID
    ObjectiveType objective_type;     // Objective type to trigger on
    int target_id;                    // Target ID (0 = any)
    QuestTriggerCallback callback;    // Callback function
    int active;                       // Active flag
} QuestTrigger;

/**
 * Initialize quest trigger system
 * @return 0 on success
 */
int QuestTrigger_Initialize(void);

/**
 * Register a quest trigger
 * @param objective_type Objective type
 * @param target_id Target ID (0 = any)
 * @param callback Callback function
 * @return Trigger ID on success, negative error code on failure
 */
int QuestTrigger_Register(ObjectiveType objective_type, int target_id, QuestTriggerCallback callback);

/**
 * Unregister a quest trigger
 * @param trigger_id Trigger ID
 * @return 0 on success
 */
int QuestTrigger_Unregister(int trigger_id);

/**
 * Trigger event: Monster killed
 * @param character_id Character ID
 * @param monster_id Monster ID
 */
void QuestTrigger_OnMonsterKilled(int character_id, int monster_id);

/**
 * Trigger event: Item obtained
 * @param character_id Character ID
 * @param item_id Item ID
 * @param count Item count
 */
void QuestTrigger_OnItemObtained(int character_id, int item_id, int count);

/**
 * Trigger event: NPC talk
 * @param character_id Character ID
 * @param npc_id NPC ID
 */
void QuestTrigger_OnNPCTalk(int character_id, int npc_id);

/**
 * Trigger event: Dungeon completed
 * @param character_id Character ID
 * @param dungeon_id Dungeon ID
 */
void QuestTrigger_OnDungeonComplete(int character_id, int dungeon_id);

/**
 * Trigger event: Level reached
 * @param character_id Character ID
 * @param level New level
 */
void QuestTrigger_OnLevelReached(int character_id, int level);

/**
 * Trigger event: Item used
 * @param character_id Character ID
 * @param item_id Item ID
 */
void QuestTrigger_OnItemUsed(int character_id, int item_id);

/**
 * Cleanup quest trigger system
 */
void QuestTrigger_Cleanup(void);

#endif /* INCLUDE_QUEST_QUEST_TRIGGER_H_ */
