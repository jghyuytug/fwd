#include "log_hardware_ting_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogHardwareTing_Add(DBConnectionManager* manager, const LogHardwareTing* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_hardware_ting (occ_time, category1, category2, category3, total, ting) "
        "VALUES ('%s', %d, %d, %d, %d, %d)",
        record->occ_time, record->category1, record->category2, record->category3, record->total, record->ting);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogHardwareTing_Get(DBConnectionManager* manager, const char* occ_time, signed char category1, int category2, int category3, LogHardwareTing* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, category1, category2, category3, total, ting FROM log_hardware_ting WHERE occ_time = '%s' AND category1 = %d AND category2 = %d AND category3 = %d",
        occ_time, category1, category2, category3);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogHardwareTing));
    if (row[0]) strncpy(record->occ_time, row[0], sizeof(record->occ_time) - 1);
    record->category1 = row[1] ? atoi(row[1]) : 0;
    record->category2 = row[2] ? atoi(row[2]) : 0;
    record->category3 = row[3] ? atoi(row[3]) : 0;
    record->total = row[4] ? atoi(row[4]) : 0;
    record->ting = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogHardwareTing_GetAll(DBConnectionManager* manager, LogHardwareTing* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, category1, category2, category3, total, ting FROM log_hardware_ting");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogHardwareTing));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].category1 = row[1] ? atoi(row[1]) : 0;
        records[count].category2 = row[2] ? atoi(row[2]) : 0;
        records[count].category3 = row[3] ? atoi(row[3]) : 0;
        records[count].total = row[4] ? atoi(row[4]) : 0;
        records[count].ting = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
