#include "auto_punish_second_user_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int AutoPunishSecondUser_Add(DBConnectionManager* manager, const AutoPunishSecondUser* user) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !user) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO auto_punish_second_user (m_id, occ_time, total_trade_cnt, trade_cnt, "
        "total_trade_gold, trade_gold, punish_flag) VALUES (%d, '%s', %u, %u, %llu, %llu, %d)",
        user->m_id, user->occ_time, user->total_trade_cnt, user->trade_cnt,
        user->total_trade_gold, user->trade_gold, user->punish_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishSecondUser_Get(DBConnectionManager* manager, int m_id, AutoPunishSecondUser* user) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !user) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, total_trade_cnt, trade_cnt, total_trade_gold, trade_gold, punish_flag "
        "FROM auto_punish_second_user WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(user, 0, sizeof(AutoPunishSecondUser));
    user->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(user->occ_time, row[1], sizeof(user->occ_time) - 1);
    user->total_trade_cnt = row[2] ? (unsigned int)atoi(row[2]) : 0;
    user->trade_cnt = row[3] ? (unsigned int)atoi(row[3]) : 0;
    user->total_trade_gold = row[4] ? (unsigned long long)atoll(row[4]) : 0;
    user->trade_gold = row[5] ? (unsigned long long)atoll(row[5]) : 0;
    user->punish_flag = row[6] ? (char)atoi(row[6]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishSecondUser_Update(DBConnectionManager* manager, const AutoPunishSecondUser* user) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !user) return -1;

    snprintf(query, sizeof(query),
        "UPDATE auto_punish_second_user SET occ_time = '%s', total_trade_cnt = %u, "
        "trade_cnt = %u, total_trade_gold = %llu, trade_gold = %llu, punish_flag = %d "
        "WHERE m_id = %d",
        user->occ_time, user->total_trade_cnt, user->trade_cnt,
        user->total_trade_gold, user->trade_gold, user->punish_flag, user->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishSecondUser_Delete(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM auto_punish_second_user WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishSecondUser_Exists(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT 1 FROM auto_punish_second_user WHERE m_id = %d LIMIT 1", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = 1;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int AutoPunishSecondUser_GetAll(DBConnectionManager* manager, AutoPunishSecondUser* users,
                                  int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !users || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, total_trade_cnt, trade_cnt, total_trade_gold, trade_gold, punish_flag "
        "FROM auto_punish_second_user ORDER BY m_id");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&users[count], 0, sizeof(AutoPunishSecondUser));
        users[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(users[count].occ_time, row[1], sizeof(users[count].occ_time) - 1);
        users[count].total_trade_cnt = row[2] ? (unsigned int)atoi(row[2]) : 0;
        users[count].trade_cnt = row[3] ? (unsigned int)atoi(row[3]) : 0;
        users[count].total_trade_gold = row[4] ? (unsigned long long)atoll(row[4]) : 0;
        users[count].trade_gold = row[5] ? (unsigned long long)atoll(row[5]) : 0;
        users[count].punish_flag = row[6] ? (char)atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishSecondUser_GetByPunishFlag(DBConnectionManager* manager, char punish_flag,
                                           AutoPunishSecondUser* users, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !users || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, total_trade_cnt, trade_cnt, total_trade_gold, trade_gold, punish_flag "
        "FROM auto_punish_second_user WHERE punish_flag = %d ORDER BY m_id", punish_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&users[count], 0, sizeof(AutoPunishSecondUser));
        users[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(users[count].occ_time, row[1], sizeof(users[count].occ_time) - 1);
        users[count].total_trade_cnt = row[2] ? (unsigned int)atoi(row[2]) : 0;
        users[count].trade_cnt = row[3] ? (unsigned int)atoi(row[3]) : 0;
        users[count].total_trade_gold = row[4] ? (unsigned long long)atoll(row[4]) : 0;
        users[count].trade_gold = row[5] ? (unsigned long long)atoll(row[5]) : 0;
        users[count].punish_flag = row[6] ? (char)atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void AutoPunishSecondUser_PrintInfo(const AutoPunishSecondUser* user) {
    if (!user) return;

    printf("=== Auto Punish Second User Info ===\n");
    printf("Member ID: %d\n", user->m_id);
    printf("Occurred Time: %s\n", user->occ_time);
    printf("Total Trade Count: %u\n", user->total_trade_cnt);
    printf("Trade Count: %u\n", user->trade_cnt);
    printf("Total Trade Gold: %llu\n", user->total_trade_gold);
    printf("Trade Gold: %llu\n", user->trade_gold);
    printf("Punish Flag: %d\n", user->punish_flag);
    printf("===================================\n");
}
