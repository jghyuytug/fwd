#include "dnf_hack_log_del_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfHackLogDel_Add(DBConnectionManager* manager, const DnfHackLogDel* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_hack_log_del (sdate, m_id, charac_no, hack_type, last_time, etc, cnt, occ_date) "
        "VALUES ('%s', %d, %d, %d, '%s', %d, %d, '%s')",
        record->sdate, record->m_id, record->charac_no, record->hack_type, record->last_time, record->etc, record->cnt, record->occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfHackLogDel_Get(DBConnectionManager* manager, const char* sdate, const char* occ_date, int m_id, short hack_type, DnfHackLogDel* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT sdate, m_id, charac_no, hack_type, last_time, etc, cnt, occ_date FROM dnf_hack_log_del WHERE sdate = '%s' AND occ_date = '%s' AND m_id = %d AND hack_type = %d",
        sdate, occ_date, m_id, hack_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfHackLogDel));
    if (row[0]) strncpy(record->sdate, row[0], sizeof(record->sdate) - 1);
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->charac_no = row[2] ? atoi(row[2]) : 0;
    record->hack_type = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->last_time, row[4], sizeof(record->last_time) - 1);
    record->etc = row[5] ? atoi(row[5]) : 0;
    record->cnt = row[6] ? atoi(row[6]) : 0;
    if (row[7]) strncpy(record->occ_date, row[7], sizeof(record->occ_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfHackLogDel_GetAll(DBConnectionManager* manager, DnfHackLogDel* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT sdate, m_id, charac_no, hack_type, last_time, etc, cnt, occ_date FROM dnf_hack_log_del");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfHackLogDel));
        if (row[0]) strncpy(records[count].sdate, row[0], sizeof(records[count].sdate) - 1);
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].charac_no = row[2] ? atoi(row[2]) : 0;
        records[count].hack_type = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].last_time, row[4], sizeof(records[count].last_time) - 1);
        records[count].etc = row[5] ? atoi(row[5]) : 0;
        records[count].cnt = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].occ_date, row[7], sizeof(records[count].occ_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
