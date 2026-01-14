#include "prod_job_ability_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int ProdJobAbilityInfo_Add(DBConnectionManager* manager, const ProdJobAbilityInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO prod_job_ability_info (job, grow_type, skill_index, skill_name) "
        "VALUES (%d, %d, %d, '%s')",
        record->job, record->grow_type, record->skill_index, record->skill_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ProdJobAbilityInfo_Get(DBConnectionManager* manager, signed char job, signed char grow_type, int skill_index, ProdJobAbilityInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT job, grow_type, skill_index, skill_name FROM prod_job_ability_info WHERE job = %d AND grow_type = %d AND skill_index = %d",
        job, grow_type, skill_index);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(ProdJobAbilityInfo));
    record->job = row[0] ? atoi(row[0]) : 0;
    record->grow_type = row[1] ? atoi(row[1]) : 0;
    record->skill_index = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->skill_name, row[3], sizeof(record->skill_name) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int ProdJobAbilityInfo_GetAll(DBConnectionManager* manager, ProdJobAbilityInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT job, grow_type, skill_index, skill_name FROM prod_job_ability_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_PROD, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(ProdJobAbilityInfo));
        records[count].job = row[0] ? atoi(row[0]) : 0;
        records[count].grow_type = row[1] ? atoi(row[1]) : 0;
        records[count].skill_index = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].skill_name, row[3], sizeof(records[count].skill_name) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
