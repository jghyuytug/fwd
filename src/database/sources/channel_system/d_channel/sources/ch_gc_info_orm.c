#include "ch_gc_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ChGcInfo_Add(DBConnectionManager* manager, const ChGcInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO ch_gc_info (group_name, group_gc_no) "
        "VALUES ('%s', '%s')",
        record->group_name, record->group_gc_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ChGcInfo_GetAll(DBConnectionManager* manager, ChGcInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT group_name, group_gc_no FROM ch_gc_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ChGcInfo));
        if (row[0]) strncpy(records[count].group_name, row[0], sizeof(records[count].group_name) - 1);
        if (row[1]) strncpy(records[count].group_gc_no, row[1], sizeof(records[count].group_gc_no) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
