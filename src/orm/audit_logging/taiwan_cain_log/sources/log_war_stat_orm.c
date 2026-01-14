#include "log_war_stat_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogWarStat_Add(DBConnectionManager* manager, const LogWarStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_war_stat (occ_date, channel_no, war_zone_cnt, peace_zone_cnt, rare_item_cnt, unique_item_cnt) "
        "VALUES ('%s', %d, %d, %d, %d, %d)",
        record->occ_date, record->channel_no, record->war_zone_cnt, record->peace_zone_cnt, record->rare_item_cnt, record->unique_item_cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogWarStat_Get(DBConnectionManager* manager, const char* occ_date, int channel_no, LogWarStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, channel_no, war_zone_cnt, peace_zone_cnt, rare_item_cnt, unique_item_cnt FROM log_war_stat WHERE occ_date = '%s' AND channel_no = %d",
        occ_date, channel_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogWarStat));
    if (row[0]) strncpy(record->occ_date, row[0], sizeof(record->occ_date) - 1);
    record->channel_no = row[1] ? atoi(row[1]) : 0;
    record->war_zone_cnt = row[2] ? atoi(row[2]) : 0;
    record->peace_zone_cnt = row[3] ? atoi(row[3]) : 0;
    record->rare_item_cnt = row[4] ? atoi(row[4]) : 0;
    record->unique_item_cnt = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogWarStat_GetAll(DBConnectionManager* manager, LogWarStat* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, channel_no, war_zone_cnt, peace_zone_cnt, rare_item_cnt, unique_item_cnt FROM log_war_stat");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogWarStat));
        if (row[0]) strncpy(records[count].occ_date, row[0], sizeof(records[count].occ_date) - 1);
        records[count].channel_no = row[1] ? atoi(row[1]) : 0;
        records[count].war_zone_cnt = row[2] ? atoi(row[2]) : 0;
        records[count].peace_zone_cnt = row[3] ? atoi(row[3]) : 0;
        records[count].rare_item_cnt = row[4] ? atoi(row[4]) : 0;
        records[count].unique_item_cnt = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
