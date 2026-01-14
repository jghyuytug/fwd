#include "dnf_hack_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfHackLog_Add(DBConnectionManager* manager, const DnfHackLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_hack_log (m_id, charac_no, hack_type, last_time, etc, cnt, occ_date) "
        "VALUES (%d, %d, %d, '%s', %d, %d, '%s')",
        record->m_id, record->charac_no, record->hack_type, record->last_time, record->etc, record->cnt, record->occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfHackLog_Get(DBConnectionManager* manager, int m_id, short hack_type, const char* occ_date, DnfHackLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_no, hack_type, last_time, etc, cnt, occ_date FROM dnf_hack_log WHERE m_id = %d AND hack_type = %d AND occ_date = '%s'",
        m_id, hack_type, occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfHackLog));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->charac_no = row[1] ? atoi(row[1]) : 0;
    record->hack_type = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->last_time, row[3], sizeof(record->last_time) - 1);
    record->etc = row[4] ? atoi(row[4]) : 0;
    record->cnt = row[5] ? atoi(row[5]) : 0;
    if (row[6]) strncpy(record->occ_date, row[6], sizeof(record->occ_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfHackLog_GetAll(DBConnectionManager* manager, DnfHackLog* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_no, hack_type, last_time, etc, cnt, occ_date FROM dnf_hack_log");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfHackLog));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].charac_no = row[1] ? atoi(row[1]) : 0;
        records[count].hack_type = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].last_time, row[3], sizeof(records[count].last_time) - 1);
        records[count].etc = row[4] ? atoi(row[4]) : 0;
        records[count].cnt = row[5] ? atoi(row[5]) : 0;
        if (row[6]) strncpy(records[count].occ_date, row[6], sizeof(records[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
