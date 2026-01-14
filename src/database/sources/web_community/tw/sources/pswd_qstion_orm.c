#include "pswd_qstion_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int PswdQstion_Add(DBConnectionManager* manager, const PswdQstion* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO pswd_qstion (q_no, q_text) "
        "VALUES (%d, '%s')",
        record->q_no, record->q_text);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PswdQstion_Get(DBConnectionManager* manager, signed char q_no, PswdQstion* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT q_no, q_text FROM pswd_qstion WHERE q_no = %d",
        q_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(PswdQstion));
    record->q_no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->q_text, row[1], sizeof(record->q_text) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int PswdQstion_GetAll(DBConnectionManager* manager, PswdQstion* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT q_no, q_text FROM pswd_qstion");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(PswdQstion));
        records[count].q_no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].q_text, row[1], sizeof(records[count].q_text) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
