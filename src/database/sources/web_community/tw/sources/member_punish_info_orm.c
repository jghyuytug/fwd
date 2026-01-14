#include "member_punish_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MemberPunishInfo_Add(DBConnectionManager* manager, const MemberPunishInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_punish_info (m_id, punish_type, occ_time, punish_value, apply_flag, start_time, end_time, admin_id, reason) "
        "VALUES (%d, %d, '%s', %d, %d, '%s', '%s', '%s', '%s')",
        record->m_id, record->punish_type, record->occ_time, record->punish_value, record->apply_flag, record->start_time, record->end_time, record->admin_id, record->reason);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishInfo_Get(DBConnectionManager* manager, int m_id, int punish_type, MemberPunishInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, punish_type, occ_time, punish_value, apply_flag, start_time, end_time, admin_id, reason FROM member_punish_info WHERE m_id = %d AND punish_type = %d",
        m_id, punish_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MemberPunishInfo));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->punish_type = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->occ_time, row[2], sizeof(record->occ_time) - 1);
    record->punish_value = row[3] ? atoi(row[3]) : 0;
    record->apply_flag = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->start_time, row[5], sizeof(record->start_time) - 1);
    if (row[6]) strncpy(record->end_time, row[6], sizeof(record->end_time) - 1);
    if (row[7]) strncpy(record->admin_id, row[7], sizeof(record->admin_id) - 1);
    if (row[8]) strncpy(record->reason, row[8], sizeof(record->reason) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishInfo_GetAll(DBConnectionManager* manager, MemberPunishInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, punish_type, occ_time, punish_value, apply_flag, start_time, end_time, admin_id, reason FROM member_punish_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberPunishInfo));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].punish_type = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].occ_time, row[2], sizeof(records[count].occ_time) - 1);
        records[count].punish_value = row[3] ? atoi(row[3]) : 0;
        records[count].apply_flag = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].start_time, row[5], sizeof(records[count].start_time) - 1);
        if (row[6]) strncpy(records[count].end_time, row[6], sizeof(records[count].end_time) - 1);
        if (row[7]) strncpy(records[count].admin_id, row[7], sizeof(records[count].admin_id) - 1);
        if (row[8]) strncpy(records[count].reason, row[8], sizeof(records[count].reason) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
