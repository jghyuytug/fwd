#include "hack_cleanpad_ratio_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int HackCleanpadRatioInfo_Add(DBConnectionManager* manager, const HackCleanpadRatioInfo* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !info) return -1;

    if (info->reg_date[0]) {
        snprintf(query, sizeof(query),
            "INSERT INTO hack_cleanpad_ratio_info (hack_type, value, reg_date) "
            "VALUES (%u, %u, '%s')",
            info->hack_type, info->value, info->reg_date);
    } else {
        snprintf(query, sizeof(query),
            "INSERT INTO hack_cleanpad_ratio_info (hack_type, value, reg_date) "
            "VALUES (%u, %u, NOW())",
            info->hack_type, info->value);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int HackCleanpadRatioInfo_Get(DBConnectionManager* manager, unsigned short hack_type, HackCleanpadRatioInfo* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !info) return -1;

    snprintf(query, sizeof(query),
        "SELECT hack_type, value, reg_date FROM hack_cleanpad_ratio_info WHERE hack_type = %u",
        hack_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(info, 0, sizeof(HackCleanpadRatioInfo));
    info->hack_type = row[0] ? (unsigned short)atoi(row[0]) : 0;
    info->value = row[1] ? (unsigned int)strtoul(row[1], NULL, 10) : 0;
    if (row[2]) strncpy(info->reg_date, row[2], sizeof(info->reg_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int HackCleanpadRatioInfo_Update(DBConnectionManager* manager, const HackCleanpadRatioInfo* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !info) return -1;

    if (info->reg_date[0]) {
        snprintf(query, sizeof(query),
            "UPDATE hack_cleanpad_ratio_info SET value = %u, reg_date = '%s' WHERE hack_type = %u",
            info->value, info->reg_date, info->hack_type);
    } else {
        snprintf(query, sizeof(query),
            "UPDATE hack_cleanpad_ratio_info SET value = %u, reg_date = NOW() WHERE hack_type = %u",
            info->value, info->hack_type);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int HackCleanpadRatioInfo_Delete(DBConnectionManager* manager, unsigned short hack_type) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM hack_cleanpad_ratio_info WHERE hack_type = %u", hack_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int HackCleanpadRatioInfo_Exists(DBConnectionManager* manager, unsigned short hack_type) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM hack_cleanpad_ratio_info WHERE hack_type = %u", hack_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0 && row[0]) {
        int count = atoi(row[0]);
        DBQueryResult_Free(&result);
        return count > 0;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int HackCleanpadRatioInfo_GetAll(DBConnectionManager* manager,
                                  HackCleanpadRatioInfo* infos,
                                  int max_count,
                                  int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT hack_type, value, reg_date FROM hack_cleanpad_ratio_info ORDER BY hack_type");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(HackCleanpadRatioInfo));
        infos[count].hack_type = row[0] ? (unsigned short)atoi(row[0]) : 0;
        infos[count].value = row[1] ? (unsigned int)strtoul(row[1], NULL, 10) : 0;
        if (row[2]) strncpy(infos[count].reg_date, row[2], sizeof(infos[count].reg_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int HackCleanpadRatioInfo_GetCount(DBConnectionManager* manager, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !count) return -1;

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM hack_cleanpad_ratio_info");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) > 0 && row[0]) {
        *count = atoi(row[0]);
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

void HackCleanpadRatioInfo_PrintInfo(const HackCleanpadRatioInfo* info) {
    if (!info) return;

    printf("Hack Cleanpad Ratio Info:\n");
    printf("  Hack Type: %u\n", info->hack_type);
    printf("  Value: %u\n", info->value);
    printf("  Registration Date: %s\n", info->reg_date);
}
