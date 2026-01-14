#include "log_cube_stat_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogCubeStat_Add(DBConnectionManager* manager, const LogCubeStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_cube_stat (occ_time, channel_no, level, item_index, type, item_count) "
        "VALUES ('%s', %d, %d, %d, %d, %d)",
        record->occ_time, record->channel_no, record->level, record->item_index, record->type, record->item_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogCubeStat_Get(DBConnectionManager* manager, const char* occ_time, int channel_no, int level, int item_index, int type, LogCubeStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, channel_no, level, item_index, type, item_count FROM log_cube_stat WHERE occ_time = '%s' AND channel_no = %d AND level = %d AND item_index = %d AND type = %d",
        occ_time, channel_no, level, item_index, type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogCubeStat));
    if (row[0]) strncpy(record->occ_time, row[0], sizeof(record->occ_time) - 1);
    record->channel_no = row[1] ? atoi(row[1]) : 0;
    record->level = row[2] ? atoi(row[2]) : 0;
    record->item_index = row[3] ? atoi(row[3]) : 0;
    record->type = row[4] ? atoi(row[4]) : 0;
    record->item_count = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogCubeStat_GetAll(DBConnectionManager* manager, LogCubeStat* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, channel_no, level, item_index, type, item_count FROM log_cube_stat");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogCubeStat));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].channel_no = row[1] ? atoi(row[1]) : 0;
        records[count].level = row[2] ? atoi(row[2]) : 0;
        records[count].item_index = row[3] ? atoi(row[3]) : 0;
        records[count].type = row[4] ? atoi(row[4]) : 0;
        records[count].item_count = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
