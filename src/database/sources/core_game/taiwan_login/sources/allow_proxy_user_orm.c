#include "allow_proxy_user_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 1024

int AllowProxyUser_Add(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO allow_proxy_user (m_id) VALUES (%u)", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int AllowProxyUser_Delete(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM allow_proxy_user WHERE m_id = %u", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int AllowProxyUser_Exists(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM allow_proxy_user WHERE m_id = %u", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int AllowProxyUser_GetAll(DBConnectionManager* manager, AllowProxyUser* users,
                          int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int count = 0;

    if (!manager || !users || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id FROM allow_proxy_user ORDER BY m_id LIMIT %d", max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&users[count], 0, sizeof(AllowProxyUser));
        users[count].m_id = row[0] ? (unsigned int)strtoul(row[0], NULL, 10) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int AllowProxyUser_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int count = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM allow_proxy_user");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        count = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return count;
}

int AllowProxyUser_DeleteAll(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "DELETE FROM allow_proxy_user");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

void AllowProxyUser_PrintInfo(const AllowProxyUser* user) {
    if (!user) {
        printf("AllowProxyUser: NULL\n");
        return;
    }

    printf("AllowProxyUser:\n");
    printf("  m_id: %u\n", user->m_id);
}
