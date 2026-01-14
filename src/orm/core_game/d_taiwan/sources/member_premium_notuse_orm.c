/**
 * member_premium_notuse_orm.c
 *
 * ORM implementation for member_premium_notuse table
 * 会员高级服务未使用记录表 - 实现高级服务未使用记录的数据库操作
 *
 * Table: member_premium_notuse (6 fields)
 * Primary Key: (event_id, pre_type, server_id, m_id, service_start) - 复合主键
 *
 * Generated: 2025-11-14
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "member_premium_notuse_orm.h"

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 6

/**
 * Add a new member_premium_notuse record (idempotent with REPLACE INTO)
 */
int MemberPremiumNotuse_Add(DBConnectionManager* manager, const MemberPremiumNotuse* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "REPLACE INTO member_premium_notuse (event_id, pre_type, m_id, service_start, service_end, server_id) "
        "VALUES (%d, %u, %d, '%s', '%s', %u)",
        record->event_id, record->pre_type, record->m_id, record->service_start,
        record->service_end, record->server_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * Get a member_premium_notuse record by composite primary key
 */
int MemberPremiumNotuse_Get(DBConnectionManager* manager,
                             int event_id, unsigned char pre_type, unsigned char server_id,
                             int m_id, const char* service_start,
                             MemberPremiumNotuse* record) {
    if (!manager || !service_start || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, pre_type, m_id, service_start, service_end, server_id "
        "FROM member_premium_notuse "
        "WHERE event_id = %d AND pre_type = %u AND server_id = %u AND m_id = %d AND service_start = '%s'",
        event_id, pre_type, server_id, m_id, service_start);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->event_id = atoi(values[0]);
        record->pre_type = (unsigned char)atoi(values[1]);
        record->m_id = atoi(values[2]);
        strncpy(record->service_start, values[3], sizeof(record->service_start) - 1);
        record->service_start[sizeof(record->service_start) - 1] = '\0';
        strncpy(record->service_end, values[4], sizeof(record->service_end) - 1);
        record->service_end[sizeof(record->service_end) - 1] = '\0';
        record->server_id = (unsigned char)atoi(values[5]);

        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

/**
 * Update an existing member_premium_notuse record
 */
int MemberPremiumNotuse_Update(DBConnectionManager* manager, const MemberPremiumNotuse* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_premium_notuse SET service_end = '%s' "
        "WHERE event_id = %d AND pre_type = %u AND server_id = %u AND m_id = %d AND service_start = '%s'",
        record->service_end, record->event_id, record->pre_type, record->server_id,
        record->m_id, record->service_start);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * Delete a member_premium_notuse record by composite primary key (idempotent)
 */
int MemberPremiumNotuse_Delete(DBConnectionManager* manager,
                                 int event_id, unsigned char pre_type, unsigned char server_id,
                                 int m_id, const char* service_start) {
    if (!manager || !service_start) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "DELETE FROM member_premium_notuse "
        "WHERE event_id = %d AND pre_type = %u AND server_id = %u AND m_id = %d AND service_start = '%s'",
        event_id, pre_type, server_id, m_id, service_start);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * Check if a member_premium_notuse record exists
 */
int MemberPremiumNotuse_Exists(DBConnectionManager* manager,
                                 int event_id, unsigned char pre_type, unsigned char server_id,
                                 int m_id, const char* service_start) {
    if (!manager || !service_start) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_premium_notuse "
        "WHERE event_id = %d AND pre_type = %u AND server_id = %u AND m_id = %d AND service_start = '%s'",
        event_id, pre_type, server_id, m_id, service_start);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    int exists = 0;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = (atoi(values[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/**
 * Get records by event_id
 */
int MemberPremiumNotuse_GetByEventId(DBConnectionManager* manager,
                                       int event_id,
                                       MemberPremiumNotuse* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, pre_type, m_id, service_start, service_end, server_id "
        "FROM member_premium_notuse "
        "WHERE event_id = %d LIMIT %d",
        event_id, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];
    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].event_id = atoi(values[0]);
        records[count].pre_type = (unsigned char)atoi(values[1]);
        records[count].m_id = atoi(values[2]);
        strncpy(records[count].service_start, values[3], sizeof(records[count].service_start) - 1);
        records[count].service_start[sizeof(records[count].service_start) - 1] = '\0';
        strncpy(records[count].service_end, values[4], sizeof(records[count].service_end) - 1);
        records[count].service_end[sizeof(records[count].service_end) - 1] = '\0';
        records[count].server_id = (unsigned char)atoi(values[5]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * Get records by member_id
 */
int MemberPremiumNotuse_GetByMemberId(DBConnectionManager* manager,
                                        int m_id,
                                        MemberPremiumNotuse* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, pre_type, m_id, service_start, service_end, server_id "
        "FROM member_premium_notuse "
        "WHERE m_id = %d LIMIT %d",
        m_id, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];
    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].event_id = atoi(values[0]);
        records[count].pre_type = (unsigned char)atoi(values[1]);
        records[count].m_id = atoi(values[2]);
        strncpy(records[count].service_start, values[3], sizeof(records[count].service_start) - 1);
        records[count].service_start[sizeof(records[count].service_start) - 1] = '\0';
        strncpy(records[count].service_end, values[4], sizeof(records[count].service_end) - 1);
        records[count].service_end[sizeof(records[count].service_end) - 1] = '\0';
        records[count].server_id = (unsigned char)atoi(values[5]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * Get records by pre_type
 */
int MemberPremiumNotuse_GetByPreType(DBConnectionManager* manager,
                                       unsigned char pre_type,
                                       MemberPremiumNotuse* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, pre_type, m_id, service_start, service_end, server_id "
        "FROM member_premium_notuse "
        "WHERE pre_type = %u LIMIT %d",
        pre_type, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];
    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].event_id = atoi(values[0]);
        records[count].pre_type = (unsigned char)atoi(values[1]);
        records[count].m_id = atoi(values[2]);
        strncpy(records[count].service_start, values[3], sizeof(records[count].service_start) - 1);
        records[count].service_start[sizeof(records[count].service_start) - 1] = '\0';
        strncpy(records[count].service_end, values[4], sizeof(records[count].service_end) - 1);
        records[count].service_end[sizeof(records[count].service_end) - 1] = '\0';
        records[count].server_id = (unsigned char)atoi(values[5]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * Get records by server_id
 */
int MemberPremiumNotuse_GetByServerId(DBConnectionManager* manager,
                                        unsigned char server_id,
                                        MemberPremiumNotuse* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, pre_type, m_id, service_start, service_end, server_id "
        "FROM member_premium_notuse "
        "WHERE server_id = %u LIMIT %d",
        server_id, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];
    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].event_id = atoi(values[0]);
        records[count].pre_type = (unsigned char)atoi(values[1]);
        records[count].m_id = atoi(values[2]);
        strncpy(records[count].service_start, values[3], sizeof(records[count].service_start) - 1);
        records[count].service_start[sizeof(records[count].service_start) - 1] = '\0';
        strncpy(records[count].service_end, values[4], sizeof(records[count].service_end) - 1);
        records[count].service_end[sizeof(records[count].service_end) - 1] = '\0';
        records[count].server_id = (unsigned char)atoi(values[5]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * Get records by service_start time range
 */
int MemberPremiumNotuse_GetByServiceStartRange(DBConnectionManager* manager,
                                                 const char* start_time, const char* end_time,
                                                 MemberPremiumNotuse* records, int max_records) {
    if (!manager || !start_time || !end_time || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, pre_type, m_id, service_start, service_end, server_id "
        "FROM member_premium_notuse "
        "WHERE service_start BETWEEN '%s' AND '%s' ORDER BY service_start DESC LIMIT %d",
        start_time, end_time, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];
    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].event_id = atoi(values[0]);
        records[count].pre_type = (unsigned char)atoi(values[1]);
        records[count].m_id = atoi(values[2]);
        strncpy(records[count].service_start, values[3], sizeof(records[count].service_start) - 1);
        records[count].service_start[sizeof(records[count].service_start) - 1] = '\0';
        strncpy(records[count].service_end, values[4], sizeof(records[count].service_end) - 1);
        records[count].service_end[sizeof(records[count].service_end) - 1] = '\0';
        records[count].server_id = (unsigned char)atoi(values[5]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * Get records by service_end time range
 */
int MemberPremiumNotuse_GetByServiceEndRange(DBConnectionManager* manager,
                                               const char* start_time, const char* end_time,
                                               MemberPremiumNotuse* records, int max_records) {
    if (!manager || !start_time || !end_time || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, pre_type, m_id, service_start, service_end, server_id "
        "FROM member_premium_notuse "
        "WHERE service_end BETWEEN '%s' AND '%s' ORDER BY service_end DESC LIMIT %d",
        start_time, end_time, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];
    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].event_id = atoi(values[0]);
        records[count].pre_type = (unsigned char)atoi(values[1]);
        records[count].m_id = atoi(values[2]);
        strncpy(records[count].service_start, values[3], sizeof(records[count].service_start) - 1);
        records[count].service_start[sizeof(records[count].service_start) - 1] = '\0';
        strncpy(records[count].service_end, values[4], sizeof(records[count].service_end) - 1);
        records[count].service_end[sizeof(records[count].service_end) - 1] = '\0';
        records[count].server_id = (unsigned char)atoi(values[5]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * Get active services (service_end > NOW())
 */
int MemberPremiumNotuse_GetActiveServices(DBConnectionManager* manager,
                                            MemberPremiumNotuse* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, pre_type, m_id, service_start, service_end, server_id "
        "FROM member_premium_notuse "
        "WHERE service_end > NOW() LIMIT %d",
        max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];
    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].event_id = atoi(values[0]);
        records[count].pre_type = (unsigned char)atoi(values[1]);
        records[count].m_id = atoi(values[2]);
        strncpy(records[count].service_start, values[3], sizeof(records[count].service_start) - 1);
        records[count].service_start[sizeof(records[count].service_start) - 1] = '\0';
        strncpy(records[count].service_end, values[4], sizeof(records[count].service_end) - 1);
        records[count].service_end[sizeof(records[count].service_end) - 1] = '\0';
        records[count].server_id = (unsigned char)atoi(values[5]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * Get expired services (service_end < NOW())
 */
int MemberPremiumNotuse_GetExpiredServices(DBConnectionManager* manager,
                                             MemberPremiumNotuse* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, pre_type, m_id, service_start, service_end, server_id "
        "FROM member_premium_notuse "
        "WHERE service_end < NOW() LIMIT %d",
        max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];
    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].event_id = atoi(values[0]);
        records[count].pre_type = (unsigned char)atoi(values[1]);
        records[count].m_id = atoi(values[2]);
        strncpy(records[count].service_start, values[3], sizeof(records[count].service_start) - 1);
        records[count].service_start[sizeof(records[count].service_start) - 1] = '\0';
        strncpy(records[count].service_end, values[4], sizeof(records[count].service_end) - 1);
        records[count].service_end[sizeof(records[count].service_end) - 1] = '\0';
        records[count].server_id = (unsigned char)atoi(values[5]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * Get records by event_id and m_id
 */
int MemberPremiumNotuse_GetByEventIdAndMemberId(DBConnectionManager* manager,
                                                  int event_id, int m_id,
                                                  MemberPremiumNotuse* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, pre_type, m_id, service_start, service_end, server_id "
        "FROM member_premium_notuse "
        "WHERE event_id = %d AND m_id = %d LIMIT %d",
        event_id, m_id, max_records);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];
    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].event_id = atoi(values[0]);
        records[count].pre_type = (unsigned char)atoi(values[1]);
        records[count].m_id = atoi(values[2]);
        strncpy(records[count].service_start, values[3], sizeof(records[count].service_start) - 1);
        records[count].service_start[sizeof(records[count].service_start) - 1] = '\0';
        strncpy(records[count].service_end, values[4], sizeof(records[count].service_end) - 1);
        records[count].service_end[sizeof(records[count].service_end) - 1] = '\0';
        records[count].server_id = (unsigned char)atoi(values[5]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * Bulk add member_premium_notuse records
 */
int MemberPremiumNotuse_BulkAdd(DBConnectionManager* manager,
                                  const MemberPremiumNotuse* records, int count) {
    if (!manager || !records || count <= 0) {
        return -1;
    }

    int i;
    int success_count = 0;
    for (i = 0; i < count; i++) {
        if (MemberPremiumNotuse_Add(manager, &records[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

/**
 * Bulk delete member_premium_notuse records by member_id
 */
int MemberPremiumNotuse_BulkDeleteByMemberId(DBConnectionManager* manager,
                                               const int* m_ids, int count) {
    if (!manager || !m_ids || count <= 0) {
        return -1;
    }

    int i;
    for (i = 0; i < count; i++) {
        char query[MAX_QUERY_LEN];
        snprintf(query, sizeof(query),
            "DELETE FROM member_premium_notuse WHERE m_id = %d", m_ids[i]);

        DBQueryResult result;
        memset(&result, 0, sizeof(DBQueryResult));

        DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);
        DBQueryResult_Free(&result);
    }

    return 0;
}

/**
 * Print member_premium_notuse record information
 */
void MemberPremiumNotuse_PrintInfo(const MemberPremiumNotuse* record) {
    if (!record) {
        printf("MemberPremiumNotuse: NULL\n");
        return;
    }

    printf("=== MemberPremiumNotuse Record ===\n");
    printf("  event_id: %d\n", record->event_id);
    printf("  pre_type: %u\n", record->pre_type);
    printf("  m_id: %d\n", record->m_id);
    printf("  service_start: %s\n", record->service_start);
    printf("  service_end: %s\n", record->service_end);
    printf("  server_id: %u\n", record->server_id);
    printf("==================================\n");
}
