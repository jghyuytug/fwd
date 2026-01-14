#include "lag_stat_dungeon_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LagStatDungeon_Add(DBConnectionManager* manager, const LagStatDungeon* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO lag_stat_dungeon (occ_time, server_id, dungeon_idx, first_average, first_deviation, first_count, boss_average, boss_deviation, boss_count) "
        "VALUES ('%s', %d, %d, %d, %d, %d, %d, %d, %d)",
        record->occ_time, record->server_id, record->dungeon_idx, record->first_average, record->first_deviation, record->first_count, record->boss_average, record->boss_deviation, record->boss_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LagStatDungeon_Get(DBConnectionManager* manager, const char* occ_time, signed char server_id, int dungeon_idx, LagStatDungeon* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, server_id, dungeon_idx, first_average, first_deviation, first_count, boss_average, boss_deviation, boss_count FROM lag_stat_dungeon WHERE occ_time = '%s' AND server_id = %d AND dungeon_idx = %d",
        occ_time, server_id, dungeon_idx);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LagStatDungeon));
    if (row[0]) strncpy(record->occ_time, row[0], sizeof(record->occ_time) - 1);
    record->server_id = row[1] ? atoi(row[1]) : 0;
    record->dungeon_idx = row[2] ? atoi(row[2]) : 0;
    record->first_average = row[3] ? atoi(row[3]) : 0;
    record->first_deviation = row[4] ? atoi(row[4]) : 0;
    record->first_count = row[5] ? atoi(row[5]) : 0;
    record->boss_average = row[6] ? atoi(row[6]) : 0;
    record->boss_deviation = row[7] ? atoi(row[7]) : 0;
    record->boss_count = row[8] ? atoi(row[8]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LagStatDungeon_GetAll(DBConnectionManager* manager, LagStatDungeon* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, server_id, dungeon_idx, first_average, first_deviation, first_count, boss_average, boss_deviation, boss_count FROM lag_stat_dungeon");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LagStatDungeon));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].server_id = row[1] ? atoi(row[1]) : 0;
        records[count].dungeon_idx = row[2] ? atoi(row[2]) : 0;
        records[count].first_average = row[3] ? atoi(row[3]) : 0;
        records[count].first_deviation = row[4] ? atoi(row[4]) : 0;
        records[count].first_count = row[5] ? atoi(row[5]) : 0;
        records[count].boss_average = row[6] ? atoi(row[6]) : 0;
        records[count].boss_deviation = row[7] ? atoi(row[7]) : 0;
        records[count].boss_count = row[8] ? atoi(row[8]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
