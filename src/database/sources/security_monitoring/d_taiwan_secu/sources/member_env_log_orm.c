#include "member_env_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MemberEnvLog_Add(DBConnectionManager* manager, const MemberEnvLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_env_log (m_id, mac_addr, log, occ_time) "
        "VALUES (%d, '%s', '%s', '%s')",
        record->m_id, record->mac_addr, record->log, record->occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_SECU, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberEnvLog_GetAll(DBConnectionManager* manager, MemberEnvLog* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, mac_addr, log, occ_time FROM member_env_log");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_SECU, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberEnvLog));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].mac_addr, row[1], sizeof(records[count].mac_addr) - 1);
        if (row[2]) strncpy(records[count].log, row[2], sizeof(records[count].log) - 1);
        if (row[3]) strncpy(records[count].occ_time, row[3], sizeof(records[count].occ_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
