/**
 * DNF游戏服务器 - LogCombat表ORM实现
 *
 * 功能: 战斗日志记录
 * 数据库: taiwan_cain_log
 * 表名: log_combat
 * 创建: 2025-11-23 (Phase 6.1)
 */

#include "../headers/log_combat_orm.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ========================================================================
 * 插入操作
 * ======================================================================== */

/**
 * 插入战斗日志记录
 */
int LogCombat_Insert(DBConnectionManager* manager, LogCombat* log) {
    if (!manager || !log) {
        return -1;
    }

    DBConnection* conn = DBConnectionManager_AcquireConnection(manager, DB_TYPE_TAIWAN_CAIN_LOG);
    if (!conn) {
        return -1;
    }

    char query[1024];
    snprintf(query, sizeof(query),
        "INSERT INTO log_combat "
        "(event_type, attacker_id, defender_id, skill_id, damage, is_critical, buff_id, timestamp) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, NOW())",
        log->event_type,
        log->attacker_id,
        log->defender_id,
        log->skill_id,
        log->damage,
        log->is_critical,
        log->buff_id
    );

    int result = DBConnection_Execute(conn, query);
    DBConnectionManager_ReleaseConnection(manager, conn);

    return (result == 0) ? 0 : -1;
}

/**
 * 批量插入战斗日志记录
 */
int LogCombat_BatchInsert(DBConnectionManager* manager, LogCombat* logs, int count) {
    if (!manager || !logs || count <= 0) {
        return -1;
    }

    DBConnection* conn = DBConnectionManager_AcquireConnection(manager, DB_TYPE_TAIWAN_CAIN_LOG);
    if (!conn) {
        return -1;
    }

    // 开始事务以提高批量插入性能
    DBConnection_Execute(conn, "START TRANSACTION");

    int inserted = 0;
    for (int i = 0; i < count; i++) {
        char query[1024];
        snprintf(query, sizeof(query),
            "INSERT INTO log_combat "
            "(event_type, attacker_id, defender_id, skill_id, damage, is_critical, buff_id, timestamp) "
            "VALUES (%d, %d, %d, %d, %d, %d, %d, NOW())",
            logs[i].event_type,
            logs[i].attacker_id,
            logs[i].defender_id,
            logs[i].skill_id,
            logs[i].damage,
            logs[i].is_critical,
            logs[i].buff_id
        );

        if (DBConnection_Execute(conn, query) == 0) {
            inserted++;
        }
    }

    // 提交事务
    DBConnection_Execute(conn, "COMMIT");

    DBConnectionManager_ReleaseConnection(manager, conn);

    return inserted;
}

/* ========================================================================
 * 查询操作
 * ======================================================================== */

/**
 * 根据日志ID查询战斗日志
 */
int LogCombat_GetByLogId(DBConnectionManager* manager, int log_id, LogCombat* log) {
    if (!manager || !log) {
        return -1;
    }

    DBConnection* conn = DBConnectionManager_AcquireConnection(manager, DB_TYPE_TAIWAN_CAIN_LOG);
    if (!conn) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT log_id, event_type, attacker_id, defender_id, skill_id, damage, is_critical, buff_id, timestamp "
        "FROM log_combat WHERE log_id = %d",
        log_id
    );

    DBResult* result = DBConnection_Query(conn, query);
    if (!result || DBResult_GetRowCount(result) == 0) {
        if (result) DBResult_Free(result);
        DBConnectionManager_ReleaseConnection(manager, conn);
        return -1;
    }

    DBRow* row = DBResult_FetchRow(result);
    if (row) {
        log->log_id = atoi(DBRow_GetField(row, 0));
        log->event_type = atoi(DBRow_GetField(row, 1));
        log->attacker_id = atoi(DBRow_GetField(row, 2));
        log->defender_id = atoi(DBRow_GetField(row, 3));
        log->skill_id = atoi(DBRow_GetField(row, 4));
        log->damage = atoi(DBRow_GetField(row, 5));
        log->is_critical = atoi(DBRow_GetField(row, 6));
        log->buff_id = atoi(DBRow_GetField(row, 7));
        strncpy(log->timestamp, DBRow_GetField(row, 8), sizeof(log->timestamp) - 1);
    }

    DBResult_Free(result);
    DBConnectionManager_ReleaseConnection(manager, conn);

    return 0;
}

/**
 * 查询指定角色的战斗日志
 */
