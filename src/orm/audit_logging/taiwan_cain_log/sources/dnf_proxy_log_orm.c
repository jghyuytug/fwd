#include "dnf_proxy_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfProxyLog_Add(DBConnectionManager* manager, const DnfProxyLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_proxy_log (occ_time, m_id, nat_ip, peer_ip, nat_tye) "
        "VALUES ('%s', %d, '%s', '%s', %d)",
        record->occ_time, record->m_id, record->nat_ip, record->peer_ip, record->nat_tye);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfProxyLog_Get(DBConnectionManager* manager, const char* occ_time, int m_id, DnfProxyLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, m_id, nat_ip, peer_ip, nat_tye FROM dnf_proxy_log WHERE occ_time = '%s' AND m_id = %d",
        occ_time, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfProxyLog));
    if (row[0]) strncpy(record->occ_time, row[0], sizeof(record->occ_time) - 1);
    record->m_id = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->nat_ip, row[2], sizeof(record->nat_ip) - 1);
    if (row[3]) strncpy(record->peer_ip, row[3], sizeof(record->peer_ip) - 1);
    record->nat_tye = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int DnfProxyLog_GetAll(DBConnectionManager* manager, DnfProxyLog* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, m_id, nat_ip, peer_ip, nat_tye FROM dnf_proxy_log");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfProxyLog));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].nat_ip, row[2], sizeof(records[count].nat_ip) - 1);
        if (row[3]) strncpy(records[count].peer_ip, row[3], sizeof(records[count].peer_ip) - 1);
        records[count].nat_tye = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
