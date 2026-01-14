#include "fight_village_lag_index_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int FightVillageLagIndex_Add(DBConnectionManager* manager, const FightVillageLagIndex* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO fight_village_lag_index (spec_id, occ_time, server_group, min_fps, avg_fps, max_fps, win_fps, full_fps, full_win_fps, full_win_nosync_fps, frame1, time1, frame2, time2, frame3, time3, frame4, time4, frame5, time5, frame6, time6, share_rate) "
        "VALUES (%d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d)",
        record->spec_id, record->occ_time, record->server_group, record->min_fps, record->avg_fps, record->max_fps, record->win_fps, record->full_fps, record->full_win_fps, record->full_win_nosync_fps, record->frame1, record->time1, record->frame2, record->time2, record->frame3, record->time3, record->frame4, record->time4, record->frame5, record->time5, record->frame6, record->time6, record->share_rate);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int FightVillageLagIndex_GetAll(DBConnectionManager* manager, FightVillageLagIndex* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[23];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT spec_id, occ_time, server_group, min_fps, avg_fps, max_fps, win_fps, full_fps, full_win_fps, full_win_nosync_fps, frame1, time1, frame2, time2, frame3, time3, frame4, time4, frame5, time5, frame6, time6, share_rate FROM fight_village_lag_index");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(FightVillageLagIndex));
        records[count].spec_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].occ_time, row[1], sizeof(records[count].occ_time) - 1);
        records[count].server_group = row[2] ? atoi(row[2]) : 0;
        records[count].min_fps = row[3] ? atoi(row[3]) : 0;
        records[count].avg_fps = row[4] ? atoi(row[4]) : 0;
        records[count].max_fps = row[5] ? atoi(row[5]) : 0;
        records[count].win_fps = row[6] ? atoi(row[6]) : 0;
        records[count].full_fps = row[7] ? atoi(row[7]) : 0;
        records[count].full_win_fps = row[8] ? atoi(row[8]) : 0;
        records[count].full_win_nosync_fps = row[9] ? atoi(row[9]) : 0;
        records[count].frame1 = row[10] ? atoi(row[10]) : 0;
        records[count].time1 = row[11] ? atof(row[11]) : 0;
        records[count].frame2 = row[12] ? atoi(row[12]) : 0;
        records[count].time2 = row[13] ? atof(row[13]) : 0;
        records[count].frame3 = row[14] ? atoi(row[14]) : 0;
        records[count].time3 = row[15] ? atof(row[15]) : 0;
        records[count].frame4 = row[16] ? atoi(row[16]) : 0;
        records[count].time4 = row[17] ? atof(row[17]) : 0;
        records[count].frame5 = row[18] ? atoi(row[18]) : 0;
        records[count].time5 = row[19] ? atof(row[19]) : 0;
        records[count].frame6 = row[20] ? atoi(row[20]) : 0;
        records[count].time6 = row[21] ? atof(row[21]) : 0;
        records[count].share_rate = row[22] ? atoi(row[22]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
