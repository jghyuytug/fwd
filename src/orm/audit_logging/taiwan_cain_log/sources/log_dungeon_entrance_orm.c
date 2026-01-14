#include "log_dungeon_entrance_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogDungeonEntrance_Add(DBConnectionManager* manager, const LogDungeonEntrance* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_dungeon_entrance (occ_date, channel_no, dungeon_index, enter_count, exercise_count) "
        "VALUES ('%s', %d, %d, %d, %d)",
        record->occ_date, record->channel_no, record->dungeon_index, record->enter_count, record->exercise_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogDungeonEntrance_Get(DBConnectionManager* manager, const char* occ_date, short channel_no, short dungeon_index, LogDungeonEntrance* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, channel_no, dungeon_index, enter_count, exercise_count FROM log_dungeon_entrance WHERE occ_date = '%s' AND channel_no = %d AND dungeon_index = %d",
        occ_date, channel_no, dungeon_index);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogDungeonEntrance));
    if (row[0]) strncpy(record->occ_date, row[0], sizeof(record->occ_date) - 1);
    record->channel_no = row[1] ? atoi(row[1]) : 0;
    record->dungeon_index = row[2] ? atoi(row[2]) : 0;
    record->enter_count = row[3] ? atoi(row[3]) : 0;
    record->exercise_count = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogDungeonEntrance_GetAll(DBConnectionManager* manager, LogDungeonEntrance* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, channel_no, dungeon_index, enter_count, exercise_count FROM log_dungeon_entrance");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogDungeonEntrance));
        if (row[0]) strncpy(records[count].occ_date, row[0], sizeof(records[count].occ_date) - 1);
        records[count].channel_no = row[1] ? atoi(row[1]) : 0;
        records[count].dungeon_index = row[2] ? atoi(row[2]) : 0;
        records[count].enter_count = row[3] ? atoi(row[3]) : 0;
        records[count].exercise_count = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
