#include "log_level_dungeon_play_count_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogLevelDungeonPlayCount_Add(DBConnectionManager* manager, const LogLevelDungeonPlayCount* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_level_dungeon_play_count (level, total_charac_count, total_dungeon_play_count) "
        "VALUES (%d, %d, %d)",
        record->level, record->total_charac_count, record->total_dungeon_play_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogLevelDungeonPlayCount_Get(DBConnectionManager* manager, short level, LogLevelDungeonPlayCount* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT level, total_charac_count, total_dungeon_play_count FROM log_level_dungeon_play_count WHERE level = %d",
        level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogLevelDungeonPlayCount));
    record->level = row[0] ? atoi(row[0]) : 0;
    record->total_charac_count = row[1] ? atoi(row[1]) : 0;
    record->total_dungeon_play_count = row[2] ? atoll(row[2]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogLevelDungeonPlayCount_GetAll(DBConnectionManager* manager, LogLevelDungeonPlayCount* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT level, total_charac_count, total_dungeon_play_count FROM log_level_dungeon_play_count");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogLevelDungeonPlayCount));
        records[count].level = row[0] ? atoi(row[0]) : 0;
        records[count].total_charac_count = row[1] ? atoi(row[1]) : 0;
        records[count].total_dungeon_play_count = row[2] ? atoll(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
