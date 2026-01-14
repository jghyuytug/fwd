#include "member_premium_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/* ======================================== CRUD Operations ======================================== */

int MemberPremiumHistory_Add(DBConnectionManager* manager, const MemberPremiumHistory* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "REPLACE INTO member_premium_history (event_id, pre_type, m_id, service_start, service_end) "
        "VALUES (%d, %u, %d, '%s', '%s')",
        record->event_id, record->pre_type, record->m_id,
        record->service_start, record->service_end);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPremiumHistory_Get(DBConnectionManager* manager, int event_id, int pre_type,
                              int m_id, const char* service_start, MemberPremiumHistory* record) {
    if (!manager || !service_start || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, pre_type, m_id, service_start, service_end "
        "FROM member_premium_history "
        "WHERE event_id = %d AND pre_type = %d AND m_id = %d AND service_start = '%s'",
        event_id, pre_type, m_id, service_start);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    int ret = -1;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->event_id = atoi(values[0]);
        record->pre_type = (unsigned char)atoi(values[1]);
        record->m_id = atoi(values[2]);
        strncpy(record->service_start, values[3], sizeof(record->service_start) - 1);
        record->service_start[sizeof(record->service_start) - 1] = '\0';
        strncpy(record->service_end, values[4], sizeof(record->service_end) - 1);
        record->service_end[sizeof(record->service_end) - 1] = '\0';
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int MemberPremiumHistory_Update(DBConnectionManager* manager, const MemberPremiumHistory* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_premium_history SET service_end = '%s' "
        "WHERE event_id = %d AND pre_type = %u AND m_id = %d AND service_start = '%s'",
        record->service_end,
        record->event_id, record->pre_type, record->m_id, record->service_start);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPremiumHistory_Delete(DBConnectionManager* manager, int event_id, int pre_type,
                                 int m_id, const char* service_start) {
    if (!manager || !service_start) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "DELETE FROM member_premium_history "
        "WHERE event_id = %d AND pre_type = %d AND m_id = %d AND service_start = '%s'",
        event_id, pre_type, m_id, service_start);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPremiumHistory_Exists(DBConnectionManager* manager, int event_id, int pre_type,
                                 int m_id, const char* service_start) {
    if (!manager || !service_start) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_premium_history "
        "WHERE event_id = %d AND pre_type = %d AND m_id = %d AND service_start = '%s'",
        event_id, pre_type, m_id, service_start);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    char* values[MAX_FIELDS];
    int exists = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = (atoi(values[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* ======================================== Business Queries ======================================== */

int MemberPremiumHistory_GetByEventId(DBConnectionManager* manager, int event_id,
                                       MemberPremiumHistory* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, pre_type, m_id, service_start, service_end "
        "FROM member_premium_history WHERE event_id = %d LIMIT %d",
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
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberPremiumHistory_GetByMemberId(DBConnectionManager* manager, int m_id,
                                        MemberPremiumHistory* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, pre_type, m_id, service_start, service_end "
        "FROM member_premium_history WHERE m_id = %d "
        "ORDER BY service_start DESC LIMIT %d",
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
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberPremiumHistory_GetByPreType(DBConnectionManager* manager, int pre_type,
                                       MemberPremiumHistory* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, pre_type, m_id, service_start, service_end "
        "FROM member_premium_history WHERE pre_type = %d LIMIT %d",
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
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberPremiumHistory_GetActiveServices(DBConnectionManager* manager, MemberPremiumHistory* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, pre_type, m_id, service_start, service_end "
        "FROM member_premium_history "
        "WHERE NOW() BETWEEN service_start AND service_end "
        "LIMIT %d",
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
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberPremiumHistory_GetByTimeRange(DBConnectionManager* manager, const char* start_time,
                                         const char* end_time, MemberPremiumHistory* records, int max_records) {
    if (!manager || !start_time || !end_time || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, pre_type, m_id, service_start, service_end "
        "FROM member_premium_history "
        "WHERE service_start BETWEEN '%s' AND '%s' "
        "LIMIT %d",
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
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberPremiumHistory_GetExpiringSoon(DBConnectionManager* manager, int days,
                                          MemberPremiumHistory* records, int max_records) {
    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, pre_type, m_id, service_start, service_end "
        "FROM member_premium_history "
        "WHERE service_end BETWEEN NOW() AND DATE_ADD(NOW(), INTERVAL %d DAY) "
        "ORDER BY service_end ASC "
        "LIMIT %d",
        days, max_records);

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
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberPremiumHistory_GetAll(DBConnectionManager* manager, MemberPremiumHistory* records,
                                 int offset, int limit) {
    if (!manager || !records || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT event_id, pre_type, m_id, service_start, service_end "
        "FROM member_premium_history LIMIT %d, %d",
        offset, limit);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < limit && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].event_id = atoi(values[0]);
        records[count].pre_type = (unsigned char)atoi(values[1]);
        records[count].m_id = atoi(values[2]);
        strncpy(records[count].service_start, values[3], sizeof(records[count].service_start) - 1);
        records[count].service_start[sizeof(records[count].service_start) - 1] = '\0';
        strncpy(records[count].service_end, values[4], sizeof(records[count].service_end) - 1);
        records[count].service_end[sizeof(records[count].service_end) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberPremiumHistory_GetCount(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM member_premium_history");

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ======================================== Bulk Operations ======================================== */

int MemberPremiumHistory_BulkAdd(DBConnectionManager* manager, const MemberPremiumHistory* records, int count) {
    if (!manager || !records || count <= 0) {
        return -1;
    }

    int success_count = 0;
    int i;

    for (i = 0; i < count; i++) {
        if (MemberPremiumHistory_Add(manager, &records[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

int MemberPremiumHistory_BulkDeleteByEventIds(DBConnectionManager* manager, const int* event_ids, int count) {
    if (!manager || !event_ids || count <= 0) {
        return -1;
    }

    int i;
    for (i = 0; i < count; i++) {
        char query[MAX_QUERY_LEN];
        snprintf(query, sizeof(query),
            "DELETE FROM member_premium_history WHERE event_id = %d", event_ids[i]);

        DBQueryResult result;
        memset(&result, 0, sizeof(DBQueryResult));

        if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
            DBQueryResult_Free(&result);
            return -1;
        }

        DBQueryResult_Free(&result);
    }

    return 0;
}

/* ======================================== Utility Functions ======================================== */

void MemberPremiumHistory_PrintInfo(const MemberPremiumHistory* record) {
    if (!record) {
        printf("MemberPremiumHistory is NULL\n");
        return;
    }

    printf("=== Member Premium History Info ===\n");
    printf("  event_id:      %d\n", record->event_id);
    printf("  pre_type:      %u\n", record->pre_type);
    printf("  m_id:          %d\n", record->m_id);
    printf("  service_start: %s\n", record->service_start);
    printf("  service_end:   %s\n", record->service_end);
    printf("===================================\n");
}
