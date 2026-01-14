#include "game_channel_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int GameChannel_Add(DBConnectionManager* manager, const GameChannel* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO game_channel (gc_now, gc_ip, gc_port, gc_max, gc_game, gc_channel, gc_ch_group, gc_channeltype) "
        "VALUES (%d, '%s', %d, %d, %d, '%s', %d, '%s')",
        record->gc_now, record->gc_ip, record->gc_port, record->gc_max, record->gc_game, record->gc_channel, record->gc_ch_group, record->gc_channeltype);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GameChannel_Get(DBConnectionManager* manager, int gc_no, GameChannel* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT gc_no, gc_now, gc_ip, gc_port, gc_max, gc_game, gc_channel, gc_ch_group, gc_channeltype FROM game_channel WHERE gc_no = %d",
        gc_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(GameChannel));
    record->gc_no = row[0] ? atoi(row[0]) : 0;
    record->gc_now = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->gc_ip, row[2], sizeof(record->gc_ip) - 1);
    record->gc_port = row[3] ? atoi(row[3]) : 0;
    record->gc_max = row[4] ? atoi(row[4]) : 0;
    record->gc_game = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->gc_channel, row[6], sizeof(record->gc_channel) - 1);
    record->gc_ch_group = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->gc_channeltype, row[8], sizeof(record->gc_channeltype) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int GameChannel_GetAll(DBConnectionManager* manager, GameChannel* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT gc_no, gc_now, gc_ip, gc_port, gc_max, gc_game, gc_channel, gc_ch_group, gc_channeltype FROM game_channel");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(GameChannel));
        records[count].gc_no = row[0] ? atoi(row[0]) : 0;
        records[count].gc_now = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].gc_ip, row[2], sizeof(records[count].gc_ip) - 1);
        records[count].gc_port = row[3] ? atoi(row[3]) : 0;
        records[count].gc_max = row[4] ? atoi(row[4]) : 0;
        records[count].gc_game = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].gc_channel, row[6], sizeof(records[count].gc_channel) - 1);
        records[count].gc_ch_group = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].gc_channeltype, row[8], sizeof(records[count].gc_channeltype) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
