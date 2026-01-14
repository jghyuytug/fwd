#include "reliable_member_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ReliableMemberLog_Add(DBConnectionManager* manager, const ReliableMemberLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO reliable_member_log (occ_date, m_id, ip) "
        "VALUES ('%s', %d, '%s')",
        record->occ_date, record->m_id, record->ip);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ReliableMemberLog_Get(DBConnectionManager* manager, const char* occ_date, int m_id, const char* ip, ReliableMemberLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, m_id, ip FROM reliable_member_log WHERE occ_date = '%s' AND m_id = %d AND ip = '%s'",
        occ_date, m_id, ip);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ReliableMemberLog));
    if (row[0]) strncpy(record->occ_date, row[0], sizeof(record->occ_date) - 1);
    record->m_id = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->ip, row[2], sizeof(record->ip) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int ReliableMemberLog_GetAll(DBConnectionManager* manager, ReliableMemberLog* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, m_id, ip FROM reliable_member_log");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ReliableMemberLog));
        if (row[0]) strncpy(records[count].occ_date, row[0], sizeof(records[count].occ_date) - 1);
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].ip, row[2], sizeof(records[count].ip) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
