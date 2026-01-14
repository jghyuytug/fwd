#include "dnf_school_charac_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfSchoolCharac_Add(DBConnectionManager* manager, const DnfSchoolCharac* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_school_charac (m_id, school_id, server_id, charac_no, charac_name, occ_time) "
        "VALUES (%d, %d, %d, %d, '%s', '%s')",
        record->m_id, record->school_id, record->server_id, record->charac_no, record->charac_name, record->occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfSchoolCharac_Get(DBConnectionManager* manager, int m_id, DnfSchoolCharac* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, school_id, server_id, charac_no, charac_name, occ_time FROM dnf_school_charac WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfSchoolCharac));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->school_id = row[1] ? atoi(row[1]) : 0;
    record->server_id = row[2] ? atoi(row[2]) : 0;
    record->charac_no = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->charac_name, row[4], sizeof(record->charac_name) - 1);
    if (row[5]) strncpy(record->occ_time, row[5], sizeof(record->occ_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfSchoolCharac_GetAll(DBConnectionManager* manager, DnfSchoolCharac* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, school_id, server_id, charac_no, charac_name, occ_time FROM dnf_school_charac");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfSchoolCharac));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].school_id = row[1] ? atoi(row[1]) : 0;
        records[count].server_id = row[2] ? atoi(row[2]) : 0;
        records[count].charac_no = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].charac_name, row[4], sizeof(records[count].charac_name) - 1);
        if (row[5]) strncpy(records[count].occ_time, row[5], sizeof(records[count].occ_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
