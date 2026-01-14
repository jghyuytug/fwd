#include "pswd_qstion_direct_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int PswdQstionDirect_Add(DBConnectionManager* manager, const PswdQstionDirect* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO pswd_qstion_direct (m_id, q_text) "
        "VALUES (%d, '%s')",
        record->m_id, record->q_text);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PswdQstionDirect_Get(DBConnectionManager* manager, int m_id, PswdQstionDirect* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, q_text FROM pswd_qstion_direct WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(PswdQstionDirect));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->q_text, row[1], sizeof(record->q_text) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int PswdQstionDirect_GetAll(DBConnectionManager* manager, PswdQstionDirect* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, q_text FROM pswd_qstion_direct");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(PswdQstionDirect));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].q_text, row[1], sizeof(records[count].q_text) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
