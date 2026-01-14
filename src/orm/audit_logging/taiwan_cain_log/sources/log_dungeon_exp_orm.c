#include "log_dungeon_exp_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogDungeonExp_Add(DBConnectionManager* manager, const LogDungeonExp* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_dungeon_exp (occ_time, channel_no, lev, exp, fatigue, user_count) "
        "VALUES ('%s', %d, %d, %d, %d, %d)",
        record->occ_time, record->channel_no, record->lev, record->exp, record->fatigue, record->user_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogDungeonExp_Get(DBConnectionManager* manager, const char* occ_time, int channel_no, int lev, LogDungeonExp* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, channel_no, lev, exp, fatigue, user_count FROM log_dungeon_exp WHERE occ_time = '%s' AND channel_no = %d AND lev = %d",
        occ_time, channel_no, lev);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogDungeonExp));
    if (row[0]) strncpy(record->occ_time, row[0], sizeof(record->occ_time) - 1);
    record->channel_no = row[1] ? atoi(row[1]) : 0;
    record->lev = row[2] ? atoi(row[2]) : 0;
    record->exp = row[3] ? atoll(row[3]) : 0;
    record->fatigue = row[4] ? atoll(row[4]) : 0;
    record->user_count = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogDungeonExp_GetAll(DBConnectionManager* manager, LogDungeonExp* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, channel_no, lev, exp, fatigue, user_count FROM log_dungeon_exp");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogDungeonExp));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].channel_no = row[1] ? atoi(row[1]) : 0;
        records[count].lev = row[2] ? atoi(row[2]) : 0;
        records[count].exp = row[3] ? atoll(row[3]) : 0;
        records[count].fatigue = row[4] ? atoll(row[4]) : 0;
        records[count].user_count = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
