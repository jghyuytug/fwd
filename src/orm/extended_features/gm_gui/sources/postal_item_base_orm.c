#include "postal_item_base_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int PostalItemBase_Add(DBConnectionManager* manager, const PostalItemBase* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO postal_item_base (Target, ItemName, ItemCode, ItemCount, MaxCount, Upgrade, Best, AmplifyOptionType, AmplifyValue, Gold, SealFlag, SeperateUpgrade, Endurance, PostalType, IsEQU, CreatedAt, Probability) "
        "VALUES (%d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', %d)",
        record->Target, record->ItemName, record->ItemCode, record->ItemCount, record->MaxCount, record->Upgrade, record->Best, record->AmplifyOptionType, record->AmplifyValue, record->Gold, record->SealFlag, record->SeperateUpgrade, record->Endurance, record->PostalType, record->IsEQU, record->CreatedAt, record->Probability);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PostalItemBase_Get(DBConnectionManager* manager, int Id, PostalItemBase* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[18];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, Target, ItemName, ItemCode, ItemCount, MaxCount, Upgrade, Best, AmplifyOptionType, AmplifyValue, Gold, SealFlag, SeperateUpgrade, Endurance, PostalType, IsEQU, CreatedAt, Probability FROM postal_item_base WHERE Id = %d",
        Id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(PostalItemBase));
    record->Id = row[0] ? atoi(row[0]) : 0;
    record->Target = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->ItemName, row[2], sizeof(record->ItemName) - 1);
    record->ItemCode = row[3] ? atoi(row[3]) : 0;
    record->ItemCount = row[4] ? atoi(row[4]) : 0;
    record->MaxCount = row[5] ? atoi(row[5]) : 0;
    record->Upgrade = row[6] ? atoi(row[6]) : 0;
    record->Best = row[7] ? atoi(row[7]) : 0;
    record->AmplifyOptionType = row[8] ? atoi(row[8]) : 0;
    record->AmplifyValue = row[9] ? atoi(row[9]) : 0;
    record->Gold = row[10] ? atoi(row[10]) : 0;
    record->SealFlag = row[11] ? atoi(row[11]) : 0;
    record->SeperateUpgrade = row[12] ? atoi(row[12]) : 0;
    record->Endurance = row[13] ? atoi(row[13]) : 0;
    record->PostalType = row[14] ? atoi(row[14]) : 0;
    record->IsEQU = row[15] ? atoi(row[15]) : 0;
    if (row[16]) strncpy(record->CreatedAt, row[16], sizeof(record->CreatedAt) - 1);
    record->Probability = row[17] ? atoll(row[17]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int PostalItemBase_GetAll(DBConnectionManager* manager, PostalItemBase* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[18];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, Target, ItemName, ItemCode, ItemCount, MaxCount, Upgrade, Best, AmplifyOptionType, AmplifyValue, Gold, SealFlag, SeperateUpgrade, Endurance, PostalType, IsEQU, CreatedAt, Probability FROM postal_item_base");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(PostalItemBase));
        records[count].Id = row[0] ? atoi(row[0]) : 0;
        records[count].Target = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].ItemName, row[2], sizeof(records[count].ItemName) - 1);
        records[count].ItemCode = row[3] ? atoi(row[3]) : 0;
        records[count].ItemCount = row[4] ? atoi(row[4]) : 0;
        records[count].MaxCount = row[5] ? atoi(row[5]) : 0;
        records[count].Upgrade = row[6] ? atoi(row[6]) : 0;
        records[count].Best = row[7] ? atoi(row[7]) : 0;
        records[count].AmplifyOptionType = row[8] ? atoi(row[8]) : 0;
        records[count].AmplifyValue = row[9] ? atoi(row[9]) : 0;
        records[count].Gold = row[10] ? atoi(row[10]) : 0;
        records[count].SealFlag = row[11] ? atoi(row[11]) : 0;
        records[count].SeperateUpgrade = row[12] ? atoi(row[12]) : 0;
        records[count].Endurance = row[13] ? atoi(row[13]) : 0;
        records[count].PostalType = row[14] ? atoi(row[14]) : 0;
        records[count].IsEQU = row[15] ? atoi(row[15]) : 0;
        if (row[16]) strncpy(records[count].CreatedAt, row[16], sizeof(records[count].CreatedAt) - 1);
        records[count].Probability = row[17] ? atoll(row[17]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
