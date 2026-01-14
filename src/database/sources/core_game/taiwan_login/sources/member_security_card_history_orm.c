#include "member_security_card_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int MemberSecurityCardHistory_Add(DBConnectionManager* manager, const MemberSecurityCardHistory* history) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !history) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO member_security_card_history (occ_time, m_id, modify_type) "
        "VALUES ('%s', %d, %d)",
        history->occ_time, history->m_id, history->modify_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSecurityCardHistory_Get(DBConnectionManager* manager, const char* occ_time, int m_id,
                                   MemberSecurityCardHistory* history) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !occ_time || !history) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, m_id, modify_type FROM member_security_card_history "
        "WHERE occ_time = '%s' AND m_id = %d", occ_time, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(history, 0, sizeof(MemberSecurityCardHistory));
    if (row[0]) strncpy(history->occ_time, row[0], sizeof(history->occ_time) - 1);
    history->m_id = row[1] ? atoi(row[1]) : 0;
    history->modify_type = row[2] ? (signed char)atoi(row[2]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSecurityCardHistory_Delete(DBConnectionManager* manager, const char* occ_time, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_time) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM member_security_card_history WHERE occ_time = '%s' AND m_id = %d",
        occ_time, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberSecurityCardHistory_Exists(DBConnectionManager* manager, const char* occ_time, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !occ_time) return 0;

    snprintf(query, sizeof(query),
        "SELECT 1 FROM member_security_card_history "
        "WHERE occ_time = '%s' AND m_id = %d LIMIT 1", occ_time, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return 0;

    int exists = (DBQueryResult_FetchRow(&result, row) > 0) ? 1 : 0;
    DBQueryResult_Free(&result);
    return exists;
}

int MemberSecurityCardHistory_GetByMember(DBConnectionManager* manager, int m_id,
                                           MemberSecurityCardHistory* histories,
                                           int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !histories || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, m_id, modify_type FROM member_security_card_history "
        "WHERE m_id = %d ORDER BY occ_time DESC", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&histories[count], 0, sizeof(MemberSecurityCardHistory));
        if (row[0]) strncpy(histories[count].occ_time, row[0], sizeof(histories[count].occ_time) - 1);
        histories[count].m_id = row[1] ? atoi(row[1]) : 0;
        histories[count].modify_type = row[2] ? (signed char)atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberSecurityCardHistory_GetByType(DBConnectionManager* manager, signed char modify_type,
                                         MemberSecurityCardHistory* histories,
                                         int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !histories || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, m_id, modify_type FROM member_security_card_history "
        "WHERE modify_type = %d ORDER BY occ_time DESC", modify_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&histories[count], 0, sizeof(MemberSecurityCardHistory));
        if (row[0]) strncpy(histories[count].occ_time, row[0], sizeof(histories[count].occ_time) - 1);
        histories[count].m_id = row[1] ? atoi(row[1]) : 0;
        histories[count].modify_type = row[2] ? (signed char)atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int MemberSecurityCardHistory_GetRecent(DBConnectionManager* manager, int m_id,
                                         int limit, MemberSecurityCardHistory* histories,
                                         int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !histories || !actual_count || limit <= 0) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_time, m_id, modify_type FROM member_security_card_history "
        "WHERE m_id = %d ORDER BY occ_time DESC LIMIT %d", m_id, limit);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < limit && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&histories[count], 0, sizeof(MemberSecurityCardHistory));
        if (row[0]) strncpy(histories[count].occ_time, row[0], sizeof(histories[count].occ_time) - 1);
        histories[count].m_id = row[1] ? atoi(row[1]) : 0;
        histories[count].modify_type = row[2] ? (signed char)atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void MemberSecurityCardHistory_PrintInfo(const MemberSecurityCardHistory* history) {
    if (!history) return;

    printf("MemberSecurityCardHistory {\n");
    printf("  occ_time: %s\n", history->occ_time);
    printf("  m_id: %d\n", history->m_id);
    printf("  modify_type: %d\n", history->modify_type);
    printf("}\n");
}
