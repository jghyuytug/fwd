#include "log_dungeon_skill_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogDungeonSkill_Add(DBConnectionManager* manager, const LogDungeonSkill* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_dungeon_skill (channel_no, dungeon_index, dungeon_diff, charac_job, charac_grow, skill_index, skill_use, skill_count, skill_cool_time, last_time) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, '%s')",
        record->channel_no, record->dungeon_index, record->dungeon_diff, record->charac_job, record->charac_grow, record->skill_index, record->skill_use, record->skill_count, record->skill_cool_time, record->last_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogDungeonSkill_Get(DBConnectionManager* manager, const char* last_time, int channel_no, int dungeon_index, int dungeon_diff, int charac_job, int charac_grow, int skill_index, LogDungeonSkill* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT channel_no, dungeon_index, dungeon_diff, charac_job, charac_grow, skill_index, skill_use, skill_count, skill_cool_time, last_time FROM log_dungeon_skill WHERE last_time = '%s' AND channel_no = %d AND dungeon_index = %d AND dungeon_diff = %d AND charac_job = %d AND charac_grow = %d AND skill_index = %d",
        last_time, channel_no, dungeon_index, dungeon_diff, charac_job, charac_grow, skill_index);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogDungeonSkill));
    record->channel_no = row[0] ? atoi(row[0]) : 0;
    record->dungeon_index = row[1] ? atoi(row[1]) : 0;
    record->dungeon_diff = row[2] ? atoi(row[2]) : 0;
    record->charac_job = row[3] ? atoi(row[3]) : 0;
    record->charac_grow = row[4] ? atoi(row[4]) : 0;
    record->skill_index = row[5] ? atoi(row[5]) : 0;
    record->skill_use = row[6] ? atoi(row[6]) : 0;
    record->skill_count = row[7] ? atoi(row[7]) : 0;
    record->skill_cool_time = row[8] ? atoi(row[8]) : 0;
    if (row[9]) strncpy(record->last_time, row[9], sizeof(record->last_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int LogDungeonSkill_GetAll(DBConnectionManager* manager, LogDungeonSkill* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT channel_no, dungeon_index, dungeon_diff, charac_job, charac_grow, skill_index, skill_use, skill_count, skill_cool_time, last_time FROM log_dungeon_skill");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogDungeonSkill));
        records[count].channel_no = row[0] ? atoi(row[0]) : 0;
        records[count].dungeon_index = row[1] ? atoi(row[1]) : 0;
        records[count].dungeon_diff = row[2] ? atoi(row[2]) : 0;
        records[count].charac_job = row[3] ? atoi(row[3]) : 0;
        records[count].charac_grow = row[4] ? atoi(row[4]) : 0;
        records[count].skill_index = row[5] ? atoi(row[5]) : 0;
        records[count].skill_use = row[6] ? atoi(row[6]) : 0;
        records[count].skill_count = row[7] ? atoi(row[7]) : 0;
        records[count].skill_cool_time = row[8] ? atoi(row[8]) : 0;
        if (row[9]) strncpy(records[count].last_time, row[9], sizeof(records[count].last_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
