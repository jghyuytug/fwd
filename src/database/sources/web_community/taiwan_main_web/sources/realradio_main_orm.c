#include "realradio_main_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int RealradioMain_Add(DBConnectionManager* manager, const RealradioMain* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO realradio_main (type, text1, text2) "
        "VALUES (%d, '%s', '%s')",
        record->type, record->text1, record->text2);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int RealradioMain_Get(DBConnectionManager* manager, signed char type, RealradioMain* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT type, text1, text2 FROM realradio_main WHERE type = %d",
        type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(RealradioMain));
    record->type = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->text1, row[1], sizeof(record->text1) - 1);
    if (row[2]) strncpy(record->text2, row[2], sizeof(record->text2) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int RealradioMain_GetAll(DBConnectionManager* manager, RealradioMain* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT type, text1, text2 FROM realradio_main");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(RealradioMain));
        records[count].type = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].text1, row[1], sizeof(records[count].text1) - 1);
        if (row[2]) strncpy(records[count].text2, row[2], sizeof(records[count].text2) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
