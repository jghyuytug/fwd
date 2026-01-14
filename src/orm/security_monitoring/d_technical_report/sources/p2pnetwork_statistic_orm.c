#include "p2pnetwork_statistic_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int P2PnetworkStatistic_Add(DBConnectionManager* manager, const P2PnetworkStatistic* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO p2pnetwork_statistic (occ_time, server_group, success_party_try, total_party_try, dungeon_bad_ping, dungeon_total, pvp_bad_ping, pvp_total, success_dungeon_clear, total_dungeon_clear, fair_pvp_total, fair_pvp_bad_ping) "
        "VALUES ('%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        record->occ_time, record->server_group, record->success_party_try, record->total_party_try, record->dungeon_bad_ping, record->dungeon_total, record->pvp_bad_ping, record->pvp_total, record->success_dungeon_clear, record->total_dungeon_clear, record->fair_pvp_total, record->fair_pvp_bad_ping);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int P2PnetworkStatistic_Get(DBConnectionManager* manager, int id, P2PnetworkStatistic* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, occ_time, server_group, success_party_try, total_party_try, dungeon_bad_ping, dungeon_total, pvp_bad_ping, pvp_total, success_dungeon_clear, total_dungeon_clear, fair_pvp_total, fair_pvp_bad_ping FROM p2pnetwork_statistic WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(P2PnetworkStatistic));
    record->id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->occ_time, row[1], sizeof(record->occ_time) - 1);
    record->server_group = row[2] ? atoi(row[2]) : 0;
    record->success_party_try = row[3] ? atoi(row[3]) : 0;
    record->total_party_try = row[4] ? atoi(row[4]) : 0;
    record->dungeon_bad_ping = row[5] ? atoi(row[5]) : 0;
    record->dungeon_total = row[6] ? atoi(row[6]) : 0;
    record->pvp_bad_ping = row[7] ? atoi(row[7]) : 0;
    record->pvp_total = row[8] ? atoi(row[8]) : 0;
    record->success_dungeon_clear = row[9] ? atoi(row[9]) : 0;
    record->total_dungeon_clear = row[10] ? atoi(row[10]) : 0;
    record->fair_pvp_total = row[11] ? atoi(row[11]) : 0;
    record->fair_pvp_bad_ping = row[12] ? atoi(row[12]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int P2PnetworkStatistic_GetAll(DBConnectionManager* manager, P2PnetworkStatistic* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, occ_time, server_group, success_party_try, total_party_try, dungeon_bad_ping, dungeon_total, pvp_bad_ping, pvp_total, success_dungeon_clear, total_dungeon_clear, fair_pvp_total, fair_pvp_bad_ping FROM p2pnetwork_statistic");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(P2PnetworkStatistic));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].occ_time, row[1], sizeof(records[count].occ_time) - 1);
        records[count].server_group = row[2] ? atoi(row[2]) : 0;
        records[count].success_party_try = row[3] ? atoi(row[3]) : 0;
        records[count].total_party_try = row[4] ? atoi(row[4]) : 0;
        records[count].dungeon_bad_ping = row[5] ? atoi(row[5]) : 0;
        records[count].dungeon_total = row[6] ? atoi(row[6]) : 0;
        records[count].pvp_bad_ping = row[7] ? atoi(row[7]) : 0;
        records[count].pvp_total = row[8] ? atoi(row[8]) : 0;
        records[count].success_dungeon_clear = row[9] ? atoi(row[9]) : 0;
        records[count].total_dungeon_clear = row[10] ? atoi(row[10]) : 0;
        records[count].fair_pvp_total = row[11] ? atoi(row[11]) : 0;
        records[count].fair_pvp_bad_ping = row[12] ? atoi(row[12]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
