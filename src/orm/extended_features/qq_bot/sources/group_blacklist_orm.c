#include "group_blacklist_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GroupBlacklist_Add(DBConnectionManager* manager, const GroupBlacklist* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO group_blacklist (QQGroupNumber, Uin) "
        "VALUES (%d, %d)",
        record->QQGroupNumber, record->Uin);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GroupBlacklist_Get(DBConnectionManager* manager, int Id, GroupBlacklist* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, QQGroupNumber, Uin FROM group_blacklist WHERE Id = %d",
        Id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GroupBlacklist));
    record->Id = row[0] ? atoi(row[0]) : 0;
    record->QQGroupNumber = row[1] ? atoi(row[1]) : 0;
    record->Uin = row[2] ? atoi(row[2]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int GroupBlacklist_GetAll(DBConnectionManager* manager, GroupBlacklist* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT Id, QQGroupNumber, Uin FROM group_blacklist");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GroupBlacklist));
        records[count].Id = row[0] ? atoi(row[0]) : 0;
        records[count].QQGroupNumber = row[1] ? atoi(row[1]) : 0;
        records[count].Uin = row[2] ? atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
