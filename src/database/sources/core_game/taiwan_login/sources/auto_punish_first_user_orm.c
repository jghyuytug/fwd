#include "auto_punish_first_user_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int AutoPunishFirstUser_Add(DBConnectionManager* manager, const AutoPunishFirstUser* user) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !user) return -1;

    if (user->occ_time[0]) {
        snprintf(query, sizeof(query),
            "INSERT INTO auto_punish_first_user "
            "(m_id, occ_time, ip, hack_type, cnt, punish_flag, hack_sub_type, hack_sub_cnt) "
            "VALUES (%d, '%s', '%s', %u, %u, %d, %u, %u)",
            user->m_id, user->occ_time, user->ip, user->hack_type, user->cnt,
            user->punish_flag, user->hack_sub_type, user->hack_sub_cnt);
    } else {
        snprintf(query, sizeof(query),
            "INSERT INTO auto_punish_first_user "
            "(m_id, occ_time, ip, hack_type, cnt, punish_flag, hack_sub_type, hack_sub_cnt) "
            "VALUES (%d, NOW(), '%s', %u, %u, %d, %u, %u)",
            user->m_id, user->ip, user->hack_type, user->cnt,
            user->punish_flag, user->hack_sub_type, user->hack_sub_cnt);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishFirstUser_Get(DBConnectionManager* manager, int m_id,
                             unsigned short hack_type, unsigned short hack_sub_type,
                             AutoPunishFirstUser* user) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !user) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, ip, hack_type, cnt, punish_flag, hack_sub_type, hack_sub_cnt "
        "FROM auto_punish_first_user WHERE m_id = %d AND hack_type = %u AND hack_sub_type = %u",
        m_id, hack_type, hack_sub_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(user, 0, sizeof(AutoPunishFirstUser));
    user->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(user->occ_time, row[1], sizeof(user->occ_time) - 1);
    if (row[2]) strncpy(user->ip, row[2], sizeof(user->ip) - 1);
    user->hack_type = row[3] ? (unsigned short)atoi(row[3]) : 0;
    user->cnt = row[4] ? (unsigned int)atol(row[4]) : 0;
    user->punish_flag = row[5] ? (char)atoi(row[5]) : 0;
    user->hack_sub_type = row[6] ? (unsigned short)atoi(row[6]) : 0;
    user->hack_sub_cnt = row[7] ? (unsigned int)atol(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishFirstUser_Update(DBConnectionManager* manager, const AutoPunishFirstUser* user) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !user) return -1;

    snprintf(query, sizeof(query),
        "UPDATE auto_punish_first_user SET occ_time = '%s', ip = '%s', cnt = %u, "
        "punish_flag = %d, hack_sub_cnt = %u "
        "WHERE m_id = %d AND hack_type = %u AND hack_sub_type = %u",
        user->occ_time, user->ip, user->cnt, user->punish_flag, user->hack_sub_cnt,
        user->m_id, user->hack_type, user->hack_sub_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishFirstUser_Delete(DBConnectionManager* manager, int m_id,
                                unsigned short hack_type, unsigned short hack_sub_type) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM auto_punish_first_user WHERE m_id = %d AND hack_type = %u AND hack_sub_type = %u",
        m_id, hack_type, hack_sub_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishFirstUser_Exists(DBConnectionManager* manager, int m_id,
                                unsigned short hack_type, unsigned short hack_sub_type) {
    AutoPunishFirstUser user;
    return AutoPunishFirstUser_Get(manager, m_id, hack_type, hack_sub_type, &user) == 0 ? 1 : 0;
}

int AutoPunishFirstUser_GetByMId(DBConnectionManager* manager, int m_id,
                                  AutoPunishFirstUser* users, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !users || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, ip, hack_type, cnt, punish_flag, hack_sub_type, hack_sub_cnt "
        "FROM auto_punish_first_user WHERE m_id = %d ORDER BY occ_time DESC",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&users[count], 0, sizeof(AutoPunishFirstUser));
        users[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(users[count].occ_time, row[1], sizeof(users[count].occ_time) - 1);
        if (row[2]) strncpy(users[count].ip, row[2], sizeof(users[count].ip) - 1);
        users[count].hack_type = row[3] ? (unsigned short)atoi(row[3]) : 0;
        users[count].cnt = row[4] ? (unsigned int)atol(row[4]) : 0;
        users[count].punish_flag = row[5] ? (char)atoi(row[5]) : 0;
        users[count].hack_sub_type = row[6] ? (unsigned short)atoi(row[6]) : 0;
        users[count].hack_sub_cnt = row[7] ? (unsigned int)atol(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishFirstUser_GetByPunishFlag(DBConnectionManager* manager, char punish_flag,
                                         AutoPunishFirstUser* users, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !users || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, ip, hack_type, cnt, punish_flag, hack_sub_type, hack_sub_cnt "
        "FROM auto_punish_first_user WHERE punish_flag = %d ORDER BY occ_time DESC",
        punish_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&users[count], 0, sizeof(AutoPunishFirstUser));
        users[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(users[count].occ_time, row[1], sizeof(users[count].occ_time) - 1);
        if (row[2]) strncpy(users[count].ip, row[2], sizeof(users[count].ip) - 1);
        users[count].hack_type = row[3] ? (unsigned short)atoi(row[3]) : 0;
        users[count].cnt = row[4] ? (unsigned int)atol(row[4]) : 0;
        users[count].punish_flag = row[5] ? (char)atoi(row[5]) : 0;
        users[count].hack_sub_type = row[6] ? (unsigned short)atoi(row[6]) : 0;
        users[count].hack_sub_cnt = row[7] ? (unsigned int)atol(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void AutoPunishFirstUser_PrintInfo(const AutoPunishFirstUser* user) {
    if (!user) return;

    printf("=== AutoPunishFirstUser ===\n");
    printf("Member ID: %d\n", user->m_id);
    printf("Occur Time: %s\n", user->occ_time);
    printf("IP: %s\n", user->ip);
    printf("Hack Type: %u\n", user->hack_type);
    printf("Count: %u\n", user->cnt);
    printf("Punish Flag: %d\n", user->punish_flag);
    printf("Hack Sub Type: %u\n", user->hack_sub_type);
    printf("Hack Sub Count: %u\n", user->hack_sub_cnt);
    printf("=========================\n");
}
