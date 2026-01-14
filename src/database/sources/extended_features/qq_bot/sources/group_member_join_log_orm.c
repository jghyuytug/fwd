#include "group_member_join_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GroupMemberJoinLog_Add(DBConnectionManager* manager, const GroupMemberJoinLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO group_member_join_log (Uin, GroupCode, NickName, JoinTime, Level, CreatedAt, JoinNumber, UpdatedAt) "
        "VALUES (%d, %d, '%s', %d, %d, '%s', %d, '%s')",
        record->Uin, record->GroupCode, record->NickName, record->JoinTime, record->Level, record->CreatedAt, record->JoinNumber, record->UpdatedAt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GroupMemberJoinLog_Get(DBConnectionManager* manager, int Id, GroupMemberJoinLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, Uin, GroupCode, NickName, JoinTime, Level, CreatedAt, JoinNumber, UpdatedAt FROM group_member_join_log WHERE Id = %d",
        Id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GroupMemberJoinLog));
    record->Id = row[0] ? atoi(row[0]) : 0;
    record->Uin = row[1] ? atoll(row[1]) : 0;
    record->GroupCode = row[2] ? atoll(row[2]) : 0;
    if (row[3]) strncpy(record->NickName, row[3], sizeof(record->NickName) - 1);
    record->JoinTime = row[4] ? atoll(row[4]) : 0;
    record->Level = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->CreatedAt, row[6], sizeof(record->CreatedAt) - 1);
    record->JoinNumber = row[7] ? atoll(row[7]) : 0;
    if (row[8]) strncpy(record->UpdatedAt, row[8], sizeof(record->UpdatedAt) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GroupMemberJoinLog_GetAll(DBConnectionManager* manager, GroupMemberJoinLog* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, Uin, GroupCode, NickName, JoinTime, Level, CreatedAt, JoinNumber, UpdatedAt FROM group_member_join_log");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GroupMemberJoinLog));
        records[count].Id = row[0] ? atoi(row[0]) : 0;
        records[count].Uin = row[1] ? atoll(row[1]) : 0;
        records[count].GroupCode = row[2] ? atoll(row[2]) : 0;
        if (row[3]) strncpy(records[count].NickName, row[3], sizeof(records[count].NickName) - 1);
        records[count].JoinTime = row[4] ? atoll(row[4]) : 0;
        records[count].Level = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].CreatedAt, row[6], sizeof(records[count].CreatedAt) - 1);
        records[count].JoinNumber = row[7] ? atoll(row[7]) : 0;
        if (row[8]) strncpy(records[count].UpdatedAt, row[8], sizeof(records[count].UpdatedAt) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
