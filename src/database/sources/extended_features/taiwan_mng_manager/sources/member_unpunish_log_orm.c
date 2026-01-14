#include "member_unpunish_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MemberUnpunishLog_Add(DBConnectionManager* manager, const MemberUnpunishLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_unpunish_log (admin_no, admin_id, total_m_id, success_m_id, fail_m_id, reg_date, reason) "
        "VALUES (%d, '%s', '%s', '%s', '%s', '%s', '%s')",
        record->admin_no, record->admin_id, record->total_m_id, record->success_m_id, record->fail_m_id, record->reg_date, record->reason);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberUnpunishLog_Get(DBConnectionManager* manager, int idx, MemberUnpunishLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT idx, admin_no, admin_id, total_m_id, success_m_id, fail_m_id, reg_date, reason FROM member_unpunish_log WHERE idx = %d",
        idx);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MemberUnpunishLog));
    record->idx = row[0] ? atoi(row[0]) : 0;
    record->admin_no = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->admin_id, row[2], sizeof(record->admin_id) - 1);
    if (row[3]) strncpy(record->total_m_id, row[3], sizeof(record->total_m_id) - 1);
    if (row[4]) strncpy(record->success_m_id, row[4], sizeof(record->success_m_id) - 1);
    if (row[5]) strncpy(record->fail_m_id, row[5], sizeof(record->fail_m_id) - 1);
    if (row[6]) strncpy(record->reg_date, row[6], sizeof(record->reg_date) - 1);
    if (row[7]) strncpy(record->reason, row[7], sizeof(record->reason) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int MemberUnpunishLog_GetAll(DBConnectionManager* manager, MemberUnpunishLog* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT idx, admin_no, admin_id, total_m_id, success_m_id, fail_m_id, reg_date, reason FROM member_unpunish_log");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberUnpunishLog));
        records[count].idx = row[0] ? atoi(row[0]) : 0;
        records[count].admin_no = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].admin_id, row[2], sizeof(records[count].admin_id) - 1);
        if (row[3]) strncpy(records[count].total_m_id, row[3], sizeof(records[count].total_m_id) - 1);
        if (row[4]) strncpy(records[count].success_m_id, row[4], sizeof(records[count].success_m_id) - 1);
        if (row[5]) strncpy(records[count].fail_m_id, row[5], sizeof(records[count].fail_m_id) - 1);
        if (row[6]) strncpy(records[count].reg_date, row[6], sizeof(records[count].reg_date) - 1);
        if (row[7]) strncpy(records[count].reason, row[7], sizeof(records[count].reason) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
