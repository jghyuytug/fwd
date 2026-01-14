#include "dnf_zeromoney_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfZeromoneyLog_Add(DBConnectionManager* manager, const DnfZeromoneyLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_zeromoney_log (to_m_id, to_nat_ip, to_peer_ip, to_nat_type, to_mac_addr, from_m_id, from_nat_ip, from_peer_ip, from_nat_type, from_mac_addr, occ_time, injury_money) "
        "VALUES (%d, '%s', '%s', %d, '%s', %d, '%s', '%s', %d, '%s', '%s', %d)",
        record->to_m_id, record->to_nat_ip, record->to_peer_ip, record->to_nat_type, record->to_mac_addr, record->from_m_id, record->from_nat_ip, record->from_peer_ip, record->from_nat_type, record->from_mac_addr, record->occ_time, record->injury_money);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfZeromoneyLog_GetAll(DBConnectionManager* manager, DnfZeromoneyLog* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT to_m_id, to_nat_ip, to_peer_ip, to_nat_type, to_mac_addr, from_m_id, from_nat_ip, from_peer_ip, from_nat_type, from_mac_addr, occ_time, injury_money FROM dnf_zeromoney_log");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfZeromoneyLog));
        records[count].to_m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].to_nat_ip, row[1], sizeof(records[count].to_nat_ip) - 1);
        if (row[2]) strncpy(records[count].to_peer_ip, row[2], sizeof(records[count].to_peer_ip) - 1);
        records[count].to_nat_type = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].to_mac_addr, row[4], sizeof(records[count].to_mac_addr) - 1);
        records[count].from_m_id = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].from_nat_ip, row[6], sizeof(records[count].from_nat_ip) - 1);
        if (row[7]) strncpy(records[count].from_peer_ip, row[7], sizeof(records[count].from_peer_ip) - 1);
        records[count].from_nat_type = row[8] ? atoi(row[8]) : 0;
        if (row[9]) strncpy(records[count].from_mac_addr, row[9], sizeof(records[count].from_mac_addr) - 1);
        if (row[10]) strncpy(records[count].occ_time, row[10], sizeof(records[count].occ_time) - 1);
        records[count].injury_money = row[11] ? atoi(row[11]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
