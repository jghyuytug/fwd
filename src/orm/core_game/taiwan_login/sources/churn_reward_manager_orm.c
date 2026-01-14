#include "churn_reward_manager_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int ChurnRewardManager_Add(DBConnectionManager* manager, const ChurnRewardManager* reward) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !reward) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO churn_reward_manager "
        "(min_day, max_day, min_val, max_val, item_id, add_info, luck_point, quest_id) "
        "VALUES (%u, %u, %u, %u, %u, %u, %u, %u)",
        reward->min_day, reward->max_day, reward->min_val, reward->max_val,
        reward->item_id, reward->add_info, reward->luck_point, reward->quest_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int ChurnRewardManager_Get(DBConnectionManager* manager, unsigned char min_day, unsigned char max_day,
                            unsigned int min_val, unsigned int max_val, unsigned char quest_id,
                            ChurnRewardManager* reward) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !reward) return -1;

    snprintf(query, sizeof(query),
        "SELECT min_day, max_day, min_val, max_val, item_id, add_info, luck_point, quest_id "
        "FROM churn_reward_manager WHERE min_day = %u AND max_day = %u AND min_val = %u "
        "AND max_val = %u AND quest_id = %u",
        min_day, max_day, min_val, max_val, quest_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(reward, 0, sizeof(ChurnRewardManager));
    reward->min_day = row[0] ? (unsigned char)atoi(row[0]) : 0;
    reward->max_day = row[1] ? (unsigned char)atoi(row[1]) : 0;
    reward->min_val = row[2] ? (unsigned int)atol(row[2]) : 0;
    reward->max_val = row[3] ? (unsigned int)atol(row[3]) : 0;
    reward->item_id = row[4] ? (unsigned int)atol(row[4]) : 0;
    reward->add_info = row[5] ? (unsigned int)atol(row[5]) : 0;
    reward->luck_point = row[6] ? (unsigned int)atol(row[6]) : 0;
    reward->quest_id = row[7] ? (unsigned char)atoi(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int ChurnRewardManager_Delete(DBConnectionManager* manager, unsigned char min_day, unsigned char max_day,
                               unsigned int min_val, unsigned int max_val, unsigned char quest_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM churn_reward_manager WHERE min_day = %u AND max_day = %u AND min_val = %u "
        "AND max_val = %u AND quest_id = %u",
        min_day, max_day, min_val, max_val, quest_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int ChurnRewardManager_Exists(DBConnectionManager* manager, unsigned char min_day, unsigned char max_day,
                               unsigned int min_val, unsigned int max_val, unsigned char quest_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM churn_reward_manager WHERE min_day = %u AND max_day = %u AND min_val = %u "
        "AND max_val = %u AND quest_id = %u",
        min_day, max_day, min_val, max_val, quest_id);

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

int ChurnRewardManager_GetByDayRange(DBConnectionManager* manager, unsigned char min_day, unsigned char max_day,
                                      ChurnRewardManager* rewards, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !rewards || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT min_day, max_day, min_val, max_val, item_id, add_info, luck_point, quest_id "
        "FROM churn_reward_manager WHERE min_day = %u AND max_day = %u "
        "ORDER BY min_val, max_val",
        min_day, max_day);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&rewards[count], 0, sizeof(ChurnRewardManager));
        rewards[count].min_day = row[0] ? (unsigned char)atoi(row[0]) : 0;
        rewards[count].max_day = row[1] ? (unsigned char)atoi(row[1]) : 0;
        rewards[count].min_val = row[2] ? (unsigned int)atol(row[2]) : 0;
        rewards[count].max_val = row[3] ? (unsigned int)atol(row[3]) : 0;
        rewards[count].item_id = row[4] ? (unsigned int)atol(row[4]) : 0;
        rewards[count].add_info = row[5] ? (unsigned int)atol(row[5]) : 0;
        rewards[count].luck_point = row[6] ? (unsigned int)atol(row[6]) : 0;
        rewards[count].quest_id = row[7] ? (unsigned char)atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int ChurnRewardManager_GetByQuest(DBConnectionManager* manager, unsigned char quest_id,
                                   ChurnRewardManager* rewards, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !rewards || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT min_day, max_day, min_val, max_val, item_id, add_info, luck_point, quest_id "
        "FROM churn_reward_manager WHERE quest_id = %u ORDER BY min_day, max_day",
        quest_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&rewards[count], 0, sizeof(ChurnRewardManager));
        rewards[count].min_day = row[0] ? (unsigned char)atoi(row[0]) : 0;
        rewards[count].max_day = row[1] ? (unsigned char)atoi(row[1]) : 0;
        rewards[count].min_val = row[2] ? (unsigned int)atol(row[2]) : 0;
        rewards[count].max_val = row[3] ? (unsigned int)atol(row[3]) : 0;
        rewards[count].item_id = row[4] ? (unsigned int)atol(row[4]) : 0;
        rewards[count].add_info = row[5] ? (unsigned int)atol(row[5]) : 0;
        rewards[count].luck_point = row[6] ? (unsigned int)atol(row[6]) : 0;
        rewards[count].quest_id = row[7] ? (unsigned char)atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int ChurnRewardManager_GetAll(DBConnectionManager* manager, ChurnRewardManager* rewards,
                               int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !rewards || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT min_day, max_day, min_val, max_val, item_id, add_info, luck_point, quest_id "
        "FROM churn_reward_manager ORDER BY quest_id, min_day, max_day");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&rewards[count], 0, sizeof(ChurnRewardManager));
        rewards[count].min_day = row[0] ? (unsigned char)atoi(row[0]) : 0;
        rewards[count].max_day = row[1] ? (unsigned char)atoi(row[1]) : 0;
        rewards[count].min_val = row[2] ? (unsigned int)atol(row[2]) : 0;
        rewards[count].max_val = row[3] ? (unsigned int)atol(row[3]) : 0;
        rewards[count].item_id = row[4] ? (unsigned int)atol(row[4]) : 0;
        rewards[count].add_info = row[5] ? (unsigned int)atol(row[5]) : 0;
        rewards[count].luck_point = row[6] ? (unsigned int)atol(row[6]) : 0;
        rewards[count].quest_id = row[7] ? (unsigned char)atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void ChurnRewardManager_PrintInfo(const ChurnRewardManager* reward) {
    if (!reward) return;

    printf("=== Churn Reward Manager ===\n");
    printf("Day Range: %u - %u\n", reward->min_day, reward->max_day);
    printf("Value Range: %u - %u\n", reward->min_val, reward->max_val);
    printf("Quest ID: %u\n", reward->quest_id);
    printf("Item ID: %u\n", reward->item_id);
    printf("Add Info: %u\n", reward->add_info);
    printf("Luck Point: %u\n", reward->luck_point);
    printf("============================\n");
}
