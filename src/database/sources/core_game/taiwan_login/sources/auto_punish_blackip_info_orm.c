#include "auto_punish_blackip_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int AutoPunishBlackipInfo_Add(DBConnectionManager* manager, const AutoPunishBlackipInfo* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !info) return -1;

    if (info->reg_date[0]) {
        snprintf(query, sizeof(query),
            "INSERT INTO auto_punish_blackip_info (ip, start_ip, end_ip, reg_date, apply_flag) "
            "VALUES ('%s', %u, %u, '%s', %d)",
            info->ip, info->start_ip, info->end_ip, info->reg_date, info->apply_flag);
    } else {
        snprintf(query, sizeof(query),
            "INSERT INTO auto_punish_blackip_info (ip, start_ip, end_ip, reg_date, apply_flag) "
            "VALUES ('%s', %u, %u, NOW(), %d)",
            info->ip, info->start_ip, info->end_ip, info->apply_flag);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishBlackipInfo_Get(DBConnectionManager* manager, const char* ip,
                                unsigned char start_ip, unsigned char end_ip,
                                AutoPunishBlackipInfo* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !ip || !info) return -1;

    snprintf(query, sizeof(query),
        "SELECT ip, start_ip, end_ip, reg_date, apply_flag "
        "FROM auto_punish_blackip_info WHERE ip = '%s' AND start_ip = %u AND end_ip = %u",
        ip, start_ip, end_ip);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(info, 0, sizeof(AutoPunishBlackipInfo));
    if (row[0]) strncpy(info->ip, row[0], sizeof(info->ip) - 1);
    info->start_ip = row[1] ? (unsigned char)atoi(row[1]) : 0;
    info->end_ip = row[2] ? (unsigned char)atoi(row[2]) : 0;
    if (row[3]) strncpy(info->reg_date, row[3], sizeof(info->reg_date) - 1);
    info->apply_flag = row[4] ? (char)atoi(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishBlackipInfo_Update(DBConnectionManager* manager, const AutoPunishBlackipInfo* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !info) return -1;

    snprintf(query, sizeof(query),
        "UPDATE auto_punish_blackip_info SET reg_date = '%s', apply_flag = %d "
        "WHERE ip = '%s' AND start_ip = %u AND end_ip = %u",
        info->reg_date, info->apply_flag, info->ip, info->start_ip, info->end_ip);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishBlackipInfo_Delete(DBConnectionManager* manager, const char* ip,
                                  unsigned char start_ip, unsigned char end_ip) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !ip) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM auto_punish_blackip_info WHERE ip = '%s' AND start_ip = %u AND end_ip = %u",
        ip, start_ip, end_ip);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishBlackipInfo_Exists(DBConnectionManager* manager, const char* ip,
                                  unsigned char start_ip, unsigned char end_ip) {
    AutoPunishBlackipInfo info;
    return AutoPunishBlackipInfo_Get(manager, ip, start_ip, end_ip, &info) == 0 ? 1 : 0;
}

int AutoPunishBlackipInfo_GetAll(DBConnectionManager* manager, AutoPunishBlackipInfo* infos,
                                  int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT ip, start_ip, end_ip, reg_date, apply_flag "
        "FROM auto_punish_blackip_info ORDER BY reg_date DESC");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(AutoPunishBlackipInfo));
        if (row[0]) strncpy(infos[count].ip, row[0], sizeof(infos[count].ip) - 1);
        infos[count].start_ip = row[1] ? (unsigned char)atoi(row[1]) : 0;
        infos[count].end_ip = row[2] ? (unsigned char)atoi(row[2]) : 0;
        if (row[3]) strncpy(infos[count].reg_date, row[3], sizeof(infos[count].reg_date) - 1);
        infos[count].apply_flag = row[4] ? (char)atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishBlackipInfo_GetByApplyFlag(DBConnectionManager* manager, char apply_flag,
                                          AutoPunishBlackipInfo* infos,
                                          int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT ip, start_ip, end_ip, reg_date, apply_flag "
        "FROM auto_punish_blackip_info WHERE apply_flag = %d ORDER BY reg_date DESC",
        apply_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(AutoPunishBlackipInfo));
        if (row[0]) strncpy(infos[count].ip, row[0], sizeof(infos[count].ip) - 1);
        infos[count].start_ip = row[1] ? (unsigned char)atoi(row[1]) : 0;
        infos[count].end_ip = row[2] ? (unsigned char)atoi(row[2]) : 0;
        if (row[3]) strncpy(infos[count].reg_date, row[3], sizeof(infos[count].reg_date) - 1);
        infos[count].apply_flag = row[4] ? (char)atoi(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void AutoPunishBlackipInfo_PrintInfo(const AutoPunishBlackipInfo* info) {
    if (!info) return;

    printf("=== AutoPunishBlackipInfo ===\n");
    printf("IP: %s\n", info->ip);
    printf("Start IP: %u\n", info->start_ip);
    printf("End IP: %u\n", info->end_ip);
    printf("Reg Date: %s\n", info->reg_date);
    printf("Apply Flag: %d\n", info->apply_flag);
    printf("============================\n");
}
