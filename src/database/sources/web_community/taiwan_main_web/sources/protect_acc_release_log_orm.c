#include "protect_acc_release_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ProtectAccReleaseLog_Add(DBConnectionManager* manager, const ProtectAccReleaseLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO protect_acc_release_log (m_id, occ_time, punish_type, cert_flag, cert_type, reason, apply_flag) "
        "VALUES (%d, %d, %d, %d, %d, '%s', %d)",
        record->m_id, record->occ_time, record->punish_type, record->cert_flag, record->cert_type, record->reason, record->apply_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ProtectAccReleaseLog_Get(DBConnectionManager* manager, int no, ProtectAccReleaseLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, occ_time, punish_type, cert_flag, cert_type, reason, apply_flag FROM protect_acc_release_log WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ProtectAccReleaseLog));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->occ_time = row[2] ? atoi(row[2]) : 0;
    record->punish_type = row[3] ? atoi(row[3]) : 0;
    record->cert_flag = row[4] ? atoi(row[4]) : 0;
    record->cert_type = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->reason, row[6], sizeof(record->reason) - 1);
    record->apply_flag = row[7] ? atoi(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int ProtectAccReleaseLog_GetAll(DBConnectionManager* manager, ProtectAccReleaseLog* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, occ_time, punish_type, cert_flag, cert_type, reason, apply_flag FROM protect_acc_release_log");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ProtectAccReleaseLog));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].occ_time = row[2] ? atoi(row[2]) : 0;
        records[count].punish_type = row[3] ? atoi(row[3]) : 0;
        records[count].cert_flag = row[4] ? atoi(row[4]) : 0;
        records[count].cert_type = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].reason, row[6], sizeof(records[count].reason) - 1);
        records[count].apply_flag = row[7] ? atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
