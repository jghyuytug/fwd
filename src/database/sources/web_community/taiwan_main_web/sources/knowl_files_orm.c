#include "knowl_files_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int KnowlFiles_Add(DBConnectionManager* manager, const KnowlFiles* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO knowl_files (gno, gf_no, file_server, file_location) "
        "VALUES (%d, %d, '%s', '%s')",
        record->gno, record->gf_no, record->file_server, record->file_location);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int KnowlFiles_Get(DBConnectionManager* manager, int gno, signed char gf_no, KnowlFiles* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, gf_no, file_server, file_location FROM knowl_files WHERE gno = %d AND gf_no = %d",
        gno, gf_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(KnowlFiles));
    record->gno = row[0] ? atoi(row[0]) : 0;
    record->gf_no = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->file_server, row[2], sizeof(record->file_server) - 1);
    if (row[3]) strncpy(record->file_location, row[3], sizeof(record->file_location) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int KnowlFiles_GetAll(DBConnectionManager* manager, KnowlFiles* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT gno, gf_no, file_server, file_location FROM knowl_files");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(KnowlFiles));
        records[count].gno = row[0] ? atoi(row[0]) : 0;
        records[count].gf_no = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].file_server, row[2], sizeof(records[count].file_server) - 1);
        if (row[3]) strncpy(records[count].file_location, row[3], sizeof(records[count].file_location) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
