#include "max_count_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MaxCount_Add(DBConnectionManager* manager, const MaxCount* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO max_count (server_info, mc_max, mc_date) "
        "VALUES (%d, %d, '%s')",
        record->server_info, record->mc_max, record->mc_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MaxCount_GetAll(DBConnectionManager* manager, MaxCount* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_info, mc_max, mc_date FROM max_count");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MaxCount));
        records[count].server_info = row[0] ? atoi(row[0]) : 0;
        records[count].mc_max = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].mc_date, row[2], sizeof(records[count].mc_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
