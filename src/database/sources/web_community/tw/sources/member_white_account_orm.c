#include "member_white_account_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MemberWhiteAccount_Add(DBConnectionManager* manager, const MemberWhiteAccount* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_white_account (m_id, reg_date) "
        "VALUES (%d, '%s')",
        record->m_id, record->reg_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberWhiteAccount_Get(DBConnectionManager* manager, int m_id, MemberWhiteAccount* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, reg_date FROM member_white_account WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MemberWhiteAccount));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->reg_date, row[1], sizeof(record->reg_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int MemberWhiteAccount_GetAll(DBConnectionManager* manager, MemberWhiteAccount* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, reg_date FROM member_white_account");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberWhiteAccount));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].reg_date, row[1], sizeof(records[count].reg_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
