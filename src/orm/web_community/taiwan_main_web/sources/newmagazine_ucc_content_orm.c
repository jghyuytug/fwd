#include "newmagazine_ucc_content_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int NewmagazineUccContent_Add(DBConnectionManager* manager, const NewmagazineUccContent* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO newmagazine_ucc_content (mz_id, content) "
        "VALUES (%d, '%s')",
        record->mz_id, record->content);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int NewmagazineUccContent_Get(DBConnectionManager* manager, int mz_id, NewmagazineUccContent* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT mz_id, content FROM newmagazine_ucc_content WHERE mz_id = %d",
        mz_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(NewmagazineUccContent));
    record->mz_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->content, row[1], sizeof(record->content) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int NewmagazineUccContent_GetAll(DBConnectionManager* manager, NewmagazineUccContent* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT mz_id, content FROM newmagazine_ucc_content");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(NewmagazineUccContent));
        records[count].mz_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].content, row[1], sizeof(records[count].content) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
