#include "village_lag_index_daily_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int VillageLagIndexDaily_Add(DBConnectionManager* manager, const VillageLagIndexDaily* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO village_lag_index_daily (occ_date, spec_id, win_fps, win_fps_cnt, full_fps, full_fps_cnt, full_win_fps, full_win_fps_cnt, full_win_nosync_fps, full_win_nosync_fps_cnt) "
        "VALUES ('%s', %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->occ_date, record->spec_id, record->win_fps, record->win_fps_cnt, record->full_fps, record->full_fps_cnt, record->full_win_fps, record->full_win_fps_cnt, record->full_win_nosync_fps, record->full_win_nosync_fps_cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int VillageLagIndexDaily_Get(DBConnectionManager* manager, const char* occ_date, int spec_id, VillageLagIndexDaily* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, spec_id, win_fps, win_fps_cnt, full_fps, full_fps_cnt, full_win_fps, full_win_fps_cnt, full_win_nosync_fps, full_win_nosync_fps_cnt FROM village_lag_index_daily WHERE occ_date = '%s' AND spec_id = %d",
        occ_date, spec_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(VillageLagIndexDaily));
    if (row[0]) strncpy(record->occ_date, row[0], sizeof(record->occ_date) - 1);
    record->spec_id = row[1] ? atoi(row[1]) : 0;
    record->win_fps = row[2] ? atoi(row[2]) : 0;
    record->win_fps_cnt = row[3] ? atoi(row[3]) : 0;
    record->full_fps = row[4] ? atoi(row[4]) : 0;
    record->full_fps_cnt = row[5] ? atoi(row[5]) : 0;
    record->full_win_fps = row[6] ? atoi(row[6]) : 0;
    record->full_win_fps_cnt = row[7] ? atoi(row[7]) : 0;
    record->full_win_nosync_fps = row[8] ? atoi(row[8]) : 0;
    record->full_win_nosync_fps_cnt = row[9] ? atoi(row[9]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int VillageLagIndexDaily_GetAll(DBConnectionManager* manager, VillageLagIndexDaily* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, spec_id, win_fps, win_fps_cnt, full_fps, full_fps_cnt, full_win_fps, full_win_fps_cnt, full_win_nosync_fps, full_win_nosync_fps_cnt FROM village_lag_index_daily");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(VillageLagIndexDaily));
        if (row[0]) strncpy(records[count].occ_date, row[0], sizeof(records[count].occ_date) - 1);
        records[count].spec_id = row[1] ? atoi(row[1]) : 0;
        records[count].win_fps = row[2] ? atoi(row[2]) : 0;
        records[count].win_fps_cnt = row[3] ? atoi(row[3]) : 0;
        records[count].full_fps = row[4] ? atoi(row[4]) : 0;
        records[count].full_fps_cnt = row[5] ? atoi(row[5]) : 0;
        records[count].full_win_fps = row[6] ? atoi(row[6]) : 0;
        records[count].full_win_fps_cnt = row[7] ? atoi(row[7]) : 0;
        records[count].full_win_nosync_fps = row[8] ? atoi(row[8]) : 0;
        records[count].full_win_nosync_fps_cnt = row[9] ? atoi(row[9]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
