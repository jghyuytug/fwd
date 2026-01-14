#include "test2_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 1024

int Test2_Add(DBConnectionManager* manager, const Test2* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query), "INSERT INTO test2 (a) VALUES (%u)", record->a);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int Test2_Get(DBConnectionManager* manager, unsigned int a, Test2* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query), "SELECT a FROM test2 WHERE a = %u", a);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(Test2));
    record->a = row[0] ? (unsigned int)atoi(row[0]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int Test2_Delete(DBConnectionManager* manager, unsigned int a) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "DELETE FROM test2 WHERE a = %u", a);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

void Test2_PrintInfo(const Test2* record) {
    if (!record) return;
    printf("Test2: a=%u
", record->a);
}
