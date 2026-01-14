#include "sign_in_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int SignInLog_Add(DBConnectionManager* manager, const SignInLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO sign_in_log (Uin, CharacId, Uid, GroupCode, ItemCount, ItemName, CreatedAt) "
        "VALUES (%d, %d, %d, %d, %d, '%s', '%s')",
        record->Uin, record->CharacId, record->Uid, record->GroupCode, record->ItemCount, record->ItemName, record->CreatedAt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SignInLog_Get(DBConnectionManager* manager, int Id, SignInLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, Uin, CharacId, Uid, GroupCode, ItemCount, ItemName, CreatedAt FROM sign_in_log WHERE Id = %d",
        Id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(SignInLog));
    record->Id = row[0] ? atoi(row[0]) : 0;
    record->Uin = row[1] ? atoll(row[1]) : 0;
    record->CharacId = row[2] ? atoi(row[2]) : 0;
    record->Uid = row[3] ? atoi(row[3]) : 0;
    record->GroupCode = row[4] ? atoll(row[4]) : 0;
    record->ItemCount = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->ItemName, row[6], sizeof(record->ItemName) - 1);
    if (row[7]) strncpy(record->CreatedAt, row[7], sizeof(record->CreatedAt) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int SignInLog_GetAll(DBConnectionManager* manager, SignInLog* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, Uin, CharacId, Uid, GroupCode, ItemCount, ItemName, CreatedAt FROM sign_in_log");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(SignInLog));
        records[count].Id = row[0] ? atoi(row[0]) : 0;
        records[count].Uin = row[1] ? atoll(row[1]) : 0;
        records[count].CharacId = row[2] ? atoi(row[2]) : 0;
        records[count].Uid = row[3] ? atoi(row[3]) : 0;
        records[count].GroupCode = row[4] ? atoll(row[4]) : 0;
        records[count].ItemCount = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].ItemName, row[6], sizeof(records[count].ItemName) - 1);
        if (row[7]) strncpy(records[count].CreatedAt, row[7], sizeof(records[count].CreatedAt) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
