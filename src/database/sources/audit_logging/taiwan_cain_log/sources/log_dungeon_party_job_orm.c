#include "log_dungeon_party_job_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogDungeonPartyJob_Add(DBConnectionManager* manager, const LogDungeonPartyJob* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_dungeon_party_job (channel_no, dungeon_index, dungeon_diff, dungeon_standard_level, abuse_party, balkun_party, charac_job, charac_grow, party_user_count, success, last_time, job_count, rank) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', %d, %d)",
        record->channel_no, record->dungeon_index, record->dungeon_diff, record->dungeon_standard_level, record->abuse_party, record->balkun_party, record->charac_job, record->charac_grow, record->party_user_count, record->success, record->last_time, record->job_count, record->rank);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogDungeonPartyJob_Get(DBConnectionManager* manager, const char* last_time, int channel_no, int dungeon_index, int dungeon_diff, signed char dungeon_standard_level, signed char abuse_party, signed char balkun_party, int charac_job, int charac_grow, int party_user_count, int success, LogDungeonPartyJob* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT channel_no, dungeon_index, dungeon_diff, dungeon_standard_level, abuse_party, balkun_party, charac_job, charac_grow, party_user_count, success, last_time, job_count, rank FROM log_dungeon_party_job WHERE last_time = '%s' AND channel_no = %d AND dungeon_index = %d AND dungeon_diff = %d AND dungeon_standard_level = %d AND abuse_party = %d AND balkun_party = %d AND charac_job = %d AND charac_grow = %d AND party_user_count = %d AND success = %d",
        last_time, channel_no, dungeon_index, dungeon_diff, dungeon_standard_level, abuse_party, balkun_party, charac_job, charac_grow, party_user_count, success);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogDungeonPartyJob));
    record->channel_no = row[0] ? atoi(row[0]) : 0;
    record->dungeon_index = row[1] ? atoi(row[1]) : 0;
    record->dungeon_diff = row[2] ? atoi(row[2]) : 0;
    record->dungeon_standard_level = row[3] ? atoi(row[3]) : 0;
    record->abuse_party = row[4] ? atoi(row[4]) : 0;
    record->balkun_party = row[5] ? atoi(row[5]) : 0;
    record->charac_job = row[6] ? atoi(row[6]) : 0;
    record->charac_grow = row[7] ? atoi(row[7]) : 0;
    record->party_user_count = row[8] ? atoi(row[8]) : 0;
    record->success = row[9] ? atoi(row[9]) : 0;
    if (row[10]) strncpy(record->last_time, row[10], sizeof(record->last_time) - 1);
    record->job_count = row[11] ? atoi(row[11]) : 0;
    record->rank = row[12] ? atoi(row[12]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogDungeonPartyJob_GetAll(DBConnectionManager* manager, LogDungeonPartyJob* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT channel_no, dungeon_index, dungeon_diff, dungeon_standard_level, abuse_party, balkun_party, charac_job, charac_grow, party_user_count, success, last_time, job_count, rank FROM log_dungeon_party_job");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogDungeonPartyJob));
        records[count].channel_no = row[0] ? atoi(row[0]) : 0;
        records[count].dungeon_index = row[1] ? atoi(row[1]) : 0;
        records[count].dungeon_diff = row[2] ? atoi(row[2]) : 0;
        records[count].dungeon_standard_level = row[3] ? atoi(row[3]) : 0;
        records[count].abuse_party = row[4] ? atoi(row[4]) : 0;
        records[count].balkun_party = row[5] ? atoi(row[5]) : 0;
        records[count].charac_job = row[6] ? atoi(row[6]) : 0;
        records[count].charac_grow = row[7] ? atoi(row[7]) : 0;
        records[count].party_user_count = row[8] ? atoi(row[8]) : 0;
        records[count].success = row[9] ? atoi(row[9]) : 0;
        if (row[10]) strncpy(records[count].last_time, row[10], sizeof(records[count].last_time) - 1);
        records[count].job_count = row[11] ? atoi(row[11]) : 0;
        records[count].rank = row[12] ? atoi(row[12]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
