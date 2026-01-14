#include "job_list_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int JobList_Add(DBConnectionManager* manager, const JobList* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO job_list (job_name) "
        "VALUES ('%s')",
        record->job_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int JobList_Get(DBConnectionManager* manager, signed char job_code, JobList* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT job_code, job_name FROM job_list WHERE job_code = %d",
        job_code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(JobList));
    record->job_code = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->job_name, row[1], sizeof(record->job_name) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int JobList_GetAll(DBConnectionManager* manager, JobList* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT job_code, job_name FROM job_list");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(JobList));
        records[count].job_code = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].job_name, row[1], sizeof(records[count].job_name) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
