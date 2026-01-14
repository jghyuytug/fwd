#include "captcha_invalid_request_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CaptchaInvalidRequest_Add(DBConnectionManager* manager, const CaptchaInvalidRequest* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO captcha_invalid_request (m_id, request_type, occ_time) "
        "VALUES (%d, %d, '%s')",
        record->m_id, record->request_type, record->occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CaptchaInvalidRequest_Get(DBConnectionManager* manager, int id, CaptchaInvalidRequest* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, m_id, request_type, occ_time FROM captcha_invalid_request WHERE id = %d",
        id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CaptchaInvalidRequest));
    record->id = row[0] ? atoi(row[0]) : 0;
    record->m_id = row[1] ? atoi(row[1]) : 0;
    record->request_type = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(record->occ_time, row[3], sizeof(record->occ_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int CaptchaInvalidRequest_GetAll(DBConnectionManager* manager, CaptchaInvalidRequest* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT id, m_id, request_type, occ_time FROM captcha_invalid_request");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CaptchaInvalidRequest));
        records[count].id = row[0] ? atoi(row[0]) : 0;
        records[count].m_id = row[1] ? atoi(row[1]) : 0;
        records[count].request_type = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].occ_time, row[3], sizeof(records[count].occ_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
