#include "dnf_event_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192
#define MAX_FIELDS 16

/* 辅助函数：转义字符串 */
static void escape_string(const char* src, char* dest, size_t dest_size) {
    size_t i, j = 0;
    for (i = 0; src[i] && j < dest_size - 1; i++) {
        if (src[i] == '\'' || src[i] == '\\' || src[i] == '"') {
            if (j < dest_size - 2) {
                dest[j++] = '\\';
            }
        }
        dest[j++] = src[i];
    }
    dest[j] = '\0';
}

/* ==================== CRUD 操作实现 ==================== */

int DnfEventLog_Add(DBConnectionManager* manager, const DnfEventLog* record, unsigned int* inserted_id) {
    char query[MAX_QUERY_LEN];
    char escaped_expl[401];
    char escaped_etc[201];
    DBQueryResult result;

    if (!manager || !record) {
        fprintf(stderr, "[DnfEventLog_Add] Invalid parameters\n");
        return -1;
    }

    /* 转义字符串字段 */
    escape_string(record->expl, escaped_expl, sizeof(escaped_expl));
    escape_string(record->etc, escaped_etc, sizeof(escaped_etc));

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_event_log "
        "(occ_time, event_type, parameter1, parameter2, server_id, event_flag, "
        "start_time, end_time, m_id, expl, etc) "
        "VALUES (%d, %u, %u, %u, %u, %d, %d, %d, %d, '%s', '%s')",
        record->occ_time,
        record->event_type,
        record->parameter1,
        record->parameter2,
        record->server_id,
        record->event_flag,
        record->start_time,
        record->end_time,
        record->m_id,
        escaped_expl,
        escaped_etc);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[DnfEventLog_Add] Query failed\n");
        return -1;
    }

    DBQueryResult_Free(&result);

    /* 如果需要返回插入的ID，查询LAST_INSERT_ID() */
    if (inserted_id) {
        char id_query[256];
        DBQueryResult id_result;
        char* fields[MAX_FIELDS];

        snprintf(id_query, sizeof(id_query), "SELECT LAST_INSERT_ID()");
        if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, id_query, &id_result) < 0) {
            fprintf(stderr, "[DnfEventLog_Add] Failed to get insert ID\n");
            return -1;
        }

        if (id_result.row_count > 0 && DBQueryResult_FetchRow(&id_result, fields) > 0) {
            *inserted_id = fields[0] ? (unsigned int)atoi(fields[0]) : 0;
        }

        DBQueryResult_Free(&id_result);
    }

    return 0;
}

