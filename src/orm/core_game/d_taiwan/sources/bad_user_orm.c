#include "bad_user_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* Add - no is auto_increment, not included in INSERT */
int BadUser_Add(DBConnectionManager* manager, const BadUser* user) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !user) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO bad_user (m_id, bad_code, create_day, exit_day, admin_n) "
        "VALUES (%d, %d, %d, %d, %d)",
        user->m_id, user->bad_code, user->create_day, user->exit_day, user->admin_n);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Get by primary key (no) */
int BadUser_Get(DBConnectionManager* manager, unsigned int no, BadUser* user) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !user) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, bad_code, create_day, exit_day, admin_n "
        "FROM bad_user WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(user, 0, sizeof(BadUser));
    user->no = row[0] ? (unsigned int)atoi(row[0]) : 0;
    user->m_id = row[1] ? atoi(row[1]) : 0;
    user->bad_code = row[2] ? atoi(row[2]) : 0;
    user->create_day = row[3] ? atoi(row[3]) : 0;
    user->exit_day = row[4] ? atoi(row[4]) : 0;
    user->admin_n = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

/* Update by primary key (no) */
int BadUser_Update(DBConnectionManager* manager, const BadUser* user) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !user) return -1;

    snprintf(query, sizeof(query),
        "UPDATE bad_user SET m_id = %d, bad_code = %d, create_day = %d, "
        "exit_day = %d, admin_n = %d WHERE no = %u",
        user->m_id, user->bad_code, user->create_day,
        user->exit_day, user->admin_n, user->no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Delete by primary key (no) */
int BadUser_Delete(DBConnectionManager* manager, unsigned int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query), "DELETE FROM bad_user WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Check if record exists */
int BadUser_Exists(DBConnectionManager* manager, unsigned int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM bad_user WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    DBQueryResult_Free(&result);
    return exists;
}

/* Get by m_id (indexed field) */
int BadUser_GetByMId(DBConnectionManager* manager, int m_id,
                      BadUser* users, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !users || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, bad_code, create_day, exit_day, admin_n "
        "FROM bad_user WHERE m_id = %d ORDER BY no", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&users[count], 0, sizeof(BadUser));
        users[count].no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        users[count].m_id = row[1] ? atoi(row[1]) : 0;
        users[count].bad_code = row[2] ? atoi(row[2]) : 0;
        users[count].create_day = row[3] ? atoi(row[3]) : 0;
        users[count].exit_day = row[4] ? atoi(row[4]) : 0;
        users[count].admin_n = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get by bad_code (indexed field) */
int BadUser_GetByBadCode(DBConnectionManager* manager, int bad_code,
                          BadUser* users, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !users || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, bad_code, create_day, exit_day, admin_n "
        "FROM bad_user WHERE bad_code = %d ORDER BY no", bad_code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&users[count], 0, sizeof(BadUser));
        users[count].no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        users[count].m_id = row[1] ? atoi(row[1]) : 0;
        users[count].bad_code = row[2] ? atoi(row[2]) : 0;
        users[count].create_day = row[3] ? atoi(row[3]) : 0;
        users[count].exit_day = row[4] ? atoi(row[4]) : 0;
        users[count].admin_n = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get by exit_day (indexed field) */
int BadUser_GetByExitDay(DBConnectionManager* manager, int exit_day,
                          BadUser* users, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !users || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, bad_code, create_day, exit_day, admin_n "
        "FROM bad_user WHERE exit_day = %d ORDER BY no", exit_day);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&users[count], 0, sizeof(BadUser));
        users[count].no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        users[count].m_id = row[1] ? atoi(row[1]) : 0;
        users[count].bad_code = row[2] ? atoi(row[2]) : 0;
        users[count].create_day = row[3] ? atoi(row[3]) : 0;
        users[count].exit_day = row[4] ? atoi(row[4]) : 0;
        users[count].admin_n = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get all records */
int BadUser_GetAll(DBConnectionManager* manager, BadUser* users,
                    int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !users || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, m_id, bad_code, create_day, exit_day, admin_n "
        "FROM bad_user ORDER BY no LIMIT %d", max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&users[count], 0, sizeof(BadUser));
        users[count].no = row[0] ? (unsigned int)atoi(row[0]) : 0;
        users[count].m_id = row[1] ? atoi(row[1]) : 0;
        users[count].bad_code = row[2] ? atoi(row[2]) : 0;
        users[count].create_day = row[3] ? atoi(row[3]) : 0;
        users[count].exit_day = row[4] ? atoi(row[4]) : 0;
        users[count].admin_n = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Print user information */
void BadUser_PrintInfo(const BadUser* user) {
    if (!user) {
        printf("BadUser: NULL\n");
        return;
    }

    printf("BadUser:\n");
    printf("  no: %u\n", user->no);
    printf("  m_id: %d\n", user->m_id);
    printf("  bad_code: %d\n", user->bad_code);
    printf("  create_day: %d\n", user->create_day);
    printf("  exit_day: %d\n", user->exit_day);
    printf("  admin_n: %d\n", user->admin_n);
}
