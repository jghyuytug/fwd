#include "log_advance_alter_entrance_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogAdvanceAlterEntrance_Add(DBConnectionManager* manager, const LogAdvanceAlterEntrance* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_advance_alter_entrance (occ_date, stage_id, entrance_count, success_count, obtain_star) "
        "VALUES ('%s', %d, %d, %d, %d)",
        record->occ_date, record->stage_id, record->entrance_count, record->success_count, record->obtain_star);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogAdvanceAlterEntrance_Get(DBConnectionManager* manager, const char* occ_date, int stage_id, LogAdvanceAlterEntrance* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, stage_id, entrance_count, success_count, obtain_star FROM log_advance_alter_entrance WHERE occ_date = '%s' AND stage_id = %d",
        occ_date, stage_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogAdvanceAlterEntrance));
    if (row[0]) strncpy(record->occ_date, row[0], sizeof(record->occ_date) - 1);
    record->stage_id = row[1] ? atoi(row[1]) : 0;
    record->entrance_count = row[2] ? atoi(row[2]) : 0;
    record->success_count = row[3] ? atoi(row[3]) : 0;
    record->obtain_star = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogAdvanceAlterEntrance_GetAll(DBConnectionManager* manager, LogAdvanceAlterEntrance* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, stage_id, entrance_count, success_count, obtain_star FROM log_advance_alter_entrance");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogAdvanceAlterEntrance));
        if (row[0]) strncpy(records[count].occ_date, row[0], sizeof(records[count].occ_date) - 1);
        records[count].stage_id = row[1] ? atoi(row[1]) : 0;
        records[count].entrance_count = row[2] ? atoi(row[2]) : 0;
        records[count].success_count = row[3] ? atoi(row[3]) : 0;
        records[count].obtain_star = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
