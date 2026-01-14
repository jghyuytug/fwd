#include "penpic_file_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int PenpicFile_Add(DBConnectionManager* manager, const PenpicFile* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO penpic_file (content_no, file_url) "
        "VALUES (%d, '%s')",
        record->content_no, record->file_url);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PenpicFile_Get(DBConnectionManager* manager, int no, PenpicFile* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, content_no, file_url FROM penpic_file WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(PenpicFile));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->content_no = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->file_url, row[2], sizeof(record->file_url) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int PenpicFile_GetAll(DBConnectionManager* manager, PenpicFile* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, content_no, file_url FROM penpic_file");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(PenpicFile));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].content_no = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].file_url, row[2], sizeof(records[count].file_url) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
