#include "internet_line_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int InternetLine_Add(DBConnectionManager* manager, const InternetLine* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO internet_line (line_name) "
        "VALUES ('%s')",
        record->line_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int InternetLine_Get(DBConnectionManager* manager, signed char line_code, InternetLine* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT line_code, line_name FROM internet_line WHERE line_code = %d",
        line_code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(InternetLine));
    record->line_code = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->line_name, row[1], sizeof(record->line_name) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int InternetLine_GetAll(DBConnectionManager* manager, InternetLine* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT line_code, line_name FROM internet_line");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(InternetLine));
        records[count].line_code = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].line_name, row[1], sizeof(records[count].line_name) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
