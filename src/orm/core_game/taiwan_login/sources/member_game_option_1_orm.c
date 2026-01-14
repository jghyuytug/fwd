#include "member_game_option_1_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

void MemberGameOption1_Init(MemberGameOption1* option) {
    if (!option) return;
    memset(option, 0, sizeof(MemberGameOption1));
}

void MemberGameOption1_Free(MemberGameOption1* option) {
    if (!option) return;

    if (option->option_1.data) {
        free(option->option_1.data);
        option->option_1.data = NULL;
        option->option_1.length = 0;
    }
    if (option->option_2.data) {
        free(option->option_2.data);
        option->option_2.data = NULL;
        option->option_2.length = 0;
    }
    if (option->option_3.data) {
        free(option->option_3.data);
        option->option_3.data = NULL;
        option->option_3.length = 0;
    }
}

int MemberGameOption1_Add(DBConnectionManager* manager, const MemberGameOption1* option) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !option) return -1;

    /* For simplicity, using empty blobs */
    snprintf(query, sizeof(query),
        "INSERT INTO member_game_option_1 (m_id, option_1, option_2, option_3) "
        "VALUES (%d, '', '', '')",
        option->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberGameOption1_Get(DBConnectionManager* manager, int m_id, MemberGameOption1* option) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !option) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, LENGTH(option_1), LENGTH(option_2), LENGTH(option_3) "
        "FROM member_game_option_1 WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    MemberGameOption1_Init(option);
    option->m_id = row[0] ? atoi(row[0]) : 0;
    option->option_1.length = row[1] ? atoi(row[1]) : 0;
    option->option_2.length = row[2] ? atoi(row[2]) : 0;
    option->option_3.length = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberGameOption1_Update(DBConnectionManager* manager, const MemberGameOption1* option) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !option) return -1;

    /* For simplicity, using empty blobs */
    snprintf(query, sizeof(query),
        "UPDATE member_game_option_1 SET option_1 = '', option_2 = '', "
        "option_3 = '' WHERE m_id = %d",
        option->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberGameOption1_Delete(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM member_game_option_1 WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int MemberGameOption1_Exists(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_game_option_1 WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

void MemberGameOption1_PrintInfo(const MemberGameOption1* option) {
    if (!option) return;

    printf("Member Game Option 1 Info:\n");
    printf("  Member ID: %d\n", option->m_id);
    printf("  Option 1 Length: %u bytes\n", option->option_1.length);
    printf("  Option 2 Length: %u bytes\n", option->option_2.length);
    printf("  Option 3 Length: %u bytes\n", option->option_3.length);
}
