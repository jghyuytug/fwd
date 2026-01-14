#include "auto_punish_hack_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int AutoPunishHackInfo_Add(DBConnectionManager* manager, const AutoPunishHackInfo* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !info) return -1;

    if (info->reg_date[0]) {
        snprintf(query, sizeof(query),
            "INSERT INTO auto_punish_hack_info (hack_type, cnt, etc, reg_date, apply_flag, "
            "hack_sub_type, hack_sub_cnt, ip_cnt) "
            "VALUES (%u, %u, %llu, '%s', %d, %u, %u, %u)",
            info->hack_type, info->cnt, info->etc, info->reg_date, info->apply_flag,
            info->hack_sub_type, info->hack_sub_cnt, info->ip_cnt);
    } else {
        snprintf(query, sizeof(query),
            "INSERT INTO auto_punish_hack_info (hack_type, cnt, etc, reg_date, apply_flag, "
            "hack_sub_type, hack_sub_cnt, ip_cnt) "
            "VALUES (%u, %u, %llu, NOW(), %d, %u, %u, %u)",
            info->hack_type, info->cnt, info->etc, info->apply_flag,
            info->hack_sub_type, info->hack_sub_cnt, info->ip_cnt);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishHackInfo_Get(DBConnectionManager* manager, unsigned short hack_type,
                            char apply_flag, unsigned short hack_sub_type,
                            AutoPunishHackInfo* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !info) return -1;

    snprintf(query, sizeof(query),
        "SELECT hack_type, cnt, etc, reg_date, apply_flag, hack_sub_type, hack_sub_cnt, ip_cnt "
        "FROM auto_punish_hack_info WHERE hack_type = %u AND apply_flag = %d AND hack_sub_type = %u",
        hack_type, apply_flag, hack_sub_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(info, 0, sizeof(AutoPunishHackInfo));
    info->hack_type = row[0] ? (unsigned short)atoi(row[0]) : 0;
    info->cnt = row[1] ? (unsigned int)atol(row[1]) : 0;
    info->etc = row[2] ? (unsigned long long)atoll(row[2]) : 0;
    if (row[3]) strncpy(info->reg_date, row[3], sizeof(info->reg_date) - 1);
    info->apply_flag = row[4] ? (char)atoi(row[4]) : 0;
    info->hack_sub_type = row[5] ? (unsigned short)atoi(row[5]) : 0;
    info->hack_sub_cnt = row[6] ? (unsigned int)atol(row[6]) : 0;
    info->ip_cnt = row[7] ? (unsigned int)atol(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishHackInfo_Update(DBConnectionManager* manager, const AutoPunishHackInfo* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !info) return -1;

    snprintf(query, sizeof(query),
        "UPDATE auto_punish_hack_info SET cnt = %u, etc = %llu, reg_date = '%s', "
        "hack_sub_cnt = %u, ip_cnt = %u "
        "WHERE hack_type = %u AND apply_flag = %d AND hack_sub_type = %u",
        info->cnt, info->etc, info->reg_date, info->hack_sub_cnt, info->ip_cnt,
        info->hack_type, info->apply_flag, info->hack_sub_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishHackInfo_Delete(DBConnectionManager* manager, unsigned short hack_type,
                               char apply_flag, unsigned short hack_sub_type) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM auto_punish_hack_info WHERE hack_type = %u AND apply_flag = %d "
        "AND hack_sub_type = %u",
        hack_type, apply_flag, hack_sub_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishHackInfo_Exists(DBConnectionManager* manager, unsigned short hack_type,
                               char apply_flag, unsigned short hack_sub_type) {
    AutoPunishHackInfo info;
    return AutoPunishHackInfo_Get(manager, hack_type, apply_flag, hack_sub_type, &info) == 0 ? 1 : 0;
}

int AutoPunishHackInfo_GetAll(DBConnectionManager* manager, AutoPunishHackInfo* infos,
                               int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT hack_type, cnt, etc, reg_date, apply_flag, hack_sub_type, hack_sub_cnt, ip_cnt "
        "FROM auto_punish_hack_info ORDER BY reg_date DESC");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(AutoPunishHackInfo));
        infos[count].hack_type = row[0] ? (unsigned short)atoi(row[0]) : 0;
        infos[count].cnt = row[1] ? (unsigned int)atol(row[1]) : 0;
        infos[count].etc = row[2] ? (unsigned long long)atoll(row[2]) : 0;
        if (row[3]) strncpy(infos[count].reg_date, row[3], sizeof(infos[count].reg_date) - 1);
        infos[count].apply_flag = row[4] ? (char)atoi(row[4]) : 0;
        infos[count].hack_sub_type = row[5] ? (unsigned short)atoi(row[5]) : 0;
        infos[count].hack_sub_cnt = row[6] ? (unsigned int)atol(row[6]) : 0;
        infos[count].ip_cnt = row[7] ? (unsigned int)atol(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishHackInfo_GetByHackType(DBConnectionManager* manager, unsigned short hack_type,
                                      AutoPunishHackInfo* infos, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT hack_type, cnt, etc, reg_date, apply_flag, hack_sub_type, hack_sub_cnt, ip_cnt "
        "FROM auto_punish_hack_info WHERE hack_type = %u ORDER BY reg_date DESC",
        hack_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(AutoPunishHackInfo));
        infos[count].hack_type = row[0] ? (unsigned short)atoi(row[0]) : 0;
        infos[count].cnt = row[1] ? (unsigned int)atol(row[1]) : 0;
        infos[count].etc = row[2] ? (unsigned long long)atoll(row[2]) : 0;
        if (row[3]) strncpy(infos[count].reg_date, row[3], sizeof(infos[count].reg_date) - 1);
        infos[count].apply_flag = row[4] ? (char)atoi(row[4]) : 0;
        infos[count].hack_sub_type = row[5] ? (unsigned short)atoi(row[5]) : 0;
        infos[count].hack_sub_cnt = row[6] ? (unsigned int)atol(row[6]) : 0;
        infos[count].ip_cnt = row[7] ? (unsigned int)atol(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishHackInfo_GetByApplyFlag(DBConnectionManager* manager, char apply_flag,
                                       AutoPunishHackInfo* infos, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT hack_type, cnt, etc, reg_date, apply_flag, hack_sub_type, hack_sub_cnt, ip_cnt "
        "FROM auto_punish_hack_info WHERE apply_flag = %d ORDER BY reg_date DESC",
        apply_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(AutoPunishHackInfo));
        infos[count].hack_type = row[0] ? (unsigned short)atoi(row[0]) : 0;
        infos[count].cnt = row[1] ? (unsigned int)atol(row[1]) : 0;
        infos[count].etc = row[2] ? (unsigned long long)atoll(row[2]) : 0;
        if (row[3]) strncpy(infos[count].reg_date, row[3], sizeof(infos[count].reg_date) - 1);
        infos[count].apply_flag = row[4] ? (char)atoi(row[4]) : 0;
        infos[count].hack_sub_type = row[5] ? (unsigned short)atoi(row[5]) : 0;
        infos[count].hack_sub_cnt = row[6] ? (unsigned int)atol(row[6]) : 0;
        infos[count].ip_cnt = row[7] ? (unsigned int)atol(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void AutoPunishHackInfo_PrintInfo(const AutoPunishHackInfo* info) {
    if (!info) return;

    printf("=== AutoPunishHackInfo ===\n");
    printf("Hack Type: %u\n", info->hack_type);
    printf("Count: %u\n", info->cnt);
    printf("ETC: %llu\n", info->etc);
    printf("Reg Date: %s\n", info->reg_date);
    printf("Apply Flag: %d\n", info->apply_flag);
    printf("Hack Sub Type: %u\n", info->hack_sub_type);
    printf("Hack Sub Count: %u\n", info->hack_sub_cnt);
    printf("IP Count: %u\n", info->ip_cnt);
    printf("==========================\n");
}
