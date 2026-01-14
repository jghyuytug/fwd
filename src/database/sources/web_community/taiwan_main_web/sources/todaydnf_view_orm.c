#include "todaydnf_view_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int TodaydnfView_Add(DBConnectionManager* manager, const TodaydnfView* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO todaydnf_view (no, type, uv, pv) "
        "VALUES (%d, %d, %d, %d)",
        record->no, record->type, record->uv, record->pv);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TodaydnfView_Get(DBConnectionManager* manager, int no, signed char type, TodaydnfView* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, type, uv, pv FROM todaydnf_view WHERE no = %d AND type = %d",
        no, type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(TodaydnfView));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->type = row[1] ? atoi(row[1]) : 0;
    record->uv = row[2] ? atoi(row[2]) : 0;
    record->pv = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int TodaydnfView_GetAll(DBConnectionManager* manager, TodaydnfView* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, type, uv, pv FROM todaydnf_view");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(TodaydnfView));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].type = row[1] ? atoi(row[1]) : 0;
        records[count].uv = row[2] ? atoi(row[2]) : 0;
        records[count].pv = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
