#include "log_3month_stop_web_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int Log3MonthStopWeb_Add(DBConnectionManager* manager, const Log3MonthStopWeb* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_3month_stop_web (occ_time, m_id, is_complete, is_confirm, server_no) "
        "VALUES ('%s', %d, %d, %d, %d)",
        record->occ_time, record->m_id, record->is_complete, record->is_confirm, record->server_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int Log3MonthStopWeb_Get(DBConnectionManager* manager, const char* occ_time, int m_id, Log3MonthStopWeb* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, m_id, is_complete, is_confirm, server_no FROM log_3month_stop_web WHERE occ_time = '%s' AND m_id = %d",
        occ_time, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(Log3MonthStopWeb));
    if (row[0]) strncpy(record->occ_time, row[0], sizeof(record->occ_time) - 1);
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->is_complete = row[2] ? atoi(row[2]) : 0;
    record->is_confirm = row[3] ? atoi(row[3]) : 0;
    record->server_no = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int Log3MonthStopWeb_GetAll(DBConnectionManager* manager, Log3MonthStopWeb* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, m_id, is_complete, is_confirm, server_no FROM log_3month_stop_web");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(Log3MonthStopWeb));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].is_complete = row[2] ? atoi(row[2]) : 0;
        records[count].is_confirm = row[3] ? atoi(row[3]) : 0;
        records[count].server_no = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
