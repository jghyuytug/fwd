#include "dnf_master_charac_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfMasterCharac_Add(DBConnectionManager* manager, const DnfMasterCharac* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_master_charac (m_id, global_type, server_id, charac_no, charac_name, job, lev) "
        "VALUES (%d, %d, %d, %d, '%s', %d, %d)",
        record->m_id, record->global_type, record->server_id, record->charac_no, record->charac_name, record->job, record->lev);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfMasterCharac_Get(DBConnectionManager* manager, int m_id, signed char global_type, signed char server_id, DnfMasterCharac* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, global_type, server_id, charac_no, charac_name, job, lev FROM dnf_master_charac WHERE m_id = %d AND global_type = %d AND server_id = %d",
        m_id, global_type, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfMasterCharac));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->global_type = row[1] ? atoi(row[1]) : 0;
    record->server_id = row[2] ? atoi(row[2]) : 0;
    record->charac_no = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->charac_name, row[4], sizeof(record->charac_name) - 1);
    record->job = row[5] ? atoi(row[5]) : 0;
    record->lev = row[6] ? atoi(row[6]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int DnfMasterCharac_GetAll(DBConnectionManager* manager, DnfMasterCharac* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, global_type, server_id, charac_no, charac_name, job, lev FROM dnf_master_charac");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfMasterCharac));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].global_type = row[1] ? atoi(row[1]) : 0;
        records[count].server_id = row[2] ? atoi(row[2]) : 0;
        records[count].charac_no = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].charac_name, row[4], sizeof(records[count].charac_name) - 1);
        records[count].job = row[5] ? atoi(row[5]) : 0;
        records[count].lev = row[6] ? atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
