#include "human_certify_try_count_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int HumanCertifyTryCount_Add(DBConnectionManager* manager, const HumanCertifyTryCount* cert) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !cert) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO human_certify_try_count (m_id, count) VALUES (%d, %u)",
        cert->m_id, cert->count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int HumanCertifyTryCount_Get(DBConnectionManager* manager, int m_id, HumanCertifyTryCount* cert) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !cert) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, count FROM human_certify_try_count WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(cert, 0, sizeof(HumanCertifyTryCount));
    cert->m_id = row[0] ? atoi(row[0]) : 0;
    cert->count = row[1] ? (unsigned int)strtoul(row[1], NULL, 10) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int HumanCertifyTryCount_Update(DBConnectionManager* manager, const HumanCertifyTryCount* cert) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !cert) return -1;

    snprintf(query, sizeof(query),
        "UPDATE human_certify_try_count SET count = %u WHERE m_id = %d",
        cert->count, cert->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int HumanCertifyTryCount_Delete(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM human_certify_try_count WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int HumanCertifyTryCount_Exists(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM human_certify_try_count WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0 && row[0]) {
        int count = atoi(row[0]);
        DBQueryResult_Free(&result);
        return count > 0;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int HumanCertifyTryCount_Increment(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE human_certify_try_count SET count = count + 1 WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int HumanCertifyTryCount_Reset(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE human_certify_try_count SET count = 0 WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int HumanCertifyTryCount_GetAll(DBConnectionManager* manager,
                                 HumanCertifyTryCount* certs,
                                 int max_count,
                                 int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !certs || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, count FROM human_certify_try_count ORDER BY m_id");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&certs[count], 0, sizeof(HumanCertifyTryCount));
        certs[count].m_id = row[0] ? atoi(row[0]) : 0;
        certs[count].count = row[1] ? (unsigned int)strtoul(row[1], NULL, 10) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void HumanCertifyTryCount_PrintInfo(const HumanCertifyTryCount* cert) {
    if (!cert) return;

    printf("Human Certify Try Count:\n");
    printf("  Member ID: %d\n", cert->m_id);
    printf("  Try Count: %u\n", cert->count);
}
