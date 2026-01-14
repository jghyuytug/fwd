#include "pu_user_list_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int PuUserList_Add(DBConnectionManager* manager, const PuUserList* user) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !user) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO pu_user_list (m_id) VALUES (%u)", user->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int PuUserList_Get(DBConnectionManager* manager, unsigned int m_id, PuUserList* user) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !user) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id FROM pu_user_list WHERE m_id = %u", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(user, 0, sizeof(PuUserList));
    user->m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int PuUserList_Update(DBConnectionManager* manager, const PuUserList* user) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !user) return -1;

    snprintf(query, sizeof(query),
        "UPDATE pu_user_list SET m_id = %u WHERE m_id = %u",
        user->m_id, user->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int PuUserList_Delete(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM pu_user_list WHERE m_id = %u", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int PuUserList_Exists(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM pu_user_list WHERE m_id = %u", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int PuUserList_GetAll(DBConnectionManager* manager, PuUserList* users,
                       int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int count = 0;

    if (!manager || !users || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id FROM pu_user_list ORDER BY m_id");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&users[count], 0, sizeof(PuUserList));
        users[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void PuUserList_PrintInfo(const PuUserList* user) {
    if (!user) {
        printf("PuUserList: NULL\n");
        return;
    }

    printf("PuUserList: m_id=%u\n", user->m_id);
}
