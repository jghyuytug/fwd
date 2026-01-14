#include "account_cerashop_restrict_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int AccountCerashopRestrict_Add(DBConnectionManager* manager, const AccountCerashopRestrict* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO account_cerashop_restrict (m_id, ipg_no, count, next_date, end_date, last_access_date) "
        "VALUES (%d, %d, %d, %d, %d, %d)",
        record->m_id, record->ipg_no, record->count, record->next_date, record->end_date, record->last_access_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AccountCerashopRestrict_Get(DBConnectionManager* manager, int m_id, int ipg_no, AccountCerashopRestrict* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, ipg_no, count, next_date, end_date, last_access_date FROM account_cerashop_restrict WHERE m_id = %d AND ipg_no = %d",
        m_id, ipg_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(AccountCerashopRestrict));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->ipg_no = row[1] ? atoi(row[1]) : 0;
    record->count = row[2] ? atoi(row[2]) : 0;
    record->next_date = row[3] ? atoi(row[3]) : 0;
    record->end_date = row[4] ? atoi(row[4]) : 0;
    record->last_access_date = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int AccountCerashopRestrict_GetAll(DBConnectionManager* manager, AccountCerashopRestrict* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, ipg_no, count, next_date, end_date, last_access_date FROM account_cerashop_restrict");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(AccountCerashopRestrict));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].ipg_no = row[1] ? atoi(row[1]) : 0;
        records[count].count = row[2] ? atoi(row[2]) : 0;
        records[count].next_date = row[3] ? atoi(row[3]) : 0;
        records[count].end_date = row[4] ? atoi(row[4]) : 0;
        records[count].last_access_date = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
