#include "member_punish_hack_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MemberPunishHack_Add(DBConnectionManager* manager, const MemberPunishHack* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_punish_hack (m_id, occ_time, period, now_flag, auto_flag, reason, hack_ip) "
        "VALUES (%d, %d, %d, %d, %d, '%s', '%s')",
        record->m_id, record->occ_time, record->period, record->now_flag, record->auto_flag, record->reason, record->hack_ip);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishHack_Get(DBConnectionManager* manager, int m_id, MemberPunishHack* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, period, now_flag, auto_flag, reason, hack_ip FROM member_punish_hack WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MemberPunishHack));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->occ_time = row[1] ? atoi(row[1]) : 0;
    record->period = row[2] ? atoi(row[2]) : 0;
    record->now_flag = row[3] ? atoi(row[3]) : 0;
    record->auto_flag = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->reason, row[5], sizeof(record->reason) - 1);
    if (row[6]) strncpy(record->hack_ip, row[6], sizeof(record->hack_ip) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishHack_GetAll(DBConnectionManager* manager, MemberPunishHack* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, period, now_flag, auto_flag, reason, hack_ip FROM member_punish_hack");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberPunishHack));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].occ_time = row[1] ? atoi(row[1]) : 0;
        records[count].period = row[2] ? atoi(row[2]) : 0;
        records[count].now_flag = row[3] ? atoi(row[3]) : 0;
        records[count].auto_flag = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].reason, row[5], sizeof(records[count].reason) - 1);
        if (row[6]) strncpy(records[count].hack_ip, row[6], sizeof(records[count].hack_ip) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
