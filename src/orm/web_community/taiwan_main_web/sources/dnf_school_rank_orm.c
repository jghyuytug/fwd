#include "dnf_school_rank_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DnfSchoolRank_Add(DBConnectionManager* manager, const DnfSchoolRank* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_school_rank (category, grade, region, school_id, name, point, rank) "
        "VALUES (%d, %d, %d, %d, '%s', %d, %d)",
        record->category, record->grade, record->region, record->school_id, record->name, record->point, record->rank);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfSchoolRank_Get(DBConnectionManager* manager, signed char category, signed char grade, signed char region, int school_id, DnfSchoolRank* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT category, grade, region, school_id, name, point, rank FROM dnf_school_rank WHERE category = %d AND grade = %d AND region = %d AND school_id = %d",
        category, grade, region, school_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DnfSchoolRank));
    record->category = row[0] ? atoi(row[0]) : 0;
    record->grade = row[1] ? atoi(row[1]) : 0;
    record->region = row[2] ? atoi(row[2]) : 0;
    record->school_id = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->name, row[4], sizeof(record->name) - 1);
    record->point = row[5] ? atoll(row[5]) : 0;
    record->rank = row[6] ? atoi(row[6]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int DnfSchoolRank_GetAll(DBConnectionManager* manager, DnfSchoolRank* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT category, grade, region, school_id, name, point, rank FROM dnf_school_rank");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DnfSchoolRank));
        records[count].category = row[0] ? atoi(row[0]) : 0;
        records[count].grade = row[1] ? atoi(row[1]) : 0;
        records[count].region = row[2] ? atoi(row[2]) : 0;
        records[count].school_id = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].name, row[4], sizeof(records[count].name) - 1);
        records[count].point = row[5] ? atoll(row[5]) : 0;
        records[count].rank = row[6] ? atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
