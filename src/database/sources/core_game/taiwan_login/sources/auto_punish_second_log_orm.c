#include "auto_punish_second_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int AutoPunishSecondLog_Add(DBConnectionManager* manager, const AutoPunishSecondLog* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !info) return -1;

    if (info->occ_time[0]) {
        snprintf(query, sizeof(query),
            "INSERT INTO auto_punish_second_log (m_id, hack_m_id, occ_time, trade_cnt, trade_gold) "
            "VALUES (%d, %d, '%s', %u, %llu)",
            info->m_id, info->hack_m_id, info->occ_time, info->trade_cnt, info->trade_gold);
    } else {
        snprintf(query, sizeof(query),
            "INSERT INTO auto_punish_second_log (m_id, hack_m_id, occ_time, trade_cnt, trade_gold) "
            "VALUES (%d, %d, NOW(), %u, %llu)",
            info->m_id, info->hack_m_id, info->trade_cnt, info->trade_gold);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishSecondLog_Get(DBConnectionManager* manager, int m_id, int hack_m_id,
                             AutoPunishSecondLog* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !info) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, hack_m_id, occ_time, trade_cnt, trade_gold "
        "FROM auto_punish_second_log WHERE m_id = %d AND hack_m_id = %d",
        m_id, hack_m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(info, 0, sizeof(AutoPunishSecondLog));
    info->m_id = row[0] ? atoi(row[0]) : 0;
    info->hack_m_id = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(info->occ_time, row[2], sizeof(info->occ_time) - 1);
    info->trade_cnt = row[3] ? (unsigned int)atol(row[3]) : 0;
    info->trade_gold = row[4] ? (unsigned long long)atoll(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishSecondLog_Update(DBConnectionManager* manager, const AutoPunishSecondLog* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !info) return -1;

    snprintf(query, sizeof(query),
        "UPDATE auto_punish_second_log SET occ_time = '%s', trade_cnt = %u, trade_gold = %llu "
        "WHERE m_id = %d AND hack_m_id = %d",
        info->occ_time, info->trade_cnt, info->trade_gold, info->m_id, info->hack_m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishSecondLog_Delete(DBConnectionManager* manager, int m_id, int hack_m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM auto_punish_second_log WHERE m_id = %d AND hack_m_id = %d",
        m_id, hack_m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishSecondLog_Exists(DBConnectionManager* manager, int m_id, int hack_m_id) {
    AutoPunishSecondLog info;
    return AutoPunishSecondLog_Get(manager, m_id, hack_m_id, &info) == 0 ? 1 : 0;
}

int AutoPunishSecondLog_GetAll(DBConnectionManager* manager, AutoPunishSecondLog* infos,
                                int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, hack_m_id, occ_time, trade_cnt, trade_gold "
        "FROM auto_punish_second_log ORDER BY occ_time DESC");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(AutoPunishSecondLog));
        infos[count].m_id = row[0] ? atoi(row[0]) : 0;
        infos[count].hack_m_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(infos[count].occ_time, row[2], sizeof(infos[count].occ_time) - 1);
        infos[count].trade_cnt = row[3] ? (unsigned int)atol(row[3]) : 0;
        infos[count].trade_gold = row[4] ? (unsigned long long)atoll(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishSecondLog_GetByMemberId(DBConnectionManager* manager, int m_id,
                                       AutoPunishSecondLog* infos, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, hack_m_id, occ_time, trade_cnt, trade_gold "
        "FROM auto_punish_second_log WHERE m_id = %d ORDER BY occ_time DESC",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(AutoPunishSecondLog));
        infos[count].m_id = row[0] ? atoi(row[0]) : 0;
        infos[count].hack_m_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(infos[count].occ_time, row[2], sizeof(infos[count].occ_time) - 1);
        infos[count].trade_cnt = row[3] ? (unsigned int)atol(row[3]) : 0;
        infos[count].trade_gold = row[4] ? (unsigned long long)atoll(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishSecondLog_GetByHackMemberId(DBConnectionManager* manager, int hack_m_id,
                                           AutoPunishSecondLog* infos, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, hack_m_id, occ_time, trade_cnt, trade_gold "
        "FROM auto_punish_second_log WHERE hack_m_id = %d ORDER BY occ_time DESC",
        hack_m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(AutoPunishSecondLog));
        infos[count].m_id = row[0] ? atoi(row[0]) : 0;
        infos[count].hack_m_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(infos[count].occ_time, row[2], sizeof(infos[count].occ_time) - 1);
        infos[count].trade_cnt = row[3] ? (unsigned int)atol(row[3]) : 0;
        infos[count].trade_gold = row[4] ? (unsigned long long)atoll(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void AutoPunishSecondLog_PrintInfo(const AutoPunishSecondLog* info) {
    if (!info) return;

    printf("=== AutoPunishSecondLog ===\n");
    printf("Member ID: %d\n", info->m_id);
    printf("Hack Member ID: %d\n", info->hack_m_id);
    printf("Occurrence Time: %s\n", info->occ_time);
    printf("Trade Count: %u\n", info->trade_cnt);
    printf("Trade Gold: %llu\n", info->trade_gold);
    printf("===========================\n");
}
