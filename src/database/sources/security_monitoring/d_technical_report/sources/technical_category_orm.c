#include "technical_category_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int TechnicalCategory_Add(DBConnectionManager* manager, const TechnicalCategory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO technical_category (cateno, pcateno, name, step) "
        "VALUES (%d, %d, '%s', %d)",
        record->cateno, record->pcateno, record->name, record->step);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int TechnicalCategory_GetAll(DBConnectionManager* manager, TechnicalCategory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT cateno, pcateno, name, step FROM technical_category");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(TechnicalCategory));
        records[count].cateno = row[0] ? atoi(row[0]) : 0;
        records[count].pcateno = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].name, row[2], sizeof(records[count].name) - 1);
        records[count].step = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
