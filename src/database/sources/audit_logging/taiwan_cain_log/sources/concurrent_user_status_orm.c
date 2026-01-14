#include "concurrent_user_status_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ConcurrentUserStatus_Add(DBConnectionManager* manager, const ConcurrentUserStatus* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO concurrent_user_status (occ_time, player_status, dungeon_index, channel_no, occ_count) "
        "VALUES ('%s', %d, %d, %d, %d)",
        record->occ_time, record->player_status, record->dungeon_index, record->channel_no, record->occ_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ConcurrentUserStatus_Get(DBConnectionManager* manager, const char* occ_time, signed char player_status, int dungeon_index, int channel_no, ConcurrentUserStatus* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, player_status, dungeon_index, channel_no, occ_count FROM concurrent_user_status WHERE occ_time = '%s' AND player_status = %d AND dungeon_index = %d AND channel_no = %d",
        occ_time, player_status, dungeon_index, channel_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ConcurrentUserStatus));
    if (row[0]) strncpy(record->occ_time, row[0], sizeof(record->occ_time) - 1);
    record->player_status = row[1] ? atoi(row[1]) : 0;
    record->dungeon_index = row[2] ? atoi(row[2]) : 0;
    record->channel_no = row[3] ? atoi(row[3]) : 0;
    record->occ_count = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int ConcurrentUserStatus_GetAll(DBConnectionManager* manager, ConcurrentUserStatus* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, player_status, dungeon_index, channel_no, occ_count FROM concurrent_user_status");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ConcurrentUserStatus));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].player_status = row[1] ? atoi(row[1]) : 0;
        records[count].dungeon_index = row[2] ? atoi(row[2]) : 0;
        records[count].channel_no = row[3] ? atoi(row[3]) : 0;
        records[count].occ_count = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
