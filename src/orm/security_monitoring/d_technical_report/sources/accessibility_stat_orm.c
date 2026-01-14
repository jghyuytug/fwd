#include "accessibility_stat_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int AccessibilityStat_Add(DBConnectionManager* manager, const AccessibilityStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO accessibility_stat (occ_time, main_type, sub_type, val) "
        "VALUES ('%s', %d, %d, %d)",
        record->occ_time, record->main_type, record->sub_type, record->val);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AccessibilityStat_Get(DBConnectionManager* manager, const char* occ_time, signed char main_type, short sub_type, AccessibilityStat* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, main_type, sub_type, val FROM accessibility_stat WHERE occ_time = '%s' AND main_type = %d AND sub_type = %d",
        occ_time, main_type, sub_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(AccessibilityStat));
    if (row[0]) strncpy(record->occ_time, row[0], sizeof(record->occ_time) - 1);
    record->main_type = row[1] ? atoi(row[1]) : 0;
    record->sub_type = row[2] ? atoi(row[2]) : 0;
    record->val = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int AccessibilityStat_GetAll(DBConnectionManager* manager, AccessibilityStat* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, main_type, sub_type, val FROM accessibility_stat");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(AccessibilityStat));
        if (row[0]) strncpy(records[count].occ_time, row[0], sizeof(records[count].occ_time) - 1);
        records[count].main_type = row[1] ? atoi(row[1]) : 0;
        records[count].sub_type = row[2] ? atoi(row[2]) : 0;
        records[count].val = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