int DnfEventLog_Get(DBConnectionManager* manager, unsigned int log_id, DnfEventLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* fields[MAX_FIELDS];
    int field_count;

    if (!manager || !record) {
        fprintf(stderr, "[DnfEventLog_Get] Invalid parameters\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT log_id, occ_time, event_type, parameter1, parameter2, server_id, "
        "event_flag, start_time, end_time, m_id, expl, etc "
        "FROM dnf_event_log WHERE log_id = %u",
        log_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[DnfEventLog_Get] Query failed\n");
        return -1;
    }

    if (result.row_count == 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    field_count = DBQueryResult_FetchRow(&result, fields);
    if (field_count < 12) {
        fprintf(stderr, "[DnfEventLog_Get] Incomplete data\n");
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 解析字段 */
    record->log_id = fields[0] ? (unsigned int)atoi(fields[0]) : 0;
    record->occ_time = fields[1] ? atoi(fields[1]) : 0;
    record->event_type = fields[2] ? (unsigned char)atoi(fields[2]) : 0;
    record->parameter1 = fields[3] ? (unsigned int)atoi(fields[3]) : 0;
    record->parameter2 = fields[4] ? (unsigned int)atoi(fields[4]) : 0;
    record->server_id = fields[5] ? (unsigned char)atoi(fields[5]) : 0;
    record->event_flag = fields[6] ? (signed char)atoi(fields[6]) : 0;
    record->start_time = fields[7] ? atoi(fields[7]) : 0;
    record->end_time = fields[8] ? atoi(fields[8]) : 0;
    record->m_id = fields[9] ? atoi(fields[9]) : 0;
    strncpy(record->expl, fields[10] ? fields[10] : "", sizeof(record->expl) - 1);
    record->expl[sizeof(record->expl) - 1] = '\0';
    strncpy(record->etc, fields[11] ? fields[11] : "", sizeof(record->etc) - 1);
    record->etc[sizeof(record->etc) - 1] = '\0';

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventLog_Update(DBConnectionManager* manager, const DnfEventLog* record) {
    char query[MAX_QUERY_LEN];
    char escaped_expl[401];
    char escaped_etc[201];
    DBQueryResult result;

    if (!manager || !record) {
        fprintf(stderr, "[DnfEventLog_Update] Invalid parameters\n");
        return -1;
    }

    /* 转义字符串字段 */
    escape_string(record->expl, escaped_expl, sizeof(escaped_expl));
    escape_string(record->etc, escaped_etc, sizeof(escaped_etc));

    snprintf(query, sizeof(query),
        "UPDATE dnf_event_log SET "
        "occ_time = %d, event_type = %u, parameter1 = %u, parameter2 = %u, "
        "server_id = %u, event_flag = %d, start_time = %d, end_time = %d, "
        "m_id = %d, expl = '%s', etc = '%s' "
        "WHERE log_id = %u",
        record->occ_time,
        record->event_type,
        record->parameter1,
        record->parameter2,
        record->server_id,
        record->event_flag,
        record->start_time,
        record->end_time,
        record->m_id,
        escaped_expl,
        escaped_etc,
        record->log_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[DnfEventLog_Update] Query failed\n");
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventLog_Delete(DBConnectionManager* manager, unsigned int log_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        fprintf(stderr, "[DnfEventLog_Delete] Invalid manager\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_event_log WHERE log_id = %u", log_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[DnfEventLog_Delete] Query failed\n");
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventLog_Exists(DBConnectionManager* manager, unsigned int log_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    int exists;

    if (!manager) {
        fprintf(stderr, "[DnfEventLog_Exists] Invalid manager\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT 1 FROM dnf_event_log WHERE log_id = %u LIMIT 1", log_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[DnfEventLog_Exists] Query failed\n");
        return -1;
    }

    exists = (result.row_count > 0) ? 1 : 0;
    DBQueryResult_Free(&result);
    return exists;
}

/* ==================== 业务操作实现 ==================== */

int DnfEventLog_GetCountByTimeRange(DBConnectionManager* manager, int start_time, int end_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* fields[MAX_FIELDS];
    int count;

    if (!manager) {
        fprintf(stderr, "[DnfEventLog_GetCountByTimeRange] Invalid manager\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM dnf_event_log WHERE occ_time >= %d AND occ_time <= %d",
        start_time, end_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[DnfEventLog_GetCountByTimeRange] Query failed\n");
        return -1;
    }

    if (result.row_count == 0 || DBQueryResult_FetchRow(&result, fields) < 1) {
        DBQueryResult_Free(&result);
        return 0;
    }

    count = fields[0] ? atoi(fields[0]) : 0;
    DBQueryResult_Free(&result);
    return count;
}

int DnfEventLog_GetCountByEventType(DBConnectionManager* manager, unsigned char event_type) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* fields[MAX_FIELDS];
    int count;

    if (!manager) {
        fprintf(stderr, "[DnfEventLog_GetCountByEventType] Invalid manager\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM dnf_event_log WHERE event_type = %u", event_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[DnfEventLog_GetCountByEventType] Query failed\n");
        return -1;
    }

    if (result.row_count == 0 || DBQueryResult_FetchRow(&result, fields) < 1) {
        DBQueryResult_Free(&result);
        return 0;
    }

    count = fields[0] ? atoi(fields[0]) : 0;
    DBQueryResult_Free(&result);
    return count;
}

int DnfEventLog_GetCountByServer(DBConnectionManager* manager, unsigned char server_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* fields[MAX_FIELDS];
    int count;

    if (!manager) {
        fprintf(stderr, "[DnfEventLog_GetCountByServer] Invalid manager\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM dnf_event_log WHERE server_id = %u", server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[DnfEventLog_GetCountByServer] Query failed\n");
        return -1;
    }

    if (result.row_count == 0 || DBQueryResult_FetchRow(&result, fields) < 1) {
        DBQueryResult_Free(&result);
        return 0;
    }

    count = fields[0] ? atoi(fields[0]) : 0;
    DBQueryResult_Free(&result);
    return count;
}

int DnfEventLog_GetCountByAccount(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* fields[MAX_FIELDS];
    int count;

    if (!manager) {
        fprintf(stderr, "[DnfEventLog_GetCountByAccount] Invalid manager\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM dnf_event_log WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[DnfEventLog_GetCountByAccount] Query failed\n");
        return -1;
    }

    if (result.row_count == 0 || DBQueryResult_FetchRow(&result, fields) < 1) {
        DBQueryResult_Free(&result);
        return 0;
    }

    count = fields[0] ? atoi(fields[0]) : 0;
    DBQueryResult_Free(&result);
    return count;
}

int DnfEventLog_DeleteOldLogs(DBConnectionManager* manager, int before_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    int affected_rows;

    if (!manager) {
        fprintf(stderr, "[DnfEventLog_DeleteOldLogs] Invalid manager\n");
        return -1;
    }

    /* 先获取记录数 */
    affected_rows = DnfEventLog_GetCountByTimeRange(manager, 0, before_time);
    if (affected_rows < 0) {
        return -1;
    }

    if (affected_rows == 0) {
        return 0;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_event_log WHERE occ_time < %d", before_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[DnfEventLog_DeleteOldLogs] Query failed\n");
        return -1;
    }

    DBQueryResult_Free(&result);
    return affected_rows;
}

int DnfEventLog_DeleteByEventTypeAndTime(DBConnectionManager* manager, unsigned char event_type,
                                         int start_time, int end_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    int affected_rows;

    if (!manager) {
        fprintf(stderr, "[DnfEventLog_DeleteByEventTypeAndTime] Invalid manager\n");
        return -1;
    }

    /* 先获取记录数 */
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM dnf_event_log "
        "WHERE event_type = %u AND occ_time >= %d AND occ_time <= %d",
        event_type, start_time, end_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[DnfEventLog_DeleteByEventTypeAndTime] Count query failed\n");
        return -1;
    }

    char* fields[MAX_FIELDS];
    if (result.row_count == 0 || DBQueryResult_FetchRow(&result, fields) < 1) {
        DBQueryResult_Free(&result);
        return 0;
    }

    affected_rows = fields[0] ? atoi(fields[0]) : 0;
    DBQueryResult_Free(&result);

    if (affected_rows == 0) {
        return 0;
    }

    /* 执行删除 */
    snprintf(query, sizeof(query),
        "DELETE FROM dnf_event_log "
        "WHERE event_type = %u AND occ_time >= %d AND occ_time <= %d",
        event_type, start_time, end_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[DnfEventLog_DeleteByEventTypeAndTime] Delete query failed\n");
        return -1;
    }

    DBQueryResult_Free(&result);
    return affected_rows;
}

int DnfEventLog_GetLatestLogIds(DBConnectionManager* manager, int limit, unsigned int* log_ids) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* fields[MAX_FIELDS];
    int count = 0;

    if (!manager || !log_ids || limit <= 0) {
        fprintf(stderr, "[DnfEventLog_GetLatestLogIds] Invalid parameters\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT log_id FROM dnf_event_log ORDER BY log_id DESC LIMIT %d", limit);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[DnfEventLog_GetLatestLogIds] Query failed\n");
        return -1;
    }

    while (count < limit && DBQueryResult_FetchRow(&result, fields) > 0) {
        log_ids[count++] = fields[0] ? (unsigned int)atoi(fields[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ==================== 工具函数实现 ==================== */

void DnfEventLog_PrintInfo(const DnfEventLog* record) {
    if (!record) {
        printf("DnfEventLog: NULL\n");
        return;
    }

    printf("========================================\n");
    printf("DNF Event Log Information:\n");
    printf("========================================\n");
    printf("Log ID:          %u\n", record->log_id);
    printf("Occur Time:      %d\n", record->occ_time);
    printf("Event Type:      %u\n", record->event_type);
    printf("Parameter 1:     %u\n", record->parameter1);
    printf("Parameter 2:     %u\n", record->parameter2);
    printf("Server ID:       %u\n", record->server_id);
    printf("Event Flag:      %d\n", record->event_flag);
    printf("Start Time:      %d\n", record->start_time);
    printf("End Time:        %d\n", record->end_time);
    printf("Account ID:      %d\n", record->m_id);
    printf("Explanation:     %s\n", record->expl);
    printf("Etc:             %s\n", record->etc);
    printf("========================================\n");
}
