#include "dungeon_lev_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DungeonLevLog_Add(DBConnectionManager* manager, const DungeonLevLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dungeon_lev_log (occ_date, dungeon_id, sum_lev, used_coin, try_cnt, clear_cnt) "
        "VALUES ('%s', %d, %d, %d, %d, %d)",
        record->occ_date, record->dungeon_id, record->sum_lev, record->used_coin, record->try_cnt, record->clear_cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DungeonLevLog_Get(DBConnectionManager* manager, const char* occ_date, int dungeon_id, DungeonLevLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, dungeon_id, sum_lev, used_coin, try_cnt, clear_cnt FROM dungeon_lev_log WHERE occ_date = '%s' AND dungeon_id = %d",
        occ_date, dungeon_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DungeonLevLog));
    if (row[0]) strncpy(record->occ_date, row[0], sizeof(record->occ_date) - 1);
    record->dungeon_id = row[1] ? atoi(row[1]) : 0;
    record->sum_lev = row[2] ? atoi(row[2]) : 0;
    record->used_coin = row[3] ? atoi(row[3]) : 0;
    record->try_cnt = row[4] ? atoi(row[4]) : 0;
    record->clear_cnt = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int DungeonLevLog_GetAll(DBConnectionManager* manager, DungeonLevLog* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, dungeon_id, sum_lev, used_coin, try_cnt, clear_cnt FROM dungeon_lev_log");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DungeonLevLog));
        if (row[0]) strncpy(records[count].occ_date, row[0], sizeof(records[count].occ_date) - 1);
        records[count].dungeon_id = row[1] ? atoi(row[1]) : 0;
        records[count].sum_lev = row[2] ? atoi(row[2]) : 0;
        records[count].used_coin = row[3] ? atoi(row[3]) : 0;
        records[count].try_cnt = row[4] ? atoi(row[4]) : 0;
        records[count].clear_cnt = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
