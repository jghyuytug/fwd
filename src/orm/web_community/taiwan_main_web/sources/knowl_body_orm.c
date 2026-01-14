#include "knowl_body_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int KnowlBody_Add(DBConnectionManager* manager, const KnowlBody* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO knowl_body (gno, body) "
        "VALUES (%d, '%s')",
        record->gno, record->body);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int KnowlBody_Get(DBConnectionManager* manager, int gno, KnowlBody* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, body FROM knowl_body WHERE gno = %d",
        gno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(KnowlBody));
    record->gno = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->body, row[1], sizeof(record->body) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int KnowlBody_GetAll(DBConnectionManager* manager, KnowlBody* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, body FROM knowl_body");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(KnowlBody));
        records[count].gno = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].body, row[1], sizeof(records[count].body) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
