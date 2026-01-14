#include "member_punish_info_history_2013_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MemberPunishInfoHistory2013_Add(DBConnectionManager* manager, const MemberPunishInfoHistory2013* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_punish_info_history_2013 (m_id, punish_type, occ_time, punish_value, apply_flag, start_time, end_time, admin_id, reason, is_kicked, first_ssn, second_ssn) "
        "VALUES (%d, %d, '%s', %d, %d, '%s', '%s', '%s', '%s', %d, '%s', '%s')",
        record->m_id, record->punish_type, record->occ_time, record->punish_value, record->apply_flag, record->start_time, record->end_time, record->admin_id, record->reason, record->is_kicked, record->first_ssn, record->second_ssn);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_SECU, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishInfoHistory2013_Get(DBConnectionManager* manager, int no, MemberPunishInfoHistory2013* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, punish_type, occ_time, punish_value, apply_flag, start_time, end_time, admin_id, reason, is_kicked, first_ssn, second_ssn FROM member_punish_info_history_2013 WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_SECU, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MemberPunishInfoHistory2013));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->punish_type = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->occ_time, row[3], sizeof(record->occ_time) - 1);
    record->punish_value = row[4] ? atoi(row[4]) : 0;
    record->apply_flag = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->start_time, row[6], sizeof(record->start_time) - 1);
    if (row[7]) strncpy(record->end_time, row[7], sizeof(record->end_time) - 1);
    if (row[8]) strncpy(record->admin_id, row[8], sizeof(record->admin_id) - 1);
    if (row[9]) strncpy(record->reason, row[9], sizeof(record->reason) - 1);
    record->is_kicked = row[10] ? atoi(row[10]) : 0;
    if (row[11]) strncpy(record->first_ssn, row[11], sizeof(record->first_ssn) - 1);
    if (row[12]) strncpy(record->second_ssn, row[12], sizeof(record->second_ssn) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishInfoHistory2013_GetAll(DBConnectionManager* manager, MemberPunishInfoHistory2013* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, punish_type, occ_time, punish_value, apply_flag, start_time, end_time, admin_id, reason, is_kicked, first_ssn, second_ssn FROM member_punish_info_history_2013");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_SECU, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberPunishInfoHistory2013));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].punish_type = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].occ_time, row[3], sizeof(records[count].occ_time) - 1);
        records[count].punish_value = row[4] ? atoi(row[4]) : 0;
        records[count].apply_flag = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].start_time, row[6], sizeof(records[count].start_time) - 1);
        if (row[7]) strncpy(records[count].end_time, row[7], sizeof(records[count].end_time) - 1);
        if (row[8]) strncpy(records[count].admin_id, row[8], sizeof(records[count].admin_id) - 1);
        if (row[9]) strncpy(records[count].reason, row[9], sizeof(records[count].reason) - 1);
        records[count].is_kicked = row[10] ? atoi(row[10]) : 0;
        if (row[11]) strncpy(records[count].first_ssn, row[11], sizeof(records[count].first_ssn) - 1);
        if (row[12]) strncpy(records[count].second_ssn, row[12], sizeof(records[count].second_ssn) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
