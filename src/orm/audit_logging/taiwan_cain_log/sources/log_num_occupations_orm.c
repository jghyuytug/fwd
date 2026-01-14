#include "log_num_occupations_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogNumOccupations_Add(DBConnectionManager* manager, const LogNumOccupations* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_num_occupations (occ_time, num_occupations_charscreen, num_occupations_seriaroom, num_login_per_min, num_logout_per_min) "
        "VALUES ('%s', %d, %d, %d, %d)",
        record->occ_time, record->num_occupations_charscreen, record->num_occupations_seriaroom, record->num_login_per_min, record->num_logout_per_min);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogNumOccupations_Get(DBConnectionManager* manager, const char* occ_time, LogNumOccupations* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, num_occupations_charscreen, num_occupations_seriaroom, num_login_per_min, num_logout_per_min FROM log_num_occupations WHERE occ_time = '%s'",
        occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogNumOccupations));
    if (row[0]) strncpy(record->occ_time, row[0], sizeof(record->occ_time) - 1);
    record->num_occupations_charscreen = row[1] ? atoi(row[1]) : 0;
    record->num_occupations_seriaroom = row[2] ? atoi(row[2]) : 0;
    record->num_login_per_min = row[3] ? atoi(row[3]) : 0;
    record->num_logout_per_min = row[4] ? atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogNumOccupations_GetAll(DBConnectionManager* manager, LogNumOccupations* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, num_occupations_charscreen, num_occupations_seriaroom, num_login_per_min, num_logout_per_min FROM log_num_occupations");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogNumOccupations));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].num_occupations_charscreen = row[1] ? atoi(row[1]) : 0;
        records[count].num_occupations_seriaroom = row[2] ? atoi(row[2]) : 0;
        records[count].num_login_per_min = row[3] ? atoi(row[3]) : 0;
        records[count].num_logout_per_min = row[4] ? atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
