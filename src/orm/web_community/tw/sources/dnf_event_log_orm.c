#include "dnf_event_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfEventLog_Add(DBConnectionManager* manager, const DnfEventLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_event_log (occ_time, event_type, parameter1, parameter2, server_id, event_flag, start_time, end_time, m_id, expl, etc) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', '%s')",
        record->occ_time, record->event_type, record->parameter1, record->parameter2, record->server_id, record->event_flag, record->start_time, record->end_time, record->m_id, record->expl, record->etc);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventLog_Get(DBConnectionManager* manager, int log_id, DnfEventLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT log_id, occ_time, event_type, parameter1, parameter2, server_id, event_flag, start_time, end_time, m_id, expl, etc FROM dnf_event_log WHERE log_id = %d",
        log_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfEventLog));
    record->log_id = row[0] ? atoi(row[0]) : 0;
    record->occ_time = row[1] ? atoi(row[1]) : 0;
    record->event_type = row[2] ? atoi(row[2]) : 0;
    record->parameter1 = row[3] ? atoi(row[3]) : 0;
    record->parameter2 = row[4] ? atoi(row[4]) : 0;
    record->server_id = row[5] ? atoi(row[5]) : 0;
    record->event_flag = row[6] ? atoi(row[6]) : 0;
    record->start_time = row[7] ? atoi(row[7]) : 0;
    record->end_time = row[8] ? atoi(row[8]) : 0;
    record->m_id = row[9] ? atoi(row[9]) : 0;
    if (row[10]) strncpy(record->expl, row[10], sizeof(record->expl) - 1);
    if (row[11]) strncpy(record->etc, row[11], sizeof(record->etc) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventLog_GetAll(DBConnectionManager* manager, DnfEventLog* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT log_id, occ_time, event_type, parameter1, parameter2, server_id, event_flag, start_time, end_time, m_id, expl, etc FROM dnf_event_log");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfEventLog));
        records[count].log_id = row[0] ? atoi(row[0]) : 0;
        records[count].occ_time = row[1] ? atoi(row[1]) : 0;
        records[count].event_type = row[2] ? atoi(row[2]) : 0;
        records[count].parameter1 = row[3] ? atoi(row[3]) : 0;
        records[count].parameter2 = row[4] ? atoi(row[4]) : 0;
        records[count].server_id = row[5] ? atoi(row[5]) : 0;
        records[count].event_flag = row[6] ? atoi(row[6]) : 0;
        records[count].start_time = row[7] ? atoi(row[7]) : 0;
        records[count].end_time = row[8] ? atoi(row[8]) : 0;
        records[count].m_id = row[9] ? atoi(row[9]) : 0;
        if (row[10]) strncpy(records[count].expl, row[10], sizeof(records[count].expl) - 1);
        if (row[11]) strncpy(records[count].etc, row[11], sizeof(records[count].etc) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
