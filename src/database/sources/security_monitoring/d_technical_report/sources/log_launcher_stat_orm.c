#include "log_launcher_stat_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogLauncherStat_Add(DBConnectionManager* manager, const LogLauncherStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_launcher_stat (occ_date, execute, cancel, success, first_success, p2p, all_time, p2p_count, all_time_count) "
        "VALUES ('%s', %d, %d, %d, %d, %f, %d, %d, %d)",
        record->occ_date, record->execute, record->cancel, record->success, record->first_success, record->p2p, record->all_time, record->p2p_count, record->all_time_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogLauncherStat_Get(DBConnectionManager* manager, const char* occ_date, LogLauncherStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, execute, cancel, success, first_success, p2p, all_time, p2p_count, all_time_count FROM log_launcher_stat WHERE occ_date = '%s'",
        occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogLauncherStat));
    if (row[0]) strncpy(record->occ_date, row[0], sizeof(record->occ_date) - 1);
    record->execute = row[1] ? atoi(row[1]) : 0;
    record->cancel = row[2] ? atoi(row[2]) : 0;
    record->success = row[3] ? atoi(row[3]) : 0;
    record->first_success = row[4] ? atoi(row[4]) : 0;
    record->p2p = row[5] ? atof(row[5]) : 0;
    record->all_time = row[6] ? atoll(row[6]) : 0;
    record->p2p_count = row[7] ? atoi(row[7]) : 0;
    record->all_time_count = row[8] ? atoi(row[8]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogLauncherStat_GetAll(DBConnectionManager* manager, LogLauncherStat* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, execute, cancel, success, first_success, p2p, all_time, p2p_count, all_time_count FROM log_launcher_stat");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogLauncherStat));
        if (row[0]) strncpy(records[count].occ_date, row[0], sizeof(records[count].occ_date) - 1);
        records[count].execute = row[1] ? atoi(row[1]) : 0;
        records[count].cancel = row[2] ? atoi(row[2]) : 0;
        records[count].success = row[3] ? atoi(row[3]) : 0;
        records[count].first_success = row[4] ? atoi(row[4]) : 0;
        records[count].p2p = row[5] ? atof(row[5]) : 0;
        records[count].all_time = row[6] ? atoll(row[6]) : 0;
        records[count].p2p_count = row[7] ? atoi(row[7]) : 0;
        records[count].all_time_count = row[8] ? atoi(row[8]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
