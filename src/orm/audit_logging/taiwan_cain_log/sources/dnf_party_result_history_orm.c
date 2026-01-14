#include "dnf_party_result_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfPartyResultHistory_Add(DBConnectionManager* manager, const DnfPartyResultHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_party_result_history (str_ip, nat_type, m_id, party_result, occ_time, map_id, dungeon_id, peer_ip, peer_nat_type, established_time, IsConnectedToRelaySrv, ConnectedTimeToRelaySrv, relay_ip, relay_udp_port, relay_tcp_port) "
        "VALUES ('%s', %d, %d, %d, '%s', %d, %d, '%s', %d, %d, '%s', %d, '%s', %d, %d)",
        record->str_ip, record->nat_type, record->m_id, record->party_result, record->occ_time, record->map_id, record->dungeon_id, record->peer_ip, record->peer_nat_type, record->established_time, record->IsConnectedToRelaySrv, record->ConnectedTimeToRelaySrv, record->relay_ip, record->relay_udp_port, record->relay_tcp_port);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfPartyResultHistory_Get(DBConnectionManager* manager, int id, DnfPartyResultHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[16];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, str_ip, nat_type, m_id, party_result, occ_time, map_id, dungeon_id, peer_ip, peer_nat_type, established_time, IsConnectedToRelaySrv, ConnectedTimeToRelaySrv, relay_ip, relay_udp_port, relay_tcp_port FROM dnf_party_result_history WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfPartyResultHistory));
    record->id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->str_ip, row[1], sizeof(record->str_ip) - 1);
    record->nat_type = row[2] ? atoi(row[2]) : 0;
    record->m_id = row[3] ? atoi(row[3]) : 0;
    record->party_result = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(record->occ_time, row[5], sizeof(record->occ_time) - 1);
    record->map_id = row[6] ? atoi(row[6]) : 0;
    record->dungeon_id = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->peer_ip, row[8], sizeof(record->peer_ip) - 1);
    record->peer_nat_type = row[9] ? atoi(row[9]) : 0;
    record->established_time = row[10] ? atoi(row[10]) : 0;
    if (row[11]) strncpy(record->IsConnectedToRelaySrv, row[11], sizeof(record->IsConnectedToRelaySrv) - 1);
    record->ConnectedTimeToRelaySrv = row[12] ? atoi(row[12]) : 0;
    if (row[13]) strncpy(record->relay_ip, row[13], sizeof(record->relay_ip) - 1);
    record->relay_udp_port = row[14] ? atoi(row[14]) : 0;
    record->relay_tcp_port = row[15] ? atoi(row[15]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int DnfPartyResultHistory_GetAll(DBConnectionManager* manager, DnfPartyResultHistory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[16];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, str_ip, nat_type, m_id, party_result, occ_time, map_id, dungeon_id, peer_ip, peer_nat_type, established_time, IsConnectedToRelaySrv, ConnectedTimeToRelaySrv, relay_ip, relay_udp_port, relay_tcp_port FROM dnf_party_result_history");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfPartyResultHistory));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].str_ip, row[1], sizeof(records[count].str_ip) - 1);
        records[count].nat_type = row[2] ? atoi(row[2]) : 0;
        records[count].m_id = row[3] ? atoi(row[3]) : 0;
        records[count].party_result = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(records[count].occ_time, row[5], sizeof(records[count].occ_time) - 1);
        records[count].map_id = row[6] ? atoi(row[6]) : 0;
        records[count].dungeon_id = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].peer_ip, row[8], sizeof(records[count].peer_ip) - 1);
        records[count].peer_nat_type = row[9] ? atoi(row[9]) : 0;
        records[count].established_time = row[10] ? atoi(row[10]) : 0;
        if (row[11]) strncpy(records[count].IsConnectedToRelaySrv, row[11], sizeof(records[count].IsConnectedToRelaySrv) - 1);
        records[count].ConnectedTimeToRelaySrv = row[12] ? atoi(row[12]) : 0;
        if (row[13]) strncpy(records[count].relay_ip, row[13], sizeof(records[count].relay_ip) - 1);
        records[count].relay_udp_port = row[14] ? atoi(row[14]) : 0;
        records[count].relay_tcp_port = row[15] ? atoi(row[15]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
