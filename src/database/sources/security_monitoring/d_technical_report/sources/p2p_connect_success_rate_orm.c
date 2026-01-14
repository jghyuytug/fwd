#include "p2p_connect_success_rate_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int P2PConnectSuccessRate_Add(DBConnectionManager* manager, const P2PConnectSuccessRate* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO p2p_connect_success_rate (server_group, connected_type, required_time, check_time, nation_code, peer_address, occ_date) "
        "VALUES (%d, %d, %d, %d, '%s', '%s', '%s')",
        record->server_group, record->connected_type, record->required_time, record->check_time, record->nation_code, record->peer_address, record->occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int P2PConnectSuccessRate_Get(DBConnectionManager* manager, int no, P2PConnectSuccessRate* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, server_group, connected_type, required_time, check_time, nation_code, peer_address, occ_date FROM p2p_connect_success_rate WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(P2PConnectSuccessRate));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->server_group = row[1] ? atoi(row[1]) : 0;
    record->connected_type = row[2] ? atoi(row[2]) : 0;
    record->required_time = row[3] ? atoi(row[3]) : 0;
    record->check_time = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->nation_code, row[5], sizeof(record->nation_code) - 1);
    if (row[6]) strncpy(record->peer_address, row[6], sizeof(record->peer_address) - 1);
    if (row[7]) strncpy(record->occ_date, row[7], sizeof(record->occ_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int P2PConnectSuccessRate_GetAll(DBConnectionManager* manager, P2PConnectSuccessRate* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, server_group, connected_type, required_time, check_time, nation_code, peer_address, occ_date FROM p2p_connect_success_rate");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(P2PConnectSuccessRate));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].server_group = row[1] ? atoi(row[1]) : 0;
        records[count].connected_type = row[2] ? atoi(row[2]) : 0;
        records[count].required_time = row[3] ? atoi(row[3]) : 0;
        records[count].check_time = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].nation_code, row[5], sizeof(records[count].nation_code) - 1);
        if (row[6]) strncpy(records[count].peer_address, row[6], sizeof(records[count].peer_address) - 1);
        if (row[7]) strncpy(records[count].occ_date, row[7], sizeof(records[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
