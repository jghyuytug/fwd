/**
 * Combat Log System - Implementation
 *
 * Phase: 6.1 - Combat System Implementation (ORM Integrated)
 * Created: 2025-11-23
 * Updated: 2025-11-23 - Database persistence integrated
 */

#include "combat_log.h"
#include "../../src/orm/audit_logging/taiwan_cain_log/headers/log_combat_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LOG_BUFFER 1000

static CombatLogEntry g_log_buffer[MAX_LOG_BUFFER];
static int g_log_count = 0;
static int g_next_log_id = 1;
static DBConnectionManager* g_db_manager = NULL;

/**
 * Initialize combat log system
 */
int CombatLog_Initialize(void) {
    memset(g_log_buffer, 0, sizeof(g_log_buffer));
    g_log_count = 0;
    g_next_log_id = 1;
    g_db_manager = NULL;  // Will be set externally if needed
    printf("[Combat] Log system initialized\n");
    return 0;
}

/**
 * Set database manager for log persistence
 */
void CombatLog_SetDatabaseManager(DBConnectionManager* db_manager) {
    g_db_manager = db_manager;
    if (db_manager) {
        printf("[Combat] Database manager configured for log persistence\n");
    }
}

/**
 * Add log entry to buffer
 */
static int AddLogEntry(const CombatLogEntry* entry) {
    if (g_log_count >= MAX_LOG_BUFFER) {
        // Auto-flush when buffer full
        CombatLog_Flush();
    }

    g_log_buffer[g_log_count++] = *entry;
    return 0;
}

/**
 * Log attack event
 */
int CombatLog_LogAttack(int attacker_id, int defender_id, int skill_id, int damage, bool is_critical) {
    CombatLogEntry entry = {0};
    entry.log_id = g_next_log_id++;
    entry.event_type = (skill_id > 0) ? COMBAT_EVENT_SKILL : COMBAT_EVENT_ATTACK;
    entry.attacker_id = attacker_id;
    entry.defender_id = defender_id;
    entry.skill_id = skill_id;
    entry.damage = damage;
    entry.is_critical = is_critical;
    entry.timestamp = time(NULL);

    printf("[Combat] LOG: Attack - %d -> %d, Skill:%d, Damage:%d%s\n",
        attacker_id, defender_id, skill_id, damage,
        is_critical ? " (CRIT)" : "");

    return AddLogEntry(&entry);
}

/**
 * Log buff apply event
 */
int CombatLog_LogBuffApply(int character_id, int buff_id, int source_id) {
    CombatLogEntry entry = {0};
    entry.log_id = g_next_log_id++;
    entry.event_type = COMBAT_EVENT_BUFF_APPLY;
    entry.attacker_id = source_id;
    entry.defender_id = character_id;
    entry.buff_id = buff_id;
    entry.timestamp = time(NULL);

    printf("[Combat] LOG: Buff Apply - Buff:%d on Character:%d from Source:%d\n",
        buff_id, character_id, source_id);

    return AddLogEntry(&entry);
}

/**
 * Log buff remove event
 */
int CombatLog_LogBuffRemove(int character_id, int buff_id) {
    CombatLogEntry entry = {0};
    entry.log_id = g_next_log_id++;
    entry.event_type = COMBAT_EVENT_BUFF_REMOVE;
    entry.defender_id = character_id;
    entry.buff_id = buff_id;
    entry.timestamp = time(NULL);

    printf("[Combat] LOG: Buff Remove - Buff:%d from Character:%d\n",
        buff_id, character_id);

    return AddLogEntry(&entry);
}

/**
 * Log death event
 */
int CombatLog_LogDeath(int character_id, int killer_id) {
    CombatLogEntry entry = {0};
    entry.log_id = g_next_log_id++;
    entry.event_type = COMBAT_EVENT_DEATH;
    entry.attacker_id = killer_id;
    entry.defender_id = character_id;
    entry.timestamp = time(NULL);

    printf("[Combat] LOG: Death - Character:%d killed by:%d\n",
        character_id, killer_id);

    return AddLogEntry(&entry);
}

/**
 * Flush logs to database
 */
int CombatLog_Flush(void) {
    if (g_log_count == 0) {
        return 0;
    }

    int flushed = 0;

    // If database manager is configured, persist logs to database
    if (g_db_manager) {
        // Convert CombatLogEntry to LogCombat format and batch insert
        LogCombat* db_logs = (LogCombat*)malloc(g_log_count * sizeof(LogCombat));
        if (db_logs) {
            for (int i = 0; i < g_log_count; i++) {
                memset(&db_logs[i], 0, sizeof(LogCombat));
                db_logs[i].event_type = g_log_buffer[i].event_type;
                db_logs[i].attacker_id = g_log_buffer[i].attacker_id;
                db_logs[i].defender_id = g_log_buffer[i].defender_id;
                db_logs[i].skill_id = g_log_buffer[i].skill_id;
                db_logs[i].damage = g_log_buffer[i].damage;
                db_logs[i].is_critical = g_log_buffer[i].is_critical ? 1 : 0;
                db_logs[i].buff_id = g_log_buffer[i].buff_id;
                // timestamp will be set to NOW() in database
            }

            // Batch insert for better performance
            int inserted = LogCombat_BatchInsert(g_db_manager, db_logs, g_log_count);
            free(db_logs);

            if (inserted > 0) {
                printf("[Combat] Flushed %d log entries to database (taiwan_cain_log.log_combat)\n", inserted);
                flushed = inserted;
            } else {
                printf("[Combat] ERROR: Failed to flush logs to database\n");
            }
        } else {
            printf("[Combat] ERROR: Failed to allocate memory for log flush\n");
        }
    } else {
        // No database manager configured, just discard logs
        printf("[Combat] Flushed %d log entries (no database configured, discarded)\n", g_log_count);
        flushed = g_log_count;
    }

    // Clear buffer
    g_log_count = 0;

    return flushed;
}

/**
 * Cleanup combat log system
 */
void CombatLog_Cleanup(void) {
    // Flush remaining logs
    CombatLog_Flush();

    memset(g_log_buffer, 0, sizeof(g_log_buffer));
    g_log_count = 0;
    printf("[Combat] Log system cleanup complete\n");
}
