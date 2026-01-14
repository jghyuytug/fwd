#include "d_date_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DDate_Add(DBConnectionManager* manager, const DDate* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO d_date (aa) "
        "VALUES ('%s')",
        record->aa);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DDate_GetAll(DBConnectionManager* manager, DDate* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT aa FROM d_date");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DDate));
        if (row[0]) strncpy(records[count].aa, row[0], sizeof(records[count].aa) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
