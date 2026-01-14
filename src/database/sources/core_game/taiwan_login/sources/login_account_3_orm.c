#include "login_account_3_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int LoginAccount3_Add(DBConnectionManager* manager, const LoginAccount3* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO login_account_3 "
        "(m_id, m_channel_no, login_status, last_login_date, login_ip) "
        "VALUES (%u, %d, %u, '%s', '%s')",
        record->m_id, record->m_channel_no, record->login_status,
        record->last_login_date, record->login_ip);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LoginAccount3_Get(DBConnectionManager* manager, unsigned int m_id, LoginAccount3* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, m_channel_no, login_status, last_login_date, login_ip "
        "FROM login_account_3 WHERE m_id = %u", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LoginAccount3));
    record->m_id = row[0] ? (unsigned int)atol(row[0]) : 0;
    record->m_channel_no = row[1] ? atoi(row[1]) : 0;
    record->login_status = row[2] ? (unsigned char)atoi(row[2]) : 0;
    if (row[3]) strncpy(record->last_login_date, row[3], sizeof(record->last_login_date) - 1);
    if (row[4]) strncpy(record->login_ip, row[4], sizeof(record->login_ip) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int LoginAccount3_Update(DBConnectionManager* manager, const LoginAccount3* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "UPDATE login_account_3 SET "
        "m_channel_no = %d, login_status = %u, last_login_date = '%s', login_ip = '%s' "
        "WHERE m_id = %u",
        record->m_channel_no, record->login_status, record->last_login_date,
        record->login_ip, record->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LoginAccount3_Delete(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM login_account_3 WHERE m_id = %u", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LoginAccount3_Exists(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM login_account_3 WHERE m_id = %u", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    int count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return count > 0;
}

int LoginAccount3_UpdateLoginStatus(DBConnectionManager* manager, unsigned int m_id,
                                     unsigned char login_status) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE login_account_3 SET login_status = %u WHERE m_id = %u",
        login_status, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LoginAccount3_UpdateLastLogin(DBConnectionManager* manager, unsigned int m_id,
                                   const char* login_date, const char* login_ip) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !login_date || !login_ip) return -1;

    snprintf(query, sizeof(query),
        "UPDATE login_account_3 SET last_login_date = '%s', login_ip = '%s' WHERE m_id = %u",
        login_date, login_ip, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int LoginAccount3_GetByStatus(DBConnectionManager* manager, unsigned char login_status,
                               LoginAccount3* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, m_channel_no, login_status, last_login_date, login_ip "
        "FROM login_account_3 WHERE login_status = %u ORDER BY m_id",
        login_status);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LoginAccount3));
        records[count].m_id = row[0] ? (unsigned int)atol(row[0]) : 0;
        records[count].m_channel_no = row[1] ? atoi(row[1]) : 0;
        records[count].login_status = row[2] ? (unsigned char)atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].last_login_date, row[3], sizeof(records[count].last_login_date) - 1);
        if (row[4]) strncpy(records[count].login_ip, row[4], sizeof(records[count].login_ip) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int LoginAccount3_GetByChannel(DBConnectionManager* manager, int channel_no,
                                LoginAccount3* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, m_channel_no, login_status, last_login_date, login_ip "
        "FROM login_account_3 WHERE m_channel_no = %d ORDER BY m_id",
        channel_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LoginAccount3));
        records[count].m_id = row[0] ? (unsigned int)atol(row[0]) : 0;
        records[count].m_channel_no = row[1] ? atoi(row[1]) : 0;
        records[count].login_status = row[2] ? (unsigned char)atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].last_login_date, row[3], sizeof(records[count].last_login_date) - 1);
        if (row[4]) strncpy(records[count].login_ip, row[4], sizeof(records[count].login_ip) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void LoginAccount3_PrintInfo(const LoginAccount3* record) {
    if (!record) return;

    printf("Login Account 3 Info:\n");
    printf("  m_id: %u\n", record->m_id);
    printf("  m_channel_no: %d\n", record->m_channel_no);
    printf("  login_status: %u\n", record->login_status);
    printf("  last_login_date: %s\n", record->last_login_date);
    printf("  login_ip: %s\n", record->login_ip);
}
