#include "member_captcha_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int MemberCaptchaInfo_Add(DBConnectionManager* manager, const MemberCaptchaInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_captcha_info (m_id, cert_time, fail_count) "
        "VALUES (%d, %d, %d)",
        record->m_id, record->cert_time, record->fail_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberCaptchaInfo_Get(DBConnectionManager* manager, int m_id, MemberCaptchaInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, cert_time, fail_count FROM member_captcha_info WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(MemberCaptchaInfo));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->cert_time = row[1] ? atoi(row[1]) : 0;
    record->fail_count = row[2] ? atoi(row[2]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberCaptchaInfo_GetAll(DBConnectionManager* manager, MemberCaptchaInfo* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, cert_time, fail_count FROM member_captcha_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(MemberCaptchaInfo));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].cert_time = row[1] ? atoi(row[1]) : 0;
        records[count].fail_count = row[2] ? atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
