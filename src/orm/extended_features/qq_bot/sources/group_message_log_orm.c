#include "group_message_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GroupMessageLog_Add(DBConnectionManager* manager, const GroupMessageLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO group_message_log (QQGroupNumber, QQGroupName, SendQQNumber, SendNickName, QQMessage, CreatedAt) "
        "VALUES (%d, '%s', %d, '%s', '%s', '%s')",
        record->QQGroupNumber, record->QQGroupName, record->SendQQNumber, record->SendNickName, record->QQMessage, record->CreatedAt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GroupMessageLog_Get(DBConnectionManager* manager, int Id, GroupMessageLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, QQGroupNumber, QQGroupName, SendQQNumber, SendNickName, QQMessage, CreatedAt FROM group_message_log WHERE Id = %d",
        Id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GroupMessageLog));
    record->Id = row[0] ? atoi(row[0]) : 0;
    record->QQGroupNumber = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->QQGroupName, row[2], sizeof(record->QQGroupName) - 1);
    record->SendQQNumber = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->SendNickName, row[4], sizeof(record->SendNickName) - 1);
    if (row[5]) strncpy(record->QQMessage, row[5], sizeof(record->QQMessage) - 1);
    if (row[6]) strncpy(record->CreatedAt, row[6], sizeof(record->CreatedAt) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GroupMessageLog_GetAll(DBConnectionManager* manager, GroupMessageLog* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, QQGroupNumber, QQGroupName, SendQQNumber, SendNickName, QQMessage, CreatedAt FROM group_message_log");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GroupMessageLog));
        records[count].Id = row[0] ? atoi(row[0]) : 0;
        records[count].QQGroupNumber = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].QQGroupName, row[2], sizeof(records[count].QQGroupName) - 1);
        records[count].SendQQNumber = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].SendNickName, row[4], sizeof(records[count].SendNickName) - 1);
        if (row[5]) strncpy(records[count].QQMessage, row[5], sizeof(records[count].QQMessage) - 1);
        if (row[6]) strncpy(records[count].CreatedAt, row[6], sizeof(records[count].CreatedAt) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
