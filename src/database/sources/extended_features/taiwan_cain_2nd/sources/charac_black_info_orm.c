#include "charac_black_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CharacBlackInfo_Add(DBConnectionManager* manager, const CharacBlackInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO charac_black_info (charac_no, black_point, offset_point, problem_child_time) "
        "VALUES (%d, %d, %d, '%s')",
        record->charac_no, record->black_point, record->offset_point, record->problem_child_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacBlackInfo_Get(DBConnectionManager* manager, int charac_no, CharacBlackInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, black_point, offset_point, problem_child_time FROM charac_black_info WHERE charac_no = %d",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CharacBlackInfo));
    record->charac_no = row[0] ? atoi(row[0]) : 0;
    record->black_point = row[1] ? atoi(row[1]) : 0;
    record->offset_point = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->problem_child_time, row[3], sizeof(record->problem_child_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int CharacBlackInfo_GetAll(DBConnectionManager* manager, CharacBlackInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT charac_no, black_point, offset_point, problem_child_time FROM charac_black_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CharacBlackInfo));
        records[count].charac_no = row[0] ? atoi(row[0]) : 0;
        records[count].black_point = row[1] ? atoi(row[1]) : 0;
        records[count].offset_point = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].problem_child_time, row[3], sizeof(records[count].problem_child_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
