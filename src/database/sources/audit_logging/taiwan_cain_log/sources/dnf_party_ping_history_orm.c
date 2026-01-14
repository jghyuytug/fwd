#include "dnf_party_ping_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfPartyPingHistory_Add(DBConnectionManager* manager, const DnfPartyPingHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_party_ping_history (str_ip, m_id, ip_1, nat_type_1, ping1, mtu_1, ip_2, nat_type_2, ping2, mtu_2, ip_3, nat_type_3, ping3, mtu_3, ip_4, nat_type_4, ping4, mtu_4, occ_time, map_id, dungeon_id, end_reason) "
        "VALUES ('%s', %d, '%s', %d, %d, %d, '%s', %d, %d, %d, '%s', %d, %d, %d, '%s', %d, %d, %d, '%s', %d, %d, '%s')",
        record->str_ip, record->m_id, record->ip_1, record->nat_type_1, record->ping1, record->mtu_1, record->ip_2, record->nat_type_2, record->ping2, record->mtu_2, record->ip_3, record->nat_type_3, record->ping3, record->mtu_3, record->ip_4, record->nat_type_4, record->ping4, record->mtu_4, record->occ_time, record->map_id, record->dungeon_id, record->end_reason);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfPartyPingHistory_Get(DBConnectionManager* manager, int id, DnfPartyPingHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[23];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, str_ip, m_id, ip_1, nat_type_1, ping1, mtu_1, ip_2, nat_type_2, ping2, mtu_2, ip_3, nat_type_3, ping3, mtu_3, ip_4, nat_type_4, ping4, mtu_4, occ_time, map_id, dungeon_id, end_reason FROM dnf_party_ping_history WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfPartyPingHistory));
    record->id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->str_ip, row[1], sizeof(record->str_ip) - 1);
    record->m_id = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->ip_1, row[3], sizeof(record->ip_1) - 1);
    record->nat_type_1 = row[4] ? atoi(row[4]) : 0;
    record->ping1 = row[5] ? atoi(row[5]) : 0;
    record->mtu_1 = row[6] ? atoi(row[6]) : 0;
    if (row[7]) strncpy(record->ip_2, row[7], sizeof(record->ip_2) - 1);
    record->nat_type_2 = row[8] ? atoi(row[8]) : 0;
    record->ping2 = row[9] ? atoi(row[9]) : 0;
    record->mtu_2 = row[10] ? atoi(row[10]) : 0;
    if (row[11]) strncpy(record->ip_3, row[11], sizeof(record->ip_3) - 1);
    record->nat_type_3 = row[12] ? atoi(row[12]) : 0;
    record->ping3 = row[13] ? atoi(row[13]) : 0;
    record->mtu_3 = row[14] ? atoi(row[14]) : 0;
    if (row[15]) strncpy(record->ip_4, row[15], sizeof(record->ip_4) - 1);
    record->nat_type_4 = row[16] ? atoi(row[16]) : 0;
    record->ping4 = row[17] ? atoi(row[17]) : 0;
    record->mtu_4 = row[18] ? atoi(row[18]) : 0;
    if (row[19]) strncpy(record->occ_time, row[19], sizeof(record->occ_time) - 1);
    record->map_id = row[20] ? atoi(row[20]) : 0;
    record->dungeon_id = row[21] ? atoi(row[21]) : 0;
    if (row[22]) strncpy(record->end_reason, row[22], sizeof(record->end_reason) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfPartyPingHistory_GetAll(DBConnectionManager* manager, DnfPartyPingHistory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[23];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, str_ip, m_id, ip_1, nat_type_1, ping1, mtu_1, ip_2, nat_type_2, ping2, mtu_2, ip_3, nat_type_3, ping3, mtu_3, ip_4, nat_type_4, ping4, mtu_4, occ_time, map_id, dungeon_id, end_reason FROM dnf_party_ping_history");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfPartyPingHistory));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].str_ip, row[1], sizeof(records[count].str_ip) - 1);
        records[count].m_id = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].ip_1, row[3], sizeof(records[count].ip_1) - 1);
        records[count].nat_type_1 = row[4] ? atoi(row[4]) : 0;
        records[count].ping1 = row[5] ? atoi(row[5]) : 0;
        records[count].mtu_1 = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].ip_2, row[7], sizeof(records[count].ip_2) - 1);
        records[count].nat_type_2 = row[8] ? atoi(row[8]) : 0;
        records[count].ping2 = row[9] ? atoi(row[9]) : 0;
        records[count].mtu_2 = row[10] ? atoi(row[10]) : 0;
        if (row[11]) strncpy(records[count].ip_3, row[11], sizeof(records[count].ip_3) - 1);
        records[count].nat_type_3 = row[12] ? atoi(row[12]) : 0;
        records[count].ping3 = row[13] ? atoi(row[13]) : 0;
        records[count].mtu_3 = row[14] ? atoi(row[14]) : 0;
        if (row[15]) strncpy(records[count].ip_4, row[15], sizeof(records[count].ip_4) - 1);
        records[count].nat_type_4 = row[16] ? atoi(row[16]) : 0;
        records[count].ping4 = row[17] ? atoi(row[17]) : 0;
        records[count].mtu_4 = row[18] ? atoi(row[18]) : 0;
        if (row[19]) strncpy(records[count].occ_time, row[19], sizeof(records[count].occ_time) - 1);
        records[count].map_id = row[20] ? atoi(row[20]) : 0;
        records[count].dungeon_id = row[21] ? atoi(row[21]) : 0;
        if (row[22]) strncpy(records[count].end_reason, row[22], sizeof(records[count].end_reason) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
