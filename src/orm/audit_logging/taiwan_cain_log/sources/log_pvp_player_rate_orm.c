#include "log_pvp_player_rate_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogPvpPlayerRate_Add(DBConnectionManager* manager, const LogPvpPlayerRate* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_pvp_player_rate (channel_no, occ_date, single_one_to_one, single_one_to_one_other, team_one_to_one, team_one_to_one_other, relay_one_to_one, relay_one_to_one_other) "
        "VALUES (%d, '%s', %d, %d, %d, %d, %d, %d)",
        record->channel_no, record->occ_date, record->single_one_to_one, record->single_one_to_one_other, record->team_one_to_one, record->team_one_to_one_other, record->relay_one_to_one, record->relay_one_to_one_other);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogPvpPlayerRate_Get(DBConnectionManager* manager, int channel_no, const char* occ_date, LogPvpPlayerRate* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT channel_no, occ_date, single_one_to_one, single_one_to_one_other, team_one_to_one, team_one_to_one_other, relay_one_to_one, relay_one_to_one_other FROM log_pvp_player_rate WHERE channel_no = %d AND occ_date = '%s'",
        channel_no, occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogPvpPlayerRate));
    record->channel_no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->occ_date, row[1], sizeof(record->occ_date) - 1);
    record->single_one_to_one = row[2] ? atoi(row[2]) : 0;
    record->single_one_to_one_other = row[3] ? atoi(row[3]) : 0;
    record->team_one_to_one = row[4] ? atoi(row[4]) : 0;
    record->team_one_to_one_other = row[5] ? atoi(row[5]) : 0;
    record->relay_one_to_one = row[6] ? atoi(row[6]) : 0;
    record->relay_one_to_one_other = row[7] ? atoi(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogPvpPlayerRate_GetAll(DBConnectionManager* manager, LogPvpPlayerRate* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT channel_no, occ_date, single_one_to_one, single_one_to_one_other, team_one_to_one, team_one_to_one_other, relay_one_to_one, relay_one_to_one_other FROM log_pvp_player_rate");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogPvpPlayerRate));
        records[count].channel_no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].occ_date, row[1], sizeof(records[count].occ_date) - 1);
        records[count].single_one_to_one = row[2] ? atoi(row[2]) : 0;
        records[count].single_one_to_one_other = row[3] ? atoi(row[3]) : 0;
        records[count].team_one_to_one = row[4] ? atoi(row[4]) : 0;
        records[count].team_one_to_one_other = row[5] ? atoi(row[5]) : 0;
        records[count].relay_one_to_one = row[6] ? atoi(row[6]) : 0;
        records[count].relay_one_to_one_other = row[7] ? atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
