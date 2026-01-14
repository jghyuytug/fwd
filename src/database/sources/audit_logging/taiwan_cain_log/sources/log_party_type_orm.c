#include "log_party_type_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogPartyType_Add(DBConnectionManager* manager, const LogPartyType* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_party_type (channel_no, occ_date, guild_count, member_count, helped_count, normal_count, member_helped_count) "
        "VALUES (%d, '%s', %d, %d, %d, %d, %d)",
        record->channel_no, record->occ_date, record->guild_count, record->member_count, record->helped_count, record->normal_count, record->member_helped_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogPartyType_Get(DBConnectionManager* manager, int channel_no, const char* occ_date, LogPartyType* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT channel_no, occ_date, guild_count, member_count, helped_count, normal_count, member_helped_count FROM log_party_type WHERE channel_no = %d AND occ_date = '%s'",
        channel_no, occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogPartyType));
    record->channel_no = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->occ_date, row[1], sizeof(record->occ_date) - 1);
    record->guild_count = row[2] ? atoi(row[2]) : 0;
    record->member_count = row[3] ? atoi(row[3]) : 0;
    record->helped_count = row[4] ? atoi(row[4]) : 0;
    record->normal_count = row[5] ? atoi(row[5]) : 0;
    record->member_helped_count = row[6] ? atoi(row[6]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogPartyType_GetAll(DBConnectionManager* manager, LogPartyType* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT channel_no, occ_date, guild_count, member_count, helped_count, normal_count, member_helped_count FROM log_party_type");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogPartyType));
        records[count].channel_no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].occ_date, row[1], sizeof(records[count].occ_date) - 1);
        records[count].guild_count = row[2] ? atoi(row[2]) : 0;
        records[count].member_count = row[3] ? atoi(row[3]) : 0;
        records[count].helped_count = row[4] ? atoi(row[4]) : 0;
        records[count].normal_count = row[5] ? atoi(row[5]) : 0;
        records[count].member_helped_count = row[6] ? atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
