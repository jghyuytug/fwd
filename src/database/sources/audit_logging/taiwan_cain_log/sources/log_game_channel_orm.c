#include "log_game_channel_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogGameChannel_Add(DBConnectionManager* manager, const LogGameChannel* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_game_channel (gc_up_time, gc_game, gc_no, gc_channel, gc_ch_group, gc_ip, gc_new, gc_now, gc_out) "
        "VALUES ('%s', %d, %d, '%s', %d, '%s', %d, %d, %d)",
        record->gc_up_time, record->gc_game, record->gc_no, record->gc_channel, record->gc_ch_group, record->gc_ip, record->gc_new, record->gc_now, record->gc_out);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogGameChannel_Get(DBConnectionManager* manager, const char* gc_up_time, signed char gc_game, int gc_no, LogGameChannel* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT gc_up_time, gc_game, gc_no, gc_channel, gc_ch_group, gc_ip, gc_new, gc_now, gc_out FROM log_game_channel WHERE gc_up_time = '%s' AND gc_game = %d AND gc_no = %d",
        gc_up_time, gc_game, gc_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogGameChannel));
    if (row[0]) strncpy(record->gc_up_time, row[0], sizeof(record->gc_up_time) - 1);
    record->gc_game = row[1] ? atoi(row[1]) : 0;
    record->gc_no = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->gc_channel, row[3], sizeof(record->gc_channel) - 1);
    record->gc_ch_group = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->gc_ip, row[5], sizeof(record->gc_ip) - 1);
    record->gc_new = row[6] ? atoi(row[6]) : 0;
    record->gc_now = row[7] ? atoi(row[7]) : 0;
    record->gc_out = row[8] ? atoi(row[8]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogGameChannel_GetAll(DBConnectionManager* manager, LogGameChannel* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT gc_up_time, gc_game, gc_no, gc_channel, gc_ch_group, gc_ip, gc_new, gc_now, gc_out FROM log_game_channel");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogGameChannel));
        if (row[0]) strncpy(records[count].gc_up_time, row[0], sizeof(records[count].gc_up_time) - 1);
        records[count].gc_game = row[1] ? atoi(row[1]) : 0;
        records[count].gc_no = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].gc_channel, row[3], sizeof(records[count].gc_channel) - 1);
        records[count].gc_ch_group = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].gc_ip, row[5], sizeof(records[count].gc_ip) - 1);
        records[count].gc_new = row[6] ? atoi(row[6]) : 0;
        records[count].gc_now = row[7] ? atoi(row[7]) : 0;
        records[count].gc_out = row[8] ? atoi(row[8]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
