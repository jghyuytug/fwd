#include "repl_body_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ReplBody_Add(DBConnectionManager* manager, const ReplBody* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO repl_body (rno, body) "
        "VALUES (%d, '%s')",
        record->rno, record->body);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ReplBody_Get(DBConnectionManager* manager, int rno, ReplBody* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT rno, body FROM repl_body WHERE rno = %d",
        rno);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ReplBody));
    record->rno = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->body, row[1], sizeof(record->body) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int ReplBody_GetAll(DBConnectionManager* manager, ReplBody* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT rno, body FROM repl_body");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ReplBody));
        records[count].rno = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].body, row[1], sizeof(records[count].body) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
