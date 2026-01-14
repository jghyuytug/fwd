#include "slang_list_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int SlangList_Add(DBConnectionManager* manager, const SlangList* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO slang_list (slang) "
        "VALUES ('%s')",
        record->slang);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SlangList_Get(DBConnectionManager* manager, const char* slang, SlangList* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT slang FROM slang_list WHERE slang = '%s'",
        slang);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(SlangList));
    if (row[0]) strncpy(record->slang, row[0], sizeof(record->slang) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int SlangList_GetAll(DBConnectionManager* manager, SlangList* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT slang FROM slang_list");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(SlangList));
        if (row[0]) strncpy(records[count].slang, row[0], sizeof(records[count].slang) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
