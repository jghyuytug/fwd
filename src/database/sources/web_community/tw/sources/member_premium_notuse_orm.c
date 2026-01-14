#include "member_premium_notuse_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MemberPremiumNotuse_Add(DBConnectionManager* manager, const MemberPremiumNotuse* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_premium_notuse (event_id, pre_type, m_id, service_start, service_end, server_id) "
        "VALUES (%d, %d, %d, '%s', '%s', %d)",
        record->event_id, record->pre_type, record->m_id, record->service_start, record->service_end, record->server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPremiumNotuse_Get(DBConnectionManager* manager, int event_id, signed char pre_type, signed char server_id, int m_id, const char* service_start, MemberPremiumNotuse* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, pre_type, m_id, service_start, service_end, server_id FROM member_premium_notuse WHERE event_id = %d AND pre_type = %d AND server_id = %d AND m_id = %d AND service_start = '%s'",
        event_id, pre_type, server_id, m_id, service_start);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MemberPremiumNotuse));
    record->event_id = row[0] ? atoi(row[0]) : 0;
    record->pre_type = row[1] ? atoi(row[1]) : 0;
    record->m_id = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->service_start, row[3], sizeof(record->service_start) - 1);
    if (row[4]) strncpy(record->service_end, row[4], sizeof(record->service_end) - 1);
    record->server_id = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPremiumNotuse_GetAll(DBConnectionManager* manager, MemberPremiumNotuse* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, pre_type, m_id, service_start, service_end, server_id FROM member_premium_notuse");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberPremiumNotuse));
        records[count].event_id = row[0] ? atoi(row[0]) : 0;
        records[count].pre_type = row[1] ? atoi(row[1]) : 0;
        records[count].m_id = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].service_start, row[3], sizeof(records[count].service_start) - 1);
        if (row[4]) strncpy(records[count].service_end, row[4], sizeof(records[count].service_end) - 1);
        records[count].server_id = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
