#include "member_punish_hack_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MemberPunishHackHistory_Add(DBConnectionManager* manager, const MemberPunishHackHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_punish_hack_history (m_id, occ_time, period, now_flag, auto_flag, reason) "
        "VALUES (%d, %d, %d, %d, %d, '%s')",
        record->m_id, record->occ_time, record->period, record->now_flag, record->auto_flag, record->reason);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishHackHistory_GetAll(DBConnectionManager* manager, MemberPunishHackHistory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, period, now_flag, auto_flag, reason FROM member_punish_hack_history");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberPunishHackHistory));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].occ_time = row[1] ? atoi(row[1]) : 0;
        records[count].period = row[2] ? atoi(row[2]) : 0;
        records[count].now_flag = row[3] ? atoi(row[3]) : 0;
        records[count].auto_flag = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].reason, row[5], sizeof(records[count].reason) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
