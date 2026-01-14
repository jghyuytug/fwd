#include "member_captcha_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/* ======================================== CRUD Operations ======================================== */

int MemberCaptchaInfo_Add(DBConnectionManager* manager, const MemberCaptchaInfo* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "REPLACE INTO member_captcha_info "
        "(m_id, cert_time, fail_count) "
        "VALUES (%u, %u, %u)",
        record->m_id, record->cert_time, record->fail_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberCaptchaInfo_Get(DBConnectionManager* manager, unsigned int m_id, MemberCaptchaInfo* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, cert_time, fail_count "
        "FROM member_captcha_info WHERE m_id = %u",
        m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    int ret = DBQueryResult_FetchRow(&result, values);

    if (ret > 0) {
        record->m_id = (unsigned int)atoi(values[0]);
        record->cert_time = (unsigned int)atoi(values[1]);
        record->fail_count = (unsigned char)atoi(values[2]);
    }

    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

int MemberCaptchaInfo_Update(DBConnectionManager* manager, const MemberCaptchaInfo* record) {
    if (!manager || !record) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_captcha_info SET cert_time = %u, fail_count = %u "
        "WHERE m_id = %u",
        record->cert_time, record->fail_count, record->m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberCaptchaInfo_Delete(DBConnectionManager* manager, unsigned int m_id) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "DELETE FROM member_captcha_info WHERE m_id = %u", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberCaptchaInfo_Exists(DBConnectionManager* manager, unsigned int m_id) {
    if (!manager) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_captcha_info WHERE m_id = %u", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    char* values[MAX_FIELDS];
    int exists = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = (atoi(values[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* ======================================== Business Queries ======================================== */

int MemberCaptchaInfo_IncrementFailCount(DBConnectionManager* manager, unsigned int m_id) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_captcha_info SET fail_count = fail_count + 1 "
        "WHERE m_id = %u",
        m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberCaptchaInfo_ResetFailCount(DBConnectionManager* manager, unsigned int m_id) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_captcha_info SET fail_count = 0 WHERE m_id = %u",
        m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberCaptchaInfo_GetByFailCountRange(DBConnectionManager* manager, int min_count,
                                           int max_count_val, MemberCaptchaInfo* records,
                                           int max_count) {
    if (!manager || !records || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, cert_time, fail_count "
        "FROM member_captcha_info WHERE fail_count BETWEEN %d AND %d LIMIT %d",
        min_count, max_count_val, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        records[count].cert_time = (unsigned int)atoi(values[1]);
        records[count].fail_count = (unsigned char)atoi(values[2]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberCaptchaInfo_GetByTimeRange(DBConnectionManager* manager, unsigned int start_time,
                                      unsigned int end_time, MemberCaptchaInfo* records,
                                      int max_count) {
    if (!manager || !records || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, cert_time, fail_count "
        "FROM member_captcha_info WHERE cert_time BETWEEN %u AND %u LIMIT %d",
        start_time, end_time, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        records[count].cert_time = (unsigned int)atoi(values[1]);
        records[count].fail_count = (unsigned char)atoi(values[2]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberCaptchaInfo_GetRecentVerified(DBConnectionManager* manager, MemberCaptchaInfo* records,
                                         int limit) {
    if (!manager || !records || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, cert_time, fail_count "
        "FROM member_captcha_info ORDER BY cert_time DESC LIMIT %d",
        limit);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < limit && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        records[count].cert_time = (unsigned int)atoi(values[1]);
        records[count].fail_count = (unsigned char)atoi(values[2]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberCaptchaInfo_GetAll(DBConnectionManager* manager, MemberCaptchaInfo* records,
                              int offset, int limit) {
    if (!manager || !records || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, cert_time, fail_count "
        "FROM member_captcha_info LIMIT %d, %d",
        offset, limit);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < limit && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].m_id = (unsigned int)atoi(values[0]);
        records[count].cert_time = (unsigned int)atoi(values[1]);
        records[count].fail_count = (unsigned char)atoi(values[2]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberCaptchaInfo_GetCount(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM member_captcha_info");

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ======================================== Bulk Operations ======================================== */

int MemberCaptchaInfo_BulkAdd(DBConnectionManager* manager, const MemberCaptchaInfo* records,
                               int count) {
    if (!manager || !records || count <= 0) {
        return -1;
    }

    int success_count = 0;
    int i;

    for (i = 0; i < count; i++) {
        if (MemberCaptchaInfo_Add(manager, &records[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

int MemberCaptchaInfo_BulkDelete(DBConnectionManager* manager, const unsigned int* m_ids,
                                  int count) {
    if (!manager || !m_ids || count <= 0) {
        return -1;
    }

    int i;
    for (i = 0; i < count; i++) {
        if (MemberCaptchaInfo_Delete(manager, m_ids[i]) != 0) {
            return -1;
        }
    }

    return 0;
}

/* ======================================== Utility Functions ======================================== */

void MemberCaptchaInfo_PrintInfo(const MemberCaptchaInfo* record) {
    if (!record) {
        printf("MemberCaptchaInfo is NULL\n");
        return;
    }

    printf("=== Member Captcha Info ===\n");
    printf("  m_id:        %u\n", record->m_id);
    printf("  cert_time:   %u\n", record->cert_time);
    printf("  fail_count:  %u\n", record->fail_count);
    printf("===========================\n");
}
