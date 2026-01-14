#include "log_dungeon_monster_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogDungeonMonster_Add(DBConnectionManager* manager, const LogDungeonMonster* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_dungeon_monster (channel_no, dungeon_index, dungeon_diff, monster_index, damage_total, damage_count, last_time) "
        "VALUES (%d, %d, %d, %d, %d, %d, '%s')",
        record->channel_no, record->dungeon_index, record->dungeon_diff, record->monster_index, record->damage_total, record->damage_count, record->last_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogDungeonMonster_Get(DBConnectionManager* manager, const char* last_time, int channel_no, int dungeon_index, int dungeon_diff, int monster_index, LogDungeonMonster* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT channel_no, dungeon_index, dungeon_diff, monster_index, damage_total, damage_count, last_time FROM log_dungeon_monster WHERE last_time = '%s' AND channel_no = %d AND dungeon_index = %d AND dungeon_diff = %d AND monster_index = %d",
        last_time, channel_no, dungeon_index, dungeon_diff, monster_index);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogDungeonMonster));
    record->channel_no = row[0] ? atoi(row[0]) : 0;
    record->dungeon_index = row[1] ? atoi(row[1]) : 0;
    record->dungeon_diff = row[2] ? atoi(row[2]) : 0;
    record->monster_index = row[3] ? atoi(row[3]) : 0;
    record->damage_total = row[4] ? atoi(row[4]) : 0;
    record->damage_count = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->last_time, row[6], sizeof(record->last_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int LogDungeonMonster_GetAll(DBConnectionManager* manager, LogDungeonMonster* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT channel_no, dungeon_index, dungeon_diff, monster_index, damage_total, damage_count, last_time FROM log_dungeon_monster");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogDungeonMonster));
        records[count].channel_no = row[0] ? atoi(row[0]) : 0;
        records[count].dungeon_index = row[1] ? atoi(row[1]) : 0;
        records[count].dungeon_diff = row[2] ? atoi(row[2]) : 0;
        records[count].monster_index = row[3] ? atoi(row[3]) : 0;
        records[count].damage_total = row[4] ? atoi(row[4]) : 0;
        records[count].damage_count = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].last_time, row[6], sizeof(records[count].last_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
