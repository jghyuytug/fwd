/**
 * Combat Log System - Header
 * Purpose: Record combat events to database
 *
 * Phase: 6.1 - Combat System Implementation
 * Created: 2025-11-23
 */

#ifndef SRC_COMBAT_COMBAT_LOG_H_
#define SRC_COMBAT_COMBAT_LOG_H_

#include <common/defs.h>
#include <time.h>

/**
 * Combat event type
 */
typedef enum {
    COMBAT_EVENT_ATTACK = 1,
    COMBAT_EVENT_SKILL = 2,
    COMBAT_EVENT_BUFF_APPLY = 3,
    COMBAT_EVENT_BUFF_REMOVE = 4,
    COMBAT_EVENT_DEATH = 5
} CombatEventType;

/**
 * Combat log entry
 */
typedef struct {
    int log_id;
    CombatEventType event_type;
    int attacker_id;
    int defender_id;
    int skill_id;
    int damage;
    bool is_critical;
    int buff_id;
    time_t timestamp;
} CombatLogEntry;

/**
 * Initialize combat log system
 * @return 0 on success
 */
int CombatLog_Initialize(void);

/**
 * Log attack event
 * @param attacker_id Attacker character ID
 * @param defender_id Defender character ID
 * @param skill_id Skill ID
 * @param damage Damage dealt
 * @param is_critical Whether it was critical
 * @return 0 on success
 */
int CombatLog_LogAttack(int attacker_id, int defender_id, int skill_id, int damage, bool is_critical);

/**
 * Log buff apply event
 * @param character_id Character ID
 * @param buff_id Buff ID
 * @param source_id Source character ID
 * @return 0 on success
 */
int CombatLog_LogBuffApply(int character_id, int buff_id, int source_id);

/**
 * Log buff remove event
 * @param character_id Character ID
 * @param buff_id Buff ID
 * @return 0 on success
 */
int CombatLog_LogBuffRemove(int character_id, int buff_id);

/**
 * Log death event
 * @param character_id Dead character ID
 * @param killer_id Killer character ID
 * @return 0 on success
 */
int CombatLog_LogDeath(int character_id, int killer_id);

/**
 * Set database manager for log persistence
 * @param db_manager Database connection manager
 */
void CombatLog_SetDatabaseManager(DBConnectionManager* db_manager);

/**
 * Flush logs to database (batched write)
 * @return Number of logs written
 */
int CombatLog_Flush(void);

/**
 * Cleanup combat log system
 */
void CombatLog_Cleanup(void);

#endif /* SRC_COMBAT_COMBAT_LOG_H_ */
