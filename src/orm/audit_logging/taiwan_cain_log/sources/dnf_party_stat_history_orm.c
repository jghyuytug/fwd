#include "dnf_party_stat_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfPartyStatHistory_Add(DBConnectionManager* manager, const DnfPartyStatHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_party_stat_history (str_ip, m_id, try_num, party_result, party_num, ping1, ping2, ping3, ping4, occ_time) "
        "VALUES ('%s', %d, %d, '%s', %d, %d, %d, %d, %d, '%s')",
        record->str_ip, record->m_id, record->try_num, record->party_result, record->party_num, record->ping1, record->ping2, record->ping3, record->ping4, record->occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfPartyStatHistory_Get(DBConnectionManager* manager, int id, DnfPartyStatHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, str_ip, m_id, try_num, party_result, party_num, ping1, ping2, ping3, ping4, occ_time FROM dnf_party_stat_history WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfPartyStatHistory));
    record->id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->str_ip, row[1], sizeof(record->str_ip) - 1);
    record->m_id = row[2] ? atoi(row[2]) : 0;
    record->try_num = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->party_result, row[4], sizeof(record->party_result) - 1);
    record->party_num = row[5] ? atoi(row[5]) : 0;
    record->ping1 = row[6] ? atoi(row[6]) : 0;
    record->ping2 = row[7] ? atoi(row[7]) : 0;
    record->ping3 = row[8] ? atoi(row[8]) : 0;
    record->ping4 = row[9] ? atoi(row[9]) : 0;
    if (row[10]) strncpy(record->occ_time, row[10], sizeof(record->occ_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfPartyStatHistory_GetAll(DBConnectionManager* manager, DnfPartyStatHistory* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, str_ip, m_id, try_num, party_result, party_num, ping1, ping2, ping3, ping4, occ_time FROM dnf_party_stat_history");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfPartyStatHistory));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].str_ip, row[1], sizeof(records[count].str_ip) - 1);
        records[count].m_id = row[2] ? atoi(row[2]) : 0;
        records[count].try_num = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].party_result, row[4], sizeof(records[count].party_result) - 1);
        records[count].party_num = row[5] ? atoi(row[5]) : 0;
        records[count].ping1 = row[6] ? atoi(row[6]) : 0;
        records[count].ping2 = row[7] ? atoi(row[7]) : 0;
        records[count].ping3 = row[8] ? atoi(row[8]) : 0;
        records[count].ping4 = row[9] ? atoi(row[9]) : 0;
        if (row[10]) strncpy(records[count].occ_time, row[10], sizeof(records[count].occ_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
