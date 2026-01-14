#include "power_war_user_rank_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int PowerWarUserRank_Add(DBConnectionManager* manager, const PowerWarUserRank* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO power_war_user_rank (server_id, rank, charac_no, power_war_point, power_side) "
        "VALUES (%d, %d, %d, %d, %d)",
        record->server_id, record->rank, record->charac_no, record->power_war_point, record->power_side);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PowerWarUserRank_Get(DBConnectionManager* manager, int charac_no, signed char server_id, PowerWarUserRank* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_id, rank, charac_no, power_war_point, power_side FROM power_war_user_rank WHERE charac_no = %d AND server_id = %d",
        charac_no, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(PowerWarUserRank));
    record->server_id = row[0] ? atoi(row[0]) : 0;
    record->rank = row[1] ? atoi(row[1]) : 0;
    record->charac_no = row[2] ? atoi(row[2]) : 0;
    record->power_war_point = row[3] ? atoi(row[3]) : 0;
    record->power_side = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int PowerWarUserRank_GetAll(DBConnectionManager* manager, PowerWarUserRank* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT server_id, rank, charac_no, power_war_point, power_side FROM power_war_user_rank");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(PowerWarUserRank));
        records[count].server_id = row[0] ? atoi(row[0]) : 0;
        records[count].rank = row[1] ? atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        records[count].power_war_point = row[3] ? atoi(row[3]) : 0;
        records[count].power_side = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
