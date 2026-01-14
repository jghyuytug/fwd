#include "p2p_statistics_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int P2PStatistics_Add(DBConnectionManager* manager, const P2PStatistics* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO p2p_statistics (occ_time, server_group, p2p_user, p2p_min_ping, p2p_max_ping, p2p_avg_ping, p2p_over_ping_100, p2p_over_ping_200, p2p_over_ping_300, p2p_over_ping_400, relay_user, relay_min_ping, relay_max_ping, relay_avg_ping, relay_over_ping_100, relay_over_ping_200, relay_over_ping_300, relay_over_ping_400) "
        "VALUES ('%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->occ_time, record->server_group, record->p2p_user, record->p2p_min_ping, record->p2p_max_ping, record->p2p_avg_ping, record->p2p_over_ping_100, record->p2p_over_ping_200, record->p2p_over_ping_300, record->p2p_over_ping_400, record->relay_user, record->relay_min_ping, record->relay_max_ping, record->relay_avg_ping, record->relay_over_ping_100, record->relay_over_ping_200, record->relay_over_ping_300, record->relay_over_ping_400);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int P2PStatistics_Get(DBConnectionManager* manager, const char* occ_time, signed char server_group, P2PStatistics* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[18];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, server_group, p2p_user, p2p_min_ping, p2p_max_ping, p2p_avg_ping, p2p_over_ping_100, p2p_over_ping_200, p2p_over_ping_300, p2p_over_ping_400, relay_user, relay_min_ping, relay_max_ping, relay_avg_ping, relay_over_ping_100, relay_over_ping_200, relay_over_ping_300, relay_over_ping_400 FROM p2p_statistics WHERE occ_time = '%s' AND server_group = %d",
        occ_time, server_group);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(P2PStatistics));
    if (row[0]) strncpy(record->occ_time, row[0], sizeof(record->occ_time) - 1);
    record->server_group = row[1] ? atoi(row[1]) : 0;
    record->p2p_user = row[2] ? atoi(row[2]) : 0;
    record->p2p_min_ping = row[3] ? atoi(row[3]) : 0;
    record->p2p_max_ping = row[4] ? atoi(row[4]) : 0;
    record->p2p_avg_ping = row[5] ? atoi(row[5]) : 0;
    record->p2p_over_ping_100 = row[6] ? atoi(row[6]) : 0;
    record->p2p_over_ping_200 = row[7] ? atoi(row[7]) : 0;
    record->p2p_over_ping_300 = row[8] ? atoi(row[8]) : 0;
    record->p2p_over_ping_400 = row[9] ? atoi(row[9]) : 0;
    record->relay_user = row[10] ? atoi(row[10]) : 0;
    record->relay_min_ping = row[11] ? atoi(row[11]) : 0;
    record->relay_max_ping = row[12] ? atoi(row[12]) : 0;
    record->relay_avg_ping = row[13] ? atoi(row[13]) : 0;
    record->relay_over_ping_100 = row[14] ? atoi(row[14]) : 0;
    record->relay_over_ping_200 = row[15] ? atoi(row[15]) : 0;
    record->relay_over_ping_300 = row[16] ? atoi(row[16]) : 0;
    record->relay_over_ping_400 = row[17] ? atoi(row[17]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int P2PStatistics_GetAll(DBConnectionManager* manager, P2PStatistics* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[18];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, server_group, p2p_user, p2p_min_ping, p2p_max_ping, p2p_avg_ping, p2p_over_ping_100, p2p_over_ping_200, p2p_over_ping_300, p2p_over_ping_400, relay_user, relay_min_ping, relay_max_ping, relay_avg_ping, relay_over_ping_100, relay_over_ping_200, relay_over_ping_300, relay_over_ping_400 FROM p2p_statistics");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(P2PStatistics));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].server_group = row[1] ? atoi(row[1]) : 0;
        records[count].p2p_user = row[2] ? atoi(row[2]) : 0;
        records[count].p2p_min_ping = row[3] ? atoi(row[3]) : 0;
        records[count].p2p_max_ping = row[4] ? atoi(row[4]) : 0;
        records[count].p2p_avg_ping = row[5] ? atoi(row[5]) : 0;
        records[count].p2p_over_ping_100 = row[6] ? atoi(row[6]) : 0;
        records[count].p2p_over_ping_200 = row[7] ? atoi(row[7]) : 0;
        records[count].p2p_over_ping_300 = row[8] ? atoi(row[8]) : 0;
        records[count].p2p_over_ping_400 = row[9] ? atoi(row[9]) : 0;
        records[count].relay_user = row[10] ? atoi(row[10]) : 0;
        records[count].relay_min_ping = row[11] ? atoi(row[11]) : 0;
        records[count].relay_max_ping = row[12] ? atoi(row[12]) : 0;
        records[count].relay_avg_ping = row[13] ? atoi(row[13]) : 0;
        records[count].relay_over_ping_100 = row[14] ? atoi(row[14]) : 0;
        records[count].relay_over_ping_200 = row[15] ? atoi(row[15]) : 0;
        records[count].relay_over_ping_300 = row[16] ? atoi(row[16]) : 0;
        records[count].relay_over_ping_400 = row[17] ? atoi(row[17]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
