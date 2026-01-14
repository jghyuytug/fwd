#include "bbs_div_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int BbsDiv_Add(DBConnectionManager* manager, const BbsDiv* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO bbs_div (bbs_name, bbs_kind) "
        "VALUES ('%s', '%s')",
        record->bbs_name, record->bbs_kind);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int BbsDiv_Get(DBConnectionManager* manager, signed char bbs_code, BbsDiv* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT bbs_code, bbs_name, bbs_kind FROM bbs_div WHERE bbs_code = %d",
        bbs_code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(BbsDiv));
    record->bbs_code = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->bbs_name, row[1], sizeof(record->bbs_name) - 1);
    if (row[2]) strncpy(record->bbs_kind, row[2], sizeof(record->bbs_kind) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int BbsDiv_GetAll(DBConnectionManager* manager, BbsDiv* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT bbs_code, bbs_name, bbs_kind FROM bbs_div");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(BbsDiv));
        records[count].bbs_code = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].bbs_name, row[1], sizeof(records[count].bbs_name) - 1);
        if (row[2]) strncpy(records[count].bbs_kind, row[2], sizeof(records[count].bbs_kind) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
