#include "assert_manager_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int AssertManager_Add(DBConnectionManager* manager, const AssertManager* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO assert_manager (file_name, file_line, reason, cnt) "
        "VALUES ('%s', %d, '%s', %d)",
        record->file_name, record->file_line, record->reason, record->cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AssertManager_Get(DBConnectionManager* manager, const char* file_name, short file_line, AssertManager* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT file_name, file_line, reason, cnt FROM assert_manager WHERE file_name = '%s' AND file_line = %d",
        file_name, file_line);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(AssertManager));
    if (row[0]) strncpy(record->file_name, row[0], sizeof(record->file_name) - 1);
    record->file_line = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->reason, row[2], sizeof(record->reason) - 1);
    record->cnt = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int AssertManager_GetAll(DBConnectionManager* manager, AssertManager* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT file_name, file_line, reason, cnt FROM assert_manager");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(AssertManager));
        if (row[0]) strncpy(records[count].file_name, row[0], sizeof(records[count].file_name) - 1);
        records[count].file_line = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].reason, row[2], sizeof(records[count].reason) - 1);
        records[count].cnt = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
