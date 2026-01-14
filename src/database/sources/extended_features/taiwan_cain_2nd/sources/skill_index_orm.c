#include "skill_index_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int SkillIndex_Add(DBConnectionManager* manager, const SkillIndex* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO skill_index (job, skill_idx, skill_name) "
        "VALUES (%d, %d, '%s')",
        record->job, record->skill_idx, record->skill_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int SkillIndex_Get(DBConnectionManager* manager, int no, SkillIndex* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, job, skill_idx, skill_name FROM skill_index WHERE no = %d",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(SkillIndex));
    record->no = row[0] ? atoi(row[0]) : 0;
    record->job = row[1] ? atoi(row[1]) : 0;
    record->skill_idx = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->skill_name, row[3], sizeof(record->skill_name) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int SkillIndex_GetAll(DBConnectionManager* manager, SkillIndex* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, job, skill_idx, skill_name FROM skill_index");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(SkillIndex));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        records[count].job = row[1] ? atoi(row[1]) : 0;
        records[count].skill_idx = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].skill_name, row[3], sizeof(records[count].skill_name) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
