#include "monitoring_logout_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MonitoringLogout_Add(DBConnectionManager* manager, const MonitoringLogout* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO monitoring_logout (m_id, logout_time, logout_ip, otp_del_type) "
        "VALUES (%d, %d, %d, %d)",
        record->m_id, record->logout_time, record->logout_ip, record->otp_del_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_SECU, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MonitoringLogout_Get(DBConnectionManager* manager, int no, MonitoringLogout* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, logout_time, logout_ip, otp_del_type FROM monitoring_logout WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_SECU, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MonitoringLogout));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->logout_time = row[2] ? atoi(row[2]) : 0;
    record->logout_ip = row[3] ? atoi(row[3]) : 0;
    record->otp_del_type = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int MonitoringLogout_GetAll(DBConnectionManager* manager, MonitoringLogout* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, logout_time, logout_ip, otp_del_type FROM monitoring_logout");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_SECU, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MonitoringLogout));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].logout_time = row[2] ? atoi(row[2]) : 0;
        records[count].logout_ip = row[3] ? atoi(row[3]) : 0;
        records[count].otp_del_type = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
