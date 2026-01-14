#include "common_index_daily_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CommonIndexDaily_Add(DBConnectionManager* manager, const CommonIndexDaily* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO common_index_daily (occ_date, server_group, spec_id, share_rate, crash_village, crash_dungeon, crash_challenge, crash_wararea, crash_fight_village, crash_dead_tower) "
        "VALUES ('%s', %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->occ_date, record->server_group, record->spec_id, record->share_rate, record->crash_village, record->crash_dungeon, record->crash_challenge, record->crash_wararea, record->crash_fight_village, record->crash_dead_tower);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CommonIndexDaily_Get(DBConnectionManager* manager, const char* occ_date, signed char server_group, int spec_id, CommonIndexDaily* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, server_group, spec_id, share_rate, crash_village, crash_dungeon, crash_challenge, crash_wararea, crash_fight_village, crash_dead_tower FROM common_index_daily WHERE occ_date = '%s' AND server_group = %d AND spec_id = %d",
        occ_date, server_group, spec_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CommonIndexDaily));
    if (row[0]) strncpy(record->occ_date, row[0], sizeof(record->occ_date) - 1);
    record->server_group = row[1] ? atoi(row[1]) : 0;
    record->spec_id = row[2] ? atoi(row[2]) : 0;
    record->share_rate = row[3] ? atoi(row[3]) : 0;
    record->crash_village = row[4] ? atoi(row[4]) : 0;
    record->crash_dungeon = row[5] ? atoi(row[5]) : 0;
    record->crash_challenge = row[6] ? atoi(row[6]) : 0;
    record->crash_wararea = row[7] ? atoi(row[7]) : 0;
    record->crash_fight_village = row[8] ? atoi(row[8]) : 0;
    record->crash_dead_tower = row[9] ? atoi(row[9]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int CommonIndexDaily_GetAll(DBConnectionManager* manager, CommonIndexDaily* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, server_group, spec_id, share_rate, crash_village, crash_dungeon, crash_challenge, crash_wararea, crash_fight_village, crash_dead_tower FROM common_index_daily");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CommonIndexDaily));
        if (row[0]) strncpy(records[count].occ_date, row[0], sizeof(records[count].occ_date) - 1);
        records[count].server_group = row[1] ? atoi(row[1]) : 0;
        records[count].spec_id = row[2] ? atoi(row[2]) : 0;
        records[count].share_rate = row[3] ? atoi(row[3]) : 0;
        records[count].crash_village = row[4] ? atoi(row[4]) : 0;
        records[count].crash_dungeon = row[5] ? atoi(row[5]) : 0;
        records[count].crash_challenge = row[6] ? atoi(row[6]) : 0;
        records[count].crash_wararea = row[7] ? atoi(row[7]) : 0;
        records[count].crash_fight_village = row[8] ? atoi(row[8]) : 0;
        records[count].crash_dead_tower = row[9] ? atoi(row[9]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
