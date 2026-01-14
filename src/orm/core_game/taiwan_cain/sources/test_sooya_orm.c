#include "test_sooya_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 2048

int TestSooya_Add(DBConnectionManager* manager, const TestSooya* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO test_sooya (m_id, charac_no, lev) VALUES (%d, %d, %u)",
        record->m_id, record->charac_no, record->lev);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int TestSooya_Get(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_no, TestSooya* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_no, lev FROM test_sooya WHERE m_id = %u AND charac_no = %u",
        m_id, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(TestSooya));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->charac_no = row[1] ? atoi(row[1]) : 0;
    record->lev = row[2] ? (unsigned char)atoi(row[2]) : 1;

    DBQueryResult_Free(&result);
    return 0;
}

int TestSooya_Update(DBConnectionManager* manager, const TestSooya* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "UPDATE test_sooya SET lev = %u WHERE m_id = %d AND charac_no = %d",
        record->lev, record->m_id, record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int TestSooya_Delete(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM test_sooya WHERE m_id = %u AND charac_no = %u",
        m_id, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int TestSooya_Exists(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM test_sooya WHERE m_id = %u AND charac_no = %u",
        m_id, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return exists > 0;
}

void TestSooya_PrintInfo(const TestSooya* record) {
    if (!record) return;

    printf("TestSooya Record:\n");
    printf("  m_id: %d\n", record->m_id);
    printf("  charac_no: %d\n", record->charac_no);
    printf("  lev: %u\n", record->lev);
}

int TestSooya_Clear(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "DELETE FROM test_sooya");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int TestSooya_UpdateLevel(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_no, unsigned char new_lev) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE test_sooya SET lev = %u WHERE m_id = %u AND charac_no = %u",
        new_lev, m_id, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int TestSooya_IncrementLevel(DBConnectionManager* manager, unsigned int m_id, unsigned int charac_no, unsigned char increment) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE test_sooya SET lev = lev + %u WHERE m_id = %u AND charac_no = %u",
        increment, m_id, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int TestSooya_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int count = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM test_sooya");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        count = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return count;
}

int TestSooya_DeleteByMid(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM test_sooya WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}
