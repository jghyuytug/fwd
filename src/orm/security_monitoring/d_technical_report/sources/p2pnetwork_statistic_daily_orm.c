#include "p2pnetwork_statistic_daily_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int P2PnetworkStatisticDaily_Add(DBConnectionManager* manager, const P2PnetworkStatisticDaily* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO p2pnetwork_statistic_daily (cur_date, success_party, dungeon_bad, pvp_bad, success_dungeon_clear, fair_pvp_bad) "
        "VALUES ('%s', %f, %f, %f, %f, %f)",
        record->cur_date, record->success_party, record->dungeon_bad, record->pvp_bad, record->success_dungeon_clear, record->fair_pvp_bad);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int P2PnetworkStatisticDaily_Get(DBConnectionManager* manager, const char* cur_date, P2PnetworkStatisticDaily* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT cur_date, success_party, dungeon_bad, pvp_bad, success_dungeon_clear, fair_pvp_bad FROM p2pnetwork_statistic_daily WHERE cur_date = '%s'",
        cur_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(P2PnetworkStatisticDaily));
    if (row[0]) strncpy(record->cur_date, row[0], sizeof(record->cur_date) - 1);
    record->success_party = row[1] ? atof(row[1]) : 0;
    record->dungeon_bad = row[2] ? atof(row[2]) : 0;
    record->pvp_bad = row[3] ? atof(row[3]) : 0;
    record->success_dungeon_clear = row[4] ? atof(row[4]) : 0;
    record->fair_pvp_bad = row[5] ? atof(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int P2PnetworkStatisticDaily_GetAll(DBConnectionManager* manager, P2PnetworkStatisticDaily* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT cur_date, success_party, dungeon_bad, pvp_bad, success_dungeon_clear, fair_pvp_bad FROM p2pnetwork_statistic_daily");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(P2PnetworkStatisticDaily));
        if (row[0]) strncpy(records[count].cur_date, row[0], sizeof(records[count].cur_date) - 1);
        records[count].success_party = row[1] ? atof(row[1]) : 0;
        records[count].dungeon_bad = row[2] ? atof(row[2]) : 0;
        records[count].pvp_bad = row[3] ? atof(row[3]) : 0;
        records[count].success_dungeon_clear = row[4] ? atof(row[4]) : 0;
        records[count].fair_pvp_bad = row[5] ? atof(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
