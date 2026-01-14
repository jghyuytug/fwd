#include "group_member_bind_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GroupMemberBind_Add(DBConnectionManager* manager, const GroupMemberBind* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO group_member_bind (Uin, CharacId, Uid, GroupCode, CreatedAt, UpdatedAt) "
        "VALUES (%d, %d, %d, %d, '%s', '%s')",
        record->Uin, record->CharacId, record->Uid, record->GroupCode, record->CreatedAt, record->UpdatedAt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GroupMemberBind_Get(DBConnectionManager* manager, long long Uin, GroupMemberBind* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT Uin, CharacId, Uid, GroupCode, CreatedAt, UpdatedAt FROM group_member_bind WHERE Uin = %d",
        Uin);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GroupMemberBind));
    record->Uin = row[0] ? atoll(row[0]) : 0;
    record->CharacId = row[1] ? atoi(row[1]) : 0;
    record->Uid = row[2] ? atoi(row[2]) : 0;
    record->GroupCode = row[3] ? atoll(row[3]) : 0;
    if (row[4]) strncpy(record->CreatedAt, row[4], sizeof(record->CreatedAt) - 1);
    if (row[5]) strncpy(record->UpdatedAt, row[5], sizeof(record->UpdatedAt) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GroupMemberBind_GetAll(DBConnectionManager* manager, GroupMemberBind* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT Uin, CharacId, Uid, GroupCode, CreatedAt, UpdatedAt FROM group_member_bind");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GroupMemberBind));
        records[count].Uin = row[0] ? atoll(row[0]) : 0;
        records[count].CharacId = row[1] ? atoi(row[1]) : 0;
        records[count].Uid = row[2] ? atoi(row[2]) : 0;
        records[count].GroupCode = row[3] ? atoll(row[3]) : 0;
        if (row[4]) strncpy(records[count].CreatedAt, row[4], sizeof(records[count].CreatedAt) - 1);
        if (row[5]) strncpy(records[count].UpdatedAt, row[5], sizeof(records[count].UpdatedAt) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
