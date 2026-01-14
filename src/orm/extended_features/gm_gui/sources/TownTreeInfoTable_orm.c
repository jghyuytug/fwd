#include "TownTreeInfoTable_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int Towntreeinfotable_Add(DBConnectionManager* manager, const Towntreeinfotable* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO TownTreeInfoTable (Items) "
        "VALUES ('%s')",
        record->Items);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Towntreeinfotable_Get(DBConnectionManager* manager, int Id, Towntreeinfotable* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, Items FROM TownTreeInfoTable WHERE Id = %d",
        Id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(Towntreeinfotable));
    record->Id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->Items, row[1], sizeof(record->Items) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int Towntreeinfotable_GetAll(DBConnectionManager* manager, Towntreeinfotable* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, Items FROM TownTreeInfoTable");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(Towntreeinfotable));
        records[count].Id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].Items, row[1], sizeof(records[count].Items) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
