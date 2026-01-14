#include "world_cup_memo_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int WorldCupMemo_Add(DBConnectionManager* manager, const WorldCupMemo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO world_cup_memo (nick_name, memo, create_time) "
        "VALUES ('%s', '%s', '%s')",
        record->nick_name, record->memo, record->create_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int WorldCupMemo_Get(DBConnectionManager* manager, int no, WorldCupMemo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, nick_name, memo, create_time FROM world_cup_memo WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(WorldCupMemo));
    record->no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->nick_name, row[1], sizeof(record->nick_name) - 1);
    if (row[2]) strncpy(record->memo, row[2], sizeof(record->memo) - 1);
    if (row[3]) strncpy(record->create_time, row[3], sizeof(record->create_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int WorldCupMemo_GetAll(DBConnectionManager* manager, WorldCupMemo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, nick_name, memo, create_time FROM world_cup_memo");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(WorldCupMemo));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].nick_name, row[1], sizeof(records[count].nick_name) - 1);
        if (row[2]) strncpy(records[count].memo, row[2], sizeof(records[count].memo) - 1);
        if (row[3]) strncpy(records[count].create_time, row[3], sizeof(records[count].create_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
