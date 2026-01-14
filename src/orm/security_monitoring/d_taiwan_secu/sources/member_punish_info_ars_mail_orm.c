#include "member_punish_info_ars_mail_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MemberPunishInfoArsMail_Add(DBConnectionManager* manager, const MemberPunishInfoArsMail* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_punish_info_ars_mail (m_id, punish_type, apply_flag, occ_time, send_time) "
        "VALUES (%d, %d, %d, '%s', '%s')",
        record->m_id, record->punish_type, record->apply_flag, record->occ_time, record->send_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_SECU, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishInfoArsMail_Get(DBConnectionManager* manager, int no, MemberPunishInfoArsMail* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, punish_type, apply_flag, occ_time, send_time FROM member_punish_info_ars_mail WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_SECU, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MemberPunishInfoArsMail));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->punish_type = row[2] ? atoi(row[2]) : 0;
    record->apply_flag = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->occ_time, row[4], sizeof(record->occ_time) - 1);
    if (row[5]) strncpy(record->send_time, row[5], sizeof(record->send_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int MemberPunishInfoArsMail_GetAll(DBConnectionManager* manager, MemberPunishInfoArsMail* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, punish_type, apply_flag, occ_time, send_time FROM member_punish_info_ars_mail");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_SECU, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberPunishInfoArsMail));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].punish_type = row[2] ? atoi(row[2]) : 0;
        records[count].apply_flag = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].occ_time, row[4], sizeof(records[count].occ_time) - 1);
        if (row[5]) strncpy(records[count].send_time, row[5], sizeof(records[count].send_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
