#include "common_index_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CommonIndex_Add(DBConnectionManager* manager, const CommonIndex* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO common_index (spec_id, occ_time, server_group, share_rate, crash_count, village_to_dungeon_lag, dungeon_to_village_lag, crash_village, crash_dungeon, crash_challenge, crash_wararea, crash_fight_village, crash_dead_tower, crash_channel, crash_chaos, crash_load) "
        "VALUES (%d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->spec_id, record->occ_time, record->server_group, record->share_rate, record->crash_count, record->village_to_dungeon_lag, record->dungeon_to_village_lag, record->crash_village, record->crash_dungeon, record->crash_challenge, record->crash_wararea, record->crash_fight_village, record->crash_dead_tower, record->crash_channel, record->crash_chaos, record->crash_load);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CommonIndex_GetAll(DBConnectionManager* manager, CommonIndex* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[16];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT spec_id, occ_time, server_group, share_rate, crash_count, village_to_dungeon_lag, dungeon_to_village_lag, crash_village, crash_dungeon, crash_challenge, crash_wararea, crash_fight_village, crash_dead_tower, crash_channel, crash_chaos, crash_load FROM common_index");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CommonIndex));
        records[count].spec_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].occ_time, row[1], sizeof(records[count].occ_time) - 1);
        records[count].server_group = row[2] ? atoi(row[2]) : 0;
        records[count].share_rate = row[3] ? atoi(row[3]) : 0;
        records[count].crash_count = row[4] ? atoi(row[4]) : 0;
        records[count].village_to_dungeon_lag = row[5] ? atoi(row[5]) : 0;
        records[count].dungeon_to_village_lag = row[6] ? atoi(row[6]) : 0;
        records[count].crash_village = row[7] ? atoi(row[7]) : 0;
        records[count].crash_dungeon = row[8] ? atoi(row[8]) : 0;
        records[count].crash_challenge = row[9] ? atoi(row[9]) : 0;
        records[count].crash_wararea = row[10] ? atoi(row[10]) : 0;
        records[count].crash_fight_village = row[11] ? atoi(row[11]) : 0;
        records[count].crash_dead_tower = row[12] ? atoi(row[12]) : 0;
        records[count].crash_channel = row[13] ? atoi(row[13]) : 0;
        records[count].crash_chaos = row[14] ? atoi(row[14]) : 0;
        records[count].crash_load = row[15] ? atoi(row[15]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
