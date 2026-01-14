#include "sign_in_item_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int SignInItemInfo_Add(DBConnectionManager* manager, const SignInItemInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO sign_in_item_info (ItemCode, MinNum, MaxNum, Probability, PostalType) "
        "VALUES (%d, %d, %d, %d, %d)",
        record->ItemCode, record->MinNum, record->MaxNum, record->Probability, record->PostalType);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SignInItemInfo_Get(DBConnectionManager* manager, int Id, SignInItemInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, ItemCode, MinNum, MaxNum, Probability, PostalType FROM sign_in_item_info WHERE Id = %d",
        Id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(SignInItemInfo));
    record->Id = row[0] ? atoi(row[0]) : 0;
    record->ItemCode = row[1] ? atoi(row[1]) : 0;
    record->MinNum = row[2] ? atoi(row[2]) : 0;
    record->MaxNum = row[3] ? atoi(row[3]) : 0;
    record->Probability = row[4] ? atoi(row[4]) : 0;
    record->PostalType = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int SignInItemInfo_GetAll(DBConnectionManager* manager, SignInItemInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, ItemCode, MinNum, MaxNum, Probability, PostalType FROM sign_in_item_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(SignInItemInfo));
        records[count].Id = row[0] ? atoi(row[0]) : 0;
        records[count].ItemCode = row[1] ? atoi(row[1]) : 0;
        records[count].MinNum = row[2] ? atoi(row[2]) : 0;
        records[count].MaxNum = row[3] ? atoi(row[3]) : 0;
        records[count].Probability = row[4] ? atoi(row[4]) : 0;
        records[count].PostalType = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
