#include "auto_punish_hack_ip_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int AutoPunishHackIp_Add(DBConnectionManager* manager, const AutoPunishHackIp* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !info) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO auto_punish_hack_ip (occ_date, hack_type, hack_sub_type, c_class_ip, cnt) "
        "VALUES ('%s', %u, %u, '%s', %u)",
        info->occ_date, info->hack_type, info->hack_sub_type, info->c_class_ip, info->cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishHackIp_Get(DBConnectionManager* manager, const char* occ_date,
                          unsigned short hack_type, unsigned short hack_sub_type,
                          const char* c_class_ip, AutoPunishHackIp* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];

    if (!manager || !occ_date || !c_class_ip || !info) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, hack_type, hack_sub_type, c_class_ip, cnt "
        "FROM auto_punish_hack_ip WHERE occ_date = '%s' AND hack_type = %u "
        "AND hack_sub_type = %u AND c_class_ip = '%s'",
        occ_date, hack_type, hack_sub_type, c_class_ip);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(info, 0, sizeof(AutoPunishHackIp));
    if (row[0]) strncpy(info->occ_date, row[0], sizeof(info->occ_date) - 1);
    info->hack_type = row[1] ? (unsigned short)atoi(row[1]) : 0;
    info->hack_sub_type = row[2] ? (unsigned short)atoi(row[2]) : 0;
    if (row[3]) strncpy(info->c_class_ip, row[3], sizeof(info->c_class_ip) - 1);
    info->cnt = row[4] ? (unsigned int)atol(row[4]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishHackIp_Update(DBConnectionManager* manager, const AutoPunishHackIp* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !info) return -1;

    snprintf(query, sizeof(query),
        "UPDATE auto_punish_hack_ip SET cnt = %u "
        "WHERE occ_date = '%s' AND hack_type = %u AND hack_sub_type = %u AND c_class_ip = '%s'",
        info->cnt, info->occ_date, info->hack_type, info->hack_sub_type, info->c_class_ip);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishHackIp_Delete(DBConnectionManager* manager, const char* occ_date,
                             unsigned short hack_type, unsigned short hack_sub_type,
                             const char* c_class_ip) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_date || !c_class_ip) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM auto_punish_hack_ip WHERE occ_date = '%s' AND hack_type = %u "
        "AND hack_sub_type = %u AND c_class_ip = '%s'",
        occ_date, hack_type, hack_sub_type, c_class_ip);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishHackIp_Exists(DBConnectionManager* manager, const char* occ_date,
                             unsigned short hack_type, unsigned short hack_sub_type,
                             const char* c_class_ip) {
    AutoPunishHackIp info;
    return AutoPunishHackIp_Get(manager, occ_date, hack_type, hack_sub_type, c_class_ip, &info) == 0 ? 1 : 0;
}

int AutoPunishHackIp_GetAll(DBConnectionManager* manager, AutoPunishHackIp* infos,
                             int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, hack_type, hack_sub_type, c_class_ip, cnt "
        "FROM auto_punish_hack_ip ORDER BY occ_date DESC");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(AutoPunishHackIp));
        if (row[0]) strncpy(infos[count].occ_date, row[0], sizeof(infos[count].occ_date) - 1);
        infos[count].hack_type = row[1] ? (unsigned short)atoi(row[1]) : 0;
        infos[count].hack_sub_type = row[2] ? (unsigned short)atoi(row[2]) : 0;
        if (row[3]) strncpy(infos[count].c_class_ip, row[3], sizeof(infos[count].c_class_ip) - 1);
        infos[count].cnt = row[4] ? (unsigned int)atol(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishHackIp_GetByDate(DBConnectionManager* manager, const char* occ_date,
                                AutoPunishHackIp* infos, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !occ_date || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, hack_type, hack_sub_type, c_class_ip, cnt "
        "FROM auto_punish_hack_ip WHERE occ_date = '%s' ORDER BY cnt DESC",
        occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(AutoPunishHackIp));
        if (row[0]) strncpy(infos[count].occ_date, row[0], sizeof(infos[count].occ_date) - 1);
        infos[count].hack_type = row[1] ? (unsigned short)atoi(row[1]) : 0;
        infos[count].hack_sub_type = row[2] ? (unsigned short)atoi(row[2]) : 0;
        if (row[3]) strncpy(infos[count].c_class_ip, row[3], sizeof(infos[count].c_class_ip) - 1);
        infos[count].cnt = row[4] ? (unsigned int)atol(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int AutoPunishHackIp_GetByHackType(DBConnectionManager* manager, unsigned short hack_type,
                                    AutoPunishHackIp* infos, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[5];
    int count = 0;

    if (!manager || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, hack_type, hack_sub_type, c_class_ip, cnt "
        "FROM auto_punish_hack_ip WHERE hack_type = %u ORDER BY occ_date DESC",
        hack_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(AutoPunishHackIp));
        if (row[0]) strncpy(infos[count].occ_date, row[0], sizeof(infos[count].occ_date) - 1);
        infos[count].hack_type = row[1] ? (unsigned short)atoi(row[1]) : 0;
        infos[count].hack_sub_type = row[2] ? (unsigned short)atoi(row[2]) : 0;
        if (row[3]) strncpy(infos[count].c_class_ip, row[3], sizeof(infos[count].c_class_ip) - 1);
        infos[count].cnt = row[4] ? (unsigned int)atol(row[4]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void AutoPunishHackIp_PrintInfo(const AutoPunishHackIp* info) {
    if (!info) return;

    printf("=== AutoPunishHackIp ===\n");
    printf("Occurrence Date: %s\n", info->occ_date);
    printf("Hack Type: %u\n", info->hack_type);
    printf("Hack Sub Type: %u\n", info->hack_sub_type);
    printf("C Class IP: %s\n", info->c_class_ip);
    printf("Count: %u\n", info->cnt);
    printf("========================\n");
}
