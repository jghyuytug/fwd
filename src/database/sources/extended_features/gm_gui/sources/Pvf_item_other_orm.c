#include "Pvf_item_other_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int PvfItemOther_Add(DBConnectionManager* manager, const PvfItemOther* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO Pvf_item_other (ItemCode, ItemName, ShopId, Type) "
        "VALUES (%d, '%s', %d, %d)",
        record->ItemCode, record->ItemName, record->ShopId, record->Type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PvfItemOther_Get(DBConnectionManager* manager, int Id, PvfItemOther* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, ItemCode, ItemName, ShopId, Type FROM Pvf_item_other WHERE Id = %d",
        Id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(PvfItemOther));
    record->Id = row[0] ? atoi(row[0]) : 0;
    record->ItemCode = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->ItemName, row[2], sizeof(record->ItemName) - 1);
    record->ShopId = row[3] ? atoi(row[3]) : 0;
    record->Type = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int PvfItemOther_GetAll(DBConnectionManager* manager, PvfItemOther* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, ItemCode, ItemName, ShopId, Type FROM Pvf_item_other");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(PvfItemOther));
        records[count].Id = row[0] ? atoi(row[0]) : 0;
        records[count].ItemCode = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].ItemName, row[2], sizeof(records[count].ItemName) - 1);
        records[count].ShopId = row[3] ? atoi(row[3]) : 0;
        records[count].Type = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
