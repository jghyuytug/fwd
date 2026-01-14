#include "log_monstertower_layer_stat_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogMonstertowerLayerStat_Add(DBConnectionManager* manager, const LogMonstertowerLayerStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_monstertower_layer_stat (occ_date, key_id, layer, monster_diff, charac_job, charac_grow, level, play_cnt, success_cnt, play_time, uniq_cnt) "
        "VALUES ('%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->occ_date, record->key_id, record->layer, record->monster_diff, record->charac_job, record->charac_grow, record->level, record->play_cnt, record->success_cnt, record->play_time, record->uniq_cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogMonstertowerLayerStat_Get(DBConnectionManager* manager, const char* occ_date, int key_id, LogMonstertowerLayerStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, key_id, layer, monster_diff, charac_job, charac_grow, level, play_cnt, success_cnt, play_time, uniq_cnt FROM log_monstertower_layer_stat WHERE occ_date = '%s' AND key_id = %d",
        occ_date, key_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogMonstertowerLayerStat));
    if (row[0]) strncpy(record->occ_date, row[0], sizeof(record->occ_date) - 1);
    record->key_id = row[1] ? atoi(row[1]) : 0;
    record->layer = row[2] ? atoi(row[2]) : 0;
    record->monster_diff = row[3] ? atoi(row[3]) : 0;
    record->charac_job = row[4] ? atoi(row[4]) : 0;
    record->charac_grow = row[5] ? atoi(row[5]) : 0;
    record->level = row[6] ? atoi(row[6]) : 0;
    record->play_cnt = row[7] ? atoi(row[7]) : 0;
    record->success_cnt = row[8] ? atoi(row[8]) : 0;
    record->play_time = row[9] ? atoll(row[9]) : 0;
    record->uniq_cnt = row[10] ? atoi(row[10]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogMonstertowerLayerStat_GetAll(DBConnectionManager* manager, LogMonstertowerLayerStat* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, key_id, layer, monster_diff, charac_job, charac_grow, level, play_cnt, success_cnt, play_time, uniq_cnt FROM log_monstertower_layer_stat");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogMonstertowerLayerStat));
        if (row[0]) strncpy(records[count].occ_date, row[0], sizeof(records[count].occ_date) - 1);
        records[count].key_id = row[1] ? atoi(row[1]) : 0;
        records[count].layer = row[2] ? atoi(row[2]) : 0;
        records[count].monster_diff = row[3] ? atoi(row[3]) : 0;
        records[count].charac_job = row[4] ? atoi(row[4]) : 0;
        records[count].charac_grow = row[5] ? atoi(row[5]) : 0;
        records[count].level = row[6] ? atoi(row[6]) : 0;
        records[count].play_cnt = row[7] ? atoi(row[7]) : 0;
        records[count].success_cnt = row[8] ? atoi(row[8]) : 0;
        records[count].play_time = row[9] ? atoll(row[9]) : 0;
        records[count].uniq_cnt = row[10] ? atoi(row[10]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
