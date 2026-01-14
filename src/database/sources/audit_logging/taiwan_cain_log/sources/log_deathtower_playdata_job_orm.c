#include "log_deathtower_playdata_job_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogDeathtowerPlaydataJob_Add(DBConnectionManager* manager, const LogDeathtowerPlaydataJob* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_deathtower_playdata_job (occ_time, type, charac_job, charac_grow, level, playcount, avg_clear_count) "
        "VALUES ('%s', %d, %d, %d, %d, %d, %d)",
        record->occ_time, record->type, record->charac_job, record->charac_grow, record->level, record->playcount, record->avg_clear_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogDeathtowerPlaydataJob_Get(DBConnectionManager* manager, const char* occ_time, signed char type, signed char charac_job, signed char charac_grow, signed char level, LogDeathtowerPlaydataJob* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, type, charac_job, charac_grow, level, playcount, avg_clear_count FROM log_deathtower_playdata_job WHERE occ_time = '%s' AND type = %d AND charac_job = %d AND charac_grow = %d AND level = %d",
        occ_time, type, charac_job, charac_grow, level);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogDeathtowerPlaydataJob));
    if (row[0]) strncpy(record->occ_time, row[0], sizeof(record->occ_time) - 1);
    record->type = row[1] ? atoi(row[1]) : 0;
    record->charac_job = row[2] ? atoi(row[2]) : 0;
    record->charac_grow = row[3] ? atoi(row[3]) : 0;
    record->level = row[4] ? atoi(row[4]) : 0;
    record->playcount = row[5] ? atoi(row[5]) : 0;
    record->avg_clear_count = row[6] ? atoi(row[6]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogDeathtowerPlaydataJob_GetAll(DBConnectionManager* manager, LogDeathtowerPlaydataJob* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, type, charac_job, charac_grow, level, playcount, avg_clear_count FROM log_deathtower_playdata_job");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogDeathtowerPlaydataJob));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].type = row[1] ? atoi(row[1]) : 0;
        records[count].charac_job = row[2] ? atoi(row[2]) : 0;
        records[count].charac_grow = row[3] ? atoi(row[3]) : 0;
        records[count].level = row[4] ? atoi(row[4]) : 0;
        records[count].playcount = row[5] ? atoi(row[5]) : 0;
        records[count].avg_clear_count = row[6] ? atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
