#include "captcha_invalid_request_type_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CaptchaInvalidRequestType_Add(DBConnectionManager* manager, const CaptchaInvalidRequestType* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO captcha_invalid_request_type (type_desc) "
        "VALUES ('%s')",
        record->type_desc);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CaptchaInvalidRequestType_Get(DBConnectionManager* manager, short type, CaptchaInvalidRequestType* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT type, type_desc FROM captcha_invalid_request_type WHERE type = %d",
        type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CaptchaInvalidRequestType));
    record->type = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->type_desc, row[1], sizeof(record->type_desc) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int CaptchaInvalidRequestType_GetAll(DBConnectionManager* manager, CaptchaInvalidRequestType* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT type, type_desc FROM captcha_invalid_request_type");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CaptchaInvalidRequestType));
        records[count].type = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].type_desc, row[1], sizeof(records[count].type_desc) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
