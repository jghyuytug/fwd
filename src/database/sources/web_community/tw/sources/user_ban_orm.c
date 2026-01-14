#include "user_ban_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int UserBan_Add(DBConnectionManager* manager, const UserBan* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO user_ban (category, m_id, ban_term, ban_reason, detail_reason, ban_date, cancel_reason, cancel_date, admin_id, status, first_ssn, second_ssn) "
        "VALUES (%d, %d, %d, %d, '%s', %d, '%s', %d, %d, %d, '%s', '%s')",
        record->category, record->m_id, record->ban_term, record->ban_reason, record->detail_reason, record->ban_date, record->cancel_reason, record->cancel_date, record->admin_id, record->status, record->first_ssn, record->second_ssn);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int UserBan_Get(DBConnectionManager* manager, int no, UserBan* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, category, m_id, ban_term, ban_reason, detail_reason, ban_date, cancel_reason, cancel_date, admin_id, status, first_ssn, second_ssn FROM user_ban WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(UserBan));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->category = row[1] ? atoi(row[1]) : 0;
    record->m_id = row[2] ? atoi(row[2]) : 0;
    record->ban_term = row[3] ? atoi(row[3]) : 0;
    record->ban_reason = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->detail_reason, row[5], sizeof(record->detail_reason) - 1);
    record->ban_date = row[6] ? atoi(row[6]) : 0;
    if (row[7]) strncpy(record->cancel_reason, row[7], sizeof(record->cancel_reason) - 1);
    record->cancel_date = row[8] ? atoi(row[8]) : 0;
    record->admin_id = row[9] ? atoi(row[9]) : 0;
    record->status = row[10] ? atoi(row[10]) : 0;
    if (row[11]) strncpy(record->first_ssn, row[11], sizeof(record->first_ssn) - 1);
    if (row[12]) strncpy(record->second_ssn, row[12], sizeof(record->second_ssn) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int UserBan_GetAll(DBConnectionManager* manager, UserBan* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, category, m_id, ban_term, ban_reason, detail_reason, ban_date, cancel_reason, cancel_date, admin_id, status, first_ssn, second_ssn FROM user_ban");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(UserBan));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].category = row[1] ? atoi(row[1]) : 0;
        records[count].m_id = row[2] ? atoi(row[2]) : 0;
        records[count].ban_term = row[3] ? atoi(row[3]) : 0;
        records[count].ban_reason = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].detail_reason, row[5], sizeof(records[count].detail_reason) - 1);
        records[count].ban_date = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].cancel_reason, row[7], sizeof(records[count].cancel_reason) - 1);
        records[count].cancel_date = row[8] ? atoi(row[8]) : 0;
        records[count].admin_id = row[9] ? atoi(row[9]) : 0;
        records[count].status = row[10] ? atoi(row[10]) : 0;
        if (row[11]) strncpy(records[count].first_ssn, row[11], sizeof(records[count].first_ssn) - 1);
        if (row[12]) strncpy(records[count].second_ssn, row[12], sizeof(records[count].second_ssn) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