int LogCombat_GetByCharacterId(DBConnectionManager* manager, int character_id, LogCombat* logs, int max_count) {
    if (!manager || !logs || max_count <= 0) {
        return 0;
    }

    DBConnection* conn = DBConnectionManager_AcquireConnection(manager, DB_TYPE_TAIWAN_CAIN_LOG);
    if (!conn) {
        return 0;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "SELECT log_id, event_type, attacker_id, defender_id, skill_id, damage, is_critical, buff_id, timestamp "
        "FROM log_combat WHERE attacker_id = %d OR defender_id = %d "
        "ORDER BY log_id DESC LIMIT %d",
        character_id, character_id, max_count
    );

    DBResult* result = DBConnection_Query(conn, query);
    if (!result) {
        DBConnectionManager_ReleaseConnection(manager, conn);
        return 0;
    }

    int count = 0;
    int row_count = DBResult_GetRowCount(result);

    for (int i = 0; i < row_count && count < max_count; i++) {
        DBRow* row = DBResult_FetchRow(result);
        if (row) {
            logs[count].log_id = atoi(DBRow_GetField(row, 0));
            logs[count].event_type = atoi(DBRow_GetField(row, 1));
            logs[count].attacker_id = atoi(DBRow_GetField(row, 2));
            logs[count].defender_id = atoi(DBRow_GetField(row, 3));
            logs[count].skill_id = atoi(DBRow_GetField(row, 4));
            logs[count].damage = atoi(DBRow_GetField(row, 5));
            logs[count].is_critical = atoi(DBRow_GetField(row, 6));
            logs[count].buff_id = atoi(DBRow_GetField(row, 7));
            strncpy(logs[count].timestamp, DBRow_GetField(row, 8), sizeof(logs[count].timestamp) - 1);
            count++;
        }
    }

    DBResult_Free(result);
    DBConnectionManager_ReleaseConnection(manager, conn);

    return count;
}

/**
 * 查询指定时间范围内的战斗日志
 */
int LogCombat_GetByTimeRange(DBConnectionManager* manager, const char* start_time, const char* end_time, LogCombat* logs, int max_count) {
    if (!manager || !start_time || !end_time || !logs || max_count <= 0) {
        return 0;
    }

    DBConnection* conn = DBConnectionManager_AcquireConnection(manager, DB_TYPE_TAIWAN_CAIN_LOG);
    if (!conn) {
        return 0;
    }

    char query[1024];
    snprintf(query, sizeof(query),
        "SELECT log_id, event_type, attacker_id, defender_id, skill_id, damage, is_critical, buff_id, timestamp "
        "FROM log_combat WHERE timestamp >= '%s' AND timestamp <= '%s' "
        "ORDER BY log_id DESC LIMIT %d",
        start_time, end_time, max_count
    );

    DBResult* result = DBConnection_Query(conn, query);
    if (!result) {
        DBConnectionManager_ReleaseConnection(manager, conn);
        return 0;
    }

    int count = 0;
    int row_count = DBResult_GetRowCount(result);

    for (int i = 0; i < row_count && count < max_count; i++) {
        DBRow* row = DBResult_FetchRow(result);
        if (row) {
            logs[count].log_id = atoi(DBRow_GetField(row, 0));
            logs[count].event_type = atoi(DBRow_GetField(row, 1));
            logs[count].attacker_id = atoi(DBRow_GetField(row, 2));
            logs[count].defender_id = atoi(DBRow_GetField(row, 3));
            logs[count].skill_id = atoi(DBRow_GetField(row, 4));
            logs[count].damage = atoi(DBRow_GetField(row, 5));
            logs[count].is_critical = atoi(DBRow_GetField(row, 6));
            logs[count].buff_id = atoi(DBRow_GetField(row, 7));
            strncpy(logs[count].timestamp, DBRow_GetField(row, 8), sizeof(logs[count].timestamp) - 1);
            count++;
        }
    }

    DBResult_Free(result);
    DBConnectionManager_ReleaseConnection(manager, conn);

    return count;
}

/* ========================================================================
 * 删除操作
 * ======================================================================== */

/**
 * 删除指定时间之前的战斗日志（数据清理）
 */
int LogCombat_DeleteBefore(DBConnectionManager* manager, const char* before_time) {
    if (!manager || !before_time) {
        return -1;
    }

    DBConnection* conn = DBConnectionManager_AcquireConnection(manager, DB_TYPE_TAIWAN_CAIN_LOG);
    if (!conn) {
        return -1;
    }

    char query[512];
    snprintf(query, sizeof(query),
        "DELETE FROM log_combat WHERE timestamp < '%s'",
        before_time
    );

    int affected_rows = DBConnection_Execute(conn, query);
    DBConnectionManager_ReleaseConnection(manager, conn);

    return affected_rows;
}

/* ========================================================================
 * 统计操作
 * ======================================================================== */

/**
 * 获取战斗日志总数
 */
int LogCombat_GetCount(DBConnectionManager* manager) {
    if (!manager) {
        return 0;
    }

    DBConnection* conn = DBConnectionManager_AcquireConnection(manager, DB_TYPE_TAIWAN_CAIN_LOG);
    if (!conn) {
        return 0;
    }

    const char* query = "SELECT COUNT(*) FROM log_combat";
    DBResult* result = DBConnection_Query(conn, query);

    if (!result) {
        DBConnectionManager_ReleaseConnection(manager, conn);
        return 0;
    }

    int count = 0;
    DBRow* row = DBResult_FetchRow(result);
    if (row) {
        count = atoi(DBRow_GetField(row, 0));
    }

    DBResult_Free(result);
    DBConnectionManager_ReleaseConnection(manager, conn);

    return count;
}
