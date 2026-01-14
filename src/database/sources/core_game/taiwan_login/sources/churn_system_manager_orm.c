#include "churn_system_manager_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int ChurnSystemManager_Add(DBConnectionManager* manager, const ChurnSystemManager* config) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !config) return -1;

    /* no is AUTO_INCREMENT, do not include it in INSERT */
    if (config->reg_time[0]) {
        snprintf(query, sizeof(query),
            "INSERT INTO churn_system_manager "
            "(weekday_var_a, weekday_var_b, weekday_var_c, weekend_var_x, weekend_var_y, "
            "weekend_var_z, next_reward_day, admin_id, reg_time, state_flag) "
            "VALUES (%d, %d, %d, %d, %d, %d, %d, %u, '%s', %u)",
            config->weekday_var_a, config->weekday_var_b, config->weekday_var_c,
            config->weekend_var_x, config->weekend_var_y, config->weekend_var_z,
            config->next_reward_day, config->admin_id, config->reg_time, config->state_flag);
    } else {
        snprintf(query, sizeof(query),
            "INSERT INTO churn_system_manager "
            "(weekday_var_a, weekday_var_b, weekday_var_c, weekend_var_x, weekend_var_y, "
            "weekend_var_z, next_reward_day, admin_id, reg_time, state_flag) "
            "VALUES (%d, %d, %d, %d, %d, %d, %d, %u, NOW(), %u)",
            config->weekday_var_a, config->weekday_var_b, config->weekday_var_c,
            config->weekend_var_x, config->weekend_var_y, config->weekend_var_z,
            config->next_reward_day, config->admin_id, config->state_flag);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int ChurnSystemManager_Get(DBConnectionManager* manager, unsigned int no, ChurnSystemManager* config) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];

    if (!manager || !config) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, weekday_var_a, weekday_var_b, weekday_var_c, weekend_var_x, weekend_var_y, "
        "weekend_var_z, next_reward_day, admin_id, reg_time, state_flag "
        "FROM churn_system_manager WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(config, 0, sizeof(ChurnSystemManager));
    config->no = row[0] ? (unsigned int)atol(row[0]) : 0;
    config->weekday_var_a = row[1] ? atoi(row[1]) : 0;
    config->weekday_var_b = row[2] ? atoi(row[2]) : 0;
    config->weekday_var_c = row[3] ? atoi(row[3]) : 0;
    config->weekend_var_x = row[4] ? atoi(row[4]) : 0;
    config->weekend_var_y = row[5] ? atoi(row[5]) : 0;
    config->weekend_var_z = row[6] ? atoi(row[6]) : 0;
    config->next_reward_day = row[7] ? atoi(row[7]) : 0;
    config->admin_id = row[8] ? (unsigned int)atol(row[8]) : 0;
    if (row[9]) strncpy(config->reg_time, row[9], sizeof(config->reg_time) - 1);
    config->state_flag = row[10] ? (unsigned char)atoi(row[10]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int ChurnSystemManager_Update(DBConnectionManager* manager, const ChurnSystemManager* config) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !config) return -1;

    snprintf(query, sizeof(query),
        "UPDATE churn_system_manager SET "
        "weekday_var_a = %d, weekday_var_b = %d, weekday_var_c = %d, "
        "weekend_var_x = %d, weekend_var_y = %d, weekend_var_z = %d, "
        "next_reward_day = %d, admin_id = %u, state_flag = %u "
        "WHERE no = %u",
        config->weekday_var_a, config->weekday_var_b, config->weekday_var_c,
        config->weekend_var_x, config->weekend_var_y, config->weekend_var_z,
        config->next_reward_day, config->admin_id, config->state_flag, config->no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int ChurnSystemManager_Delete(DBConnectionManager* manager, unsigned int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM churn_system_manager WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int ChurnSystemManager_Exists(DBConnectionManager* manager, unsigned int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM churn_system_manager WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    int count = row[0] ? atoi(row[0]) : 0;
    DBQueryResult_Free(&result);
    return count > 0;
}

int ChurnSystemManager_GetActive(DBConnectionManager* manager, ChurnSystemManager* configs,
                                  int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !configs || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, weekday_var_a, weekday_var_b, weekday_var_c, weekend_var_x, weekend_var_y, "
        "weekend_var_z, next_reward_day, admin_id, reg_time, state_flag "
        "FROM churn_system_manager WHERE state_flag = 1 ORDER BY no DESC");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&configs[count], 0, sizeof(ChurnSystemManager));
        configs[count].no = row[0] ? (unsigned int)atol(row[0]) : 0;
        configs[count].weekday_var_a = row[1] ? atoi(row[1]) : 0;
        configs[count].weekday_var_b = row[2] ? atoi(row[2]) : 0;
        configs[count].weekday_var_c = row[3] ? atoi(row[3]) : 0;
        configs[count].weekend_var_x = row[4] ? atoi(row[4]) : 0;
        configs[count].weekend_var_y = row[5] ? atoi(row[5]) : 0;
        configs[count].weekend_var_z = row[6] ? atoi(row[6]) : 0;
        configs[count].next_reward_day = row[7] ? atoi(row[7]) : 0;
        configs[count].admin_id = row[8] ? (unsigned int)atol(row[8]) : 0;
        if (row[9]) strncpy(configs[count].reg_time, row[9], sizeof(configs[count].reg_time) - 1);
        configs[count].state_flag = row[10] ? (unsigned char)atoi(row[10]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int ChurnSystemManager_GetByAdmin(DBConnectionManager* manager, unsigned int admin_id,
                                   ChurnSystemManager* configs, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !configs || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, weekday_var_a, weekday_var_b, weekday_var_c, weekend_var_x, weekend_var_y, "
        "weekend_var_z, next_reward_day, admin_id, reg_time, state_flag "
        "FROM churn_system_manager WHERE admin_id = %u ORDER BY no DESC", admin_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&configs[count], 0, sizeof(ChurnSystemManager));
        configs[count].no = row[0] ? (unsigned int)atol(row[0]) : 0;
        configs[count].weekday_var_a = row[1] ? atoi(row[1]) : 0;
        configs[count].weekday_var_b = row[2] ? atoi(row[2]) : 0;
        configs[count].weekday_var_c = row[3] ? atoi(row[3]) : 0;
        configs[count].weekend_var_x = row[4] ? atoi(row[4]) : 0;
        configs[count].weekend_var_y = row[5] ? atoi(row[5]) : 0;
        configs[count].weekend_var_z = row[6] ? atoi(row[6]) : 0;
        configs[count].next_reward_day = row[7] ? atoi(row[7]) : 0;
        configs[count].admin_id = row[8] ? (unsigned int)atol(row[8]) : 0;
        if (row[9]) strncpy(configs[count].reg_time, row[9], sizeof(configs[count].reg_time) - 1);
        configs[count].state_flag = row[10] ? (unsigned char)atoi(row[10]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int ChurnSystemManager_GetLatest(DBConnectionManager* manager, ChurnSystemManager* config) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];

    if (!manager || !config) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, weekday_var_a, weekday_var_b, weekday_var_c, weekend_var_x, weekend_var_y, "
        "weekend_var_z, next_reward_day, admin_id, reg_time, state_flag "
        "FROM churn_system_manager ORDER BY no DESC LIMIT 1");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(config, 0, sizeof(ChurnSystemManager));
    config->no = row[0] ? (unsigned int)atol(row[0]) : 0;
    config->weekday_var_a = row[1] ? atoi(row[1]) : 0;
    config->weekday_var_b = row[2] ? atoi(row[2]) : 0;
    config->weekday_var_c = row[3] ? atoi(row[3]) : 0;
    config->weekend_var_x = row[4] ? atoi(row[4]) : 0;
    config->weekend_var_y = row[5] ? atoi(row[5]) : 0;
    config->weekend_var_z = row[6] ? atoi(row[6]) : 0;
    config->next_reward_day = row[7] ? atoi(row[7]) : 0;
    config->admin_id = row[8] ? (unsigned int)atol(row[8]) : 0;
    if (row[9]) strncpy(config->reg_time, row[9], sizeof(config->reg_time) - 1);
    config->state_flag = row[10] ? (unsigned char)atoi(row[10]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

void ChurnSystemManager_PrintInfo(const ChurnSystemManager* config) {
    if (!config) return;

    printf("=== Churn System Manager ===\n");
    printf("No: %u\n", config->no);
    printf("Weekday (A, B, C): %d, %d, %d\n",
           config->weekday_var_a, config->weekday_var_b, config->weekday_var_c);
    printf("Weekend (X, Y, Z): %d, %d, %d\n",
           config->weekend_var_x, config->weekend_var_y, config->weekend_var_z);
    printf("Next Reward Day: %d\n", config->next_reward_day);
    printf("Admin ID: %u\n", config->admin_id);
    printf("Reg Time: %s\n", config->reg_time);
    printf("State Flag: %u\n", config->state_flag);
    printf("============================\n");
}
