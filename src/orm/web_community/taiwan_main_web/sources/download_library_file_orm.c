#include "download_library_file_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DownloadLibraryFile_Add(DBConnectionManager* manager, const DownloadLibraryFile* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO download_library_file (no, file_info, file_url) "
        "VALUES (%d, '%s', '%s')",
        record->no, record->file_info, record->file_url);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DownloadLibraryFile_Get(DBConnectionManager* manager, int file_no, DownloadLibraryFile* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT file_no, no, file_info, file_url FROM download_library_file WHERE file_no = %d",
        file_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DownloadLibraryFile));
    record->file_no = row[0] ? atoi(row[0]) : 0;
    record->no = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->file_info, row[2], sizeof(record->file_info) - 1);
    if (row[3]) strncpy(record->file_url, row[3], sizeof(record->file_url) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DownloadLibraryFile_GetAll(DBConnectionManager* manager, DownloadLibraryFile* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT file_no, no, file_info, file_url FROM download_library_file");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DownloadLibraryFile));
        records[count].file_no = row[0] ? atoi(row[0]) : 0;
        records[count].no = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].file_info, row[2], sizeof(records[count].file_info) - 1);
        if (row[3]) strncpy(records[count].file_url, row[3], sizeof(records[count].file_url) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
