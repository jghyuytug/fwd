#include "crash_down_by_playstatus_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CrashDownByPlaystatus_Add(DBConnectionManager* manager, const CrashDownByPlaystatus* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO crash_down_by_playstatus (occ_date, play_status, down_count) "
        "VALUES ('%s', %d, %d)",
        record->occ_date, record->play_status, record->down_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CrashDownByPlaystatus_Get(DBConnectionManager* manager, const char* occ_date, signed char play_status, CrashDownByPlaystatus* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, play_status, down_count FROM crash_down_by_playstatus WHERE occ_date = '%s' AND play_status = %d",
        occ_date, play_status);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CrashDownByPlaystatus));
    if (row[0]) strncpy(record->occ_date, row[0], sizeof(record->occ_date) - 1);
    record->play_status = row[1] ? atoi(row[1]) : 0;
    record->down_count = row[2] ? atoi(row[2]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int CrashDownByPlaystatus_GetAll(DBConnectionManager* manager, CrashDownByPlaystatus* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, play_status, down_count FROM crash_down_by_playstatus");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CrashDownByPlaystatus));
        if (row[0]) strncpy(records[count].occ_date, row[0], sizeof(records[count].occ_date) - 1);
        records[count].play_status = row[1] ? atoi(row[1]) : 0;
        records[count].down_count = row[2] ? atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
