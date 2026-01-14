#include "pvf_item_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int PvfItem_Add(DBConnectionManager* manager, const PvfItem* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO pvf_item (ItemCode, ItemName, IconId, IconIndex, Rarity, ItemCodeType, StackLimit) "
        "VALUES (%d, '%s', %d, %d, %d, %d, %d)",
        record->ItemCode, record->ItemName, record->IconId, record->IconIndex, record->Rarity, record->ItemCodeType, record->StackLimit);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PvfItem_Get(DBConnectionManager* manager, int Id, PvfItem* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, ItemCode, ItemName, IconId, IconIndex, Rarity, ItemCodeType, StackLimit FROM pvf_item WHERE Id = %d",
        Id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(PvfItem));
    record->Id = row[0] ? atoi(row[0]) : 0;
    record->ItemCode = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->ItemName, row[2], sizeof(record->ItemName) - 1);
    record->IconId = row[3] ? atoi(row[3]) : 0;
    record->IconIndex = row[4] ? atoi(row[4]) : 0;
    record->Rarity = row[5] ? atoi(row[5]) : 0;
    record->ItemCodeType = row[6] ? atoi(row[6]) : 0;
    record->StackLimit = row[7] ? atoi(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int PvfItem_GetAll(DBConnectionManager* manager, PvfItem* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, ItemCode, ItemName, IconId, IconIndex, Rarity, ItemCodeType, StackLimit FROM pvf_item");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(PvfItem));
        records[count].Id = row[0] ? atoi(row[0]) : 0;
        records[count].ItemCode = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].ItemName, row[2], sizeof(records[count].ItemName) - 1);
        records[count].IconId = row[3] ? atoi(row[3]) : 0;
        records[count].IconIndex = row[4] ? atoi(row[4]) : 0;
        records[count].Rarity = row[5] ? atoi(row[5]) : 0;
        records[count].ItemCodeType = row[6] ? atoi(row[6]) : 0;
        records[count].StackLimit = row[7] ? atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
