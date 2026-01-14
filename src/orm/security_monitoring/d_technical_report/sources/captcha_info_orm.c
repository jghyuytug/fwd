#include "captcha_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CaptchaInfo_Add(DBConnectionManager* manager, const CaptchaInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO captcha_info (occ_time, total_try_count, success_count, fail_count, block_count, incomplete_request_count, invalid_request_count) "
        "VALUES ('%s', %d, %d, %d, %d, %d, %d)",
        record->occ_time, record->total_try_count, record->success_count, record->fail_count, record->block_count, record->incomplete_request_count, record->invalid_request_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CaptchaInfo_Get(DBConnectionManager* manager, int id, CaptchaInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, occ_time, total_try_count, success_count, fail_count, block_count, incomplete_request_count, invalid_request_count FROM captcha_info WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CaptchaInfo));
    record->id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->occ_time, row[1], sizeof(record->occ_time) - 1);
    record->total_try_count = row[2] ? atoi(row[2]) : 0;
    record->success_count = row[3] ? atoi(row[3]) : 0;
    record->fail_count = row[4] ? atoi(row[4]) : 0;
    record->block_count = row[5] ? atoi(row[5]) : 0;
    record->incomplete_request_count = row[6] ? atoi(row[6]) : 0;
    record->invalid_request_count = row[7] ? atoi(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int CaptchaInfo_GetAll(DBConnectionManager* manager, CaptchaInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, occ_time, total_try_count, success_count, fail_count, block_count, incomplete_request_count, invalid_request_count FROM captcha_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CaptchaInfo));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].occ_time, row[1], sizeof(records[count].occ_time) - 1);
        records[count].total_try_count = row[2] ? atoi(row[2]) : 0;
        records[count].success_count = row[3] ? atoi(row[3]) : 0;
        records[count].fail_count = row[4] ? atoi(row[4]) : 0;
        records[count].block_count = row[5] ? atoi(row[5]) : 0;
        records[count].incomplete_request_count = row[6] ? atoi(row[6]) : 0;
        records[count].invalid_request_count = row[7] ? atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
