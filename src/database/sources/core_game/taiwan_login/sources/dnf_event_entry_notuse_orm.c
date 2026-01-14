#include "dnf_event_entry_notuse_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 16

/* ==================== CRUD 操作实现 ==================== */

int DnfEventEntryNotuse_Add(DBConnectionManager* manager, const DnfEventEntryNotuse* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        fprintf(stderr, "[DnfEventEntryNotuse_Add] Invalid parameters\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO dnf_event_entry_notuse "
        "(event_id, m_id, occ_date, server_id, charac_no, obtain_date) "
        "VALUES (%d, %d, '%s', %d, %d, '%s')",
        record->event_id,
        record->m_id,
        record->occ_date,
        record->server_id,
        record->charac_no,
        record->obtain_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[DnfEventEntryNotuse_Add] Query failed: %s\n", query);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventEntryNotuse_Get(DBConnectionManager* manager, int event_id, int m_id,
                            DnfEventEntryNotuse* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* fields[MAX_FIELDS];
    int field_count;

    if (!manager || !record) {
        fprintf(stderr, "[DnfEventEntryNotuse_Get] Invalid parameters\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT event_id, m_id, occ_date, server_id, charac_no, obtain_date "
        "FROM dnf_event_entry_notuse "
        "WHERE event_id = %d AND m_id = %d",
        event_id, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[DnfEventEntryNotuse_Get] Query failed\n");
        return -1;
    }

    if (result.row_count == 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    field_count = DBQueryResult_FetchRow(&result, fields);
    if (field_count < 6) {
        fprintf(stderr, "[DnfEventEntryNotuse_Get] Incomplete data\n");
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 解析字段 */
    record->event_id = fields[0] ? atoi(fields[0]) : 0;
    record->m_id = fields[1] ? atoi(fields[1]) : 0;
    strncpy(record->occ_date, fields[2] ? fields[2] : "", sizeof(record->occ_date) - 1);
    record->occ_date[sizeof(record->occ_date) - 1] = '\0';
    record->server_id = fields[3] ? (signed char)atoi(fields[3]) : 0;
    record->charac_no = fields[4] ? atoi(fields[4]) : 0;
    strncpy(record->obtain_date, fields[5] ? fields[5] : "", sizeof(record->obtain_date) - 1);
    record->obtain_date[sizeof(record->obtain_date) - 1] = '\0';

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventEntryNotuse_Update(DBConnectionManager* manager, const DnfEventEntryNotuse* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        fprintf(stderr, "[DnfEventEntryNotuse_Update] Invalid parameters\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE dnf_event_entry_notuse "
        "SET occ_date = '%s', server_id = %d, charac_no = %d, obtain_date = '%s' "
        "WHERE event_id = %d AND m_id = %d",
        record->occ_date,
        record->server_id,
        record->charac_no,
        record->obtain_date,
        record->event_id,
        record->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[DnfEventEntryNotuse_Update] Query failed\n");
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventEntryNotuse_Delete(DBConnectionManager* manager, int event_id, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        fprintf(stderr, "[DnfEventEntryNotuse_Delete] Invalid manager\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_event_entry_notuse WHERE event_id = %d AND m_id = %d",
        event_id, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[DnfEventEntryNotuse_Delete] Query failed\n");
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventEntryNotuse_Exists(DBConnectionManager* manager, int event_id, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    int exists;

    if (!manager) {
        fprintf(stderr, "[DnfEventEntryNotuse_Exists] Invalid manager\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT 1 FROM dnf_event_entry_notuse WHERE event_id = %d AND m_id = %d LIMIT 1",
        event_id, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[DnfEventEntryNotuse_Exists] Query failed\n");
        return -1;
    }

    exists = (result.row_count > 0) ? 1 : 0;
    DBQueryResult_Free(&result);
    return exists;
}

/* ==================== 业务操作实现 ==================== */

int DnfEventEntryNotuse_GetCountByAccount(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* fields[MAX_FIELDS];
    int count;

    if (!manager) {
        fprintf(stderr, "[DnfEventEntryNotuse_GetCountByAccount] Invalid manager\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM dnf_event_entry_notuse WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[DnfEventEntryNotuse_GetCountByAccount] Query failed\n");
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

int DnfEventEntryNotuse_GetCountByEvent(DBConnectionManager* manager, int event_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* fields[MAX_FIELDS];
    int count;

    if (!manager) {
        fprintf(stderr, "[DnfEventEntryNotuse_GetCountByEvent] Invalid manager\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM dnf_event_entry_notuse WHERE event_id = %d",
        event_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[DnfEventEntryNotuse_GetCountByEvent] Query failed\n");
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

int DnfEventEntryNotuse_GetCountByCharacter(DBConnectionManager* manager, int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* fields[MAX_FIELDS];
    int count;

    if (!manager) {
        fprintf(stderr, "[DnfEventEntryNotuse_GetCountByCharacter] Invalid manager\n");
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM dnf_event_entry_notuse WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[DnfEventEntryNotuse_GetCountByCharacter] Query failed\n");
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

int DnfEventEntryNotuse_HasAccountJoinedEvent(DBConnectionManager* manager, int event_id, int m_id) {
    /* 直接使用 Exists 函数 */
    return DnfEventEntryNotuse_Exists(manager, event_id, m_id);
}

int DnfEventEntryNotuse_DeleteByEvent(DBConnectionManager* manager, int event_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    int affected_rows;

    if (!manager) {
        fprintf(stderr, "[DnfEventEntryNotuse_DeleteByEvent] Invalid manager\n");
        return -1;
    }

    /* 先获取记录数 */
    affected_rows = DnfEventEntryNotuse_GetCountByEvent(manager, event_id);
    if (affected_rows < 0) {
        return -1;
    }

    if (affected_rows == 0) {
        return 0;  /* 没有记录需要删除 */
    }

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_event_entry_notuse WHERE event_id = %d",
        event_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[DnfEventEntryNotuse_DeleteByEvent] Query failed\n");
        return -1;
    }

    DBQueryResult_Free(&result);
    return affected_rows;
}

int DnfEventEntryNotuse_DeleteByAccount(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    int affected_rows;

    if (!manager) {
        fprintf(stderr, "[DnfEventEntryNotuse_DeleteByAccount] Invalid manager\n");
        return -1;
    }

    /* 先获取记录数 */
    affected_rows = DnfEventEntryNotuse_GetCountByAccount(manager, m_id);
    if (affected_rows < 0) {
        return -1;
    }

    if (affected_rows == 0) {
        return 0;  /* 没有记录需要删除 */
    }

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_event_entry_notuse WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        fprintf(stderr, "[DnfEventEntryNotuse_DeleteByAccount] Query failed\n");
        return -1;
    }

    DBQueryResult_Free(&result);
    return affected_rows;
}

/* ==================== 工具函数实现 ==================== */

void DnfEventEntryNotuse_PrintInfo(const DnfEventEntryNotuse* record) {
    if (!record) {
        printf("DnfEventEntryNotuse: NULL\n");
        return;
    }

    printf("========================================\n");
    printf("DNF Event Entry (Not Use) Information:\n");
    printf("========================================\n");
    printf("Event ID:        %d\n", record->event_id);
    printf("Account ID:      %d\n", record->m_id);
    printf("Occur Date:      %s\n", record->occ_date);
    printf("Server ID:       %d\n", record->server_id);
    printf("Character No:    %d\n", record->charac_no);
    printf("Obtain Date:     %s\n", record->obtain_date);
    printf("========================================\n");
}
