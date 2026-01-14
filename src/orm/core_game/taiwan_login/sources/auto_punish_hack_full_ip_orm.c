#include "auto_punish_hack_full_ip_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int AutoPunishHackFullIp_Add(DBConnectionManager* manager, const AutoPunishHackFullIp* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !info) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO auto_punish_hack_full_ip (occ_date, hack_type, hack_sub_type, full_ip, cnt) "
        "VALUES ('%s', %u, %u, '%s', %u)",
        info->occ_date, info->hack_type, info->hack_sub_type, info->full_ip, info->cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishHackFullIp_Get(DBConnectionManager* manager, const char* occ_date,
                              unsigned short hack_type, unsigned short hack_sub_type,
                              const char* full_ip, AutoPunishHackFullIp* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !occ_date || !full_ip || !info) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, hack_type, hack_sub_type, full_ip, cnt "
        "FROM auto_punish_hack_full_ip WHERE occ_date = '%s' AND hack_type = %u "
        "AND hack_sub_type = %u AND full_ip = '%s'",
        occ_date, hack_type, hack_sub_type, full_ip);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(info, 0, sizeof(AutoPunishHackFullIp));
    if (row[0]) strncpy(info->occ_date, row[0], sizeof(info->occ_date) - 1);
    info->hack_type = row[1] ? (unsigned short)atoi(row[1]) : 0;
    info->hack_sub_type = row[2] ? (unsigned short)atoi(row[2]) : 0;
    if (row[3]) strncpy(info->full_ip, row[3], sizeof(info->full_ip) - 1);
    info->cnt = row[4] ? (unsigned int)atol(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishHackFullIp_Update(DBConnectionManager* manager, const AutoPunishHackFullIp* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !info) return -1;

    snprintf(query, sizeof(query),
        "UPDATE auto_punish_hack_full_ip SET cnt = %u "
        "WHERE occ_date = '%s' AND hack_type = %u AND hack_sub_type = %u AND full_ip = '%s'",
        info->cnt, info->occ_date, info->hack_type, info->hack_sub_type, info->full_ip);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishHackFullIp_Delete(DBConnectionManager* manager, const char* occ_date,
                                 unsigned short hack_type, unsigned short hack_sub_type,
                                 const char* full_ip) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_date || !full_ip) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM auto_punish_hack_full_ip WHERE occ_date = '%s' AND hack_type = %u "
        "AND hack_sub_type = %u AND full_ip = '%s'",
        occ_date, hack_type, hack_sub_type, full_ip);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishHackFullIp_Exists(DBConnectionManager* manager, const char* occ_date,
                                 unsigned short hack_type, unsigned short hack_sub_type,
                                 const char* full_ip) {
    AutoPunishHackFullIp info;
    return AutoPunishHackFullIp_Get(manager, occ_date, hack_type, hack_sub_type, full_ip, &info) == 0 ? 1 : 0;
}

int AutoPunishHackFullIp_GetAll(DBConnectionManager* manager, AutoPunishHackFullIp* infos,
                                 int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, hack_type, hack_sub_type, full_ip, cnt "
        "FROM auto_punish_hack_full_ip ORDER BY occ_date DESC");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(AutoPunishHackFullIp));
        if (row[0]) strncpy(infos[count].occ_date, row[0], sizeof(infos[count].occ_date) - 1);
        infos[count].hack_type = row[1] ? (unsigned short)atoi(row[1]) : 0;
        infos[count].hack_sub_type = row[2] ? (unsigned short)atoi(row[2]) : 0;
        if (row[3]) strncpy(infos[count].full_ip, row[3], sizeof(infos[count].full_ip) - 1);
        infos[count].cnt = row[4] ? (unsigned int)atol(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishHackFullIp_GetByDate(DBConnectionManager* manager, const char* occ_date,
                                    AutoPunishHackFullIp* infos, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !occ_date || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, hack_type, hack_sub_type, full_ip, cnt "
        "FROM auto_punish_hack_full_ip WHERE occ_date = '%s' ORDER BY cnt DESC",
        occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(AutoPunishHackFullIp));
        if (row[0]) strncpy(infos[count].occ_date, row[0], sizeof(infos[count].occ_date) - 1);
        infos[count].hack_type = row[1] ? (unsigned short)atoi(row[1]) : 0;
        infos[count].hack_sub_type = row[2] ? (unsigned short)atoi(row[2]) : 0;
        if (row[3]) strncpy(infos[count].full_ip, row[3], sizeof(infos[count].full_ip) - 1);
        infos[count].cnt = row[4] ? (unsigned int)atol(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishHackFullIp_GetByHackType(DBConnectionManager* manager, unsigned short hack_type,
                                        AutoPunishHackFullIp* infos, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, hack_type, hack_sub_type, full_ip, cnt "
        "FROM auto_punish_hack_full_ip WHERE hack_type = %u ORDER BY occ_date DESC",
        hack_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(AutoPunishHackFullIp));
        if (row[0]) strncpy(infos[count].occ_date, row[0], sizeof(infos[count].occ_date) - 1);
        infos[count].hack_type = row[1] ? (unsigned short)atoi(row[1]) : 0;
        infos[count].hack_sub_type = row[2] ? (unsigned short)atoi(row[2]) : 0;
        if (row[3]) strncpy(infos[count].full_ip, row[3], sizeof(infos[count].full_ip) - 1);
        infos[count].cnt = row[4] ? (unsigned int)atol(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void AutoPunishHackFullIp_PrintInfo(const AutoPunishHackFullIp* info) {
    if (!info) return;

    printf("=== AutoPunishHackFullIp ===\n");
    printf("Occurrence Date: %s\n", info->occ_date);
    printf("Hack Type: %u\n", info->hack_type);
    printf("Hack Sub Type: %u\n", info->hack_sub_type);
    printf("Full IP: %s\n", info->full_ip);
    printf("Count: %u\n", info->cnt);
    printf("============================\n");
}
