#include "churn_reward_history_201507_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int ChurnRewardHistory201507_Add(DBConnectionManager* manager, const ChurnRewardHistory201507* history) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !history) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO churn_reward_history_201507 "
        "(m_id, occ_time, server_id, charac_no, item_id, add_info, luck_point, reward_order, cera) "
        "VALUES (%u, '%s', %u, %u, %u, %u, %u, %u, %u)",
        history->m_id, history->occ_time, history->server_id, history->charac_no,
        history->item_id, history->add_info, history->luck_point,
        history->reward_order, history->cera);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int ChurnRewardHistory201507_Get(DBConnectionManager* manager, unsigned int m_id,
                                  const char* occ_time, ChurnRewardHistory201507* history) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];

    if (!manager || !occ_time || !history) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, server_id, charac_no, item_id, add_info, luck_point, reward_order, cera "
        "FROM churn_reward_history_201507 WHERE m_id = %u AND occ_time = '%s'",
        m_id, occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(history, 0, sizeof(ChurnRewardHistory201507));
    history->m_id = row[0] ? (unsigned int)atol(row[0]) : 0;
    if (row[1]) strncpy(history->occ_time, row[1], sizeof(history->occ_time) - 1);
    history->server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
    history->charac_no = row[3] ? (unsigned int)atol(row[3]) : 0;
    history->item_id = row[4] ? (unsigned int)atol(row[4]) : 0;
    history->add_info = row[5] ? (unsigned int)atol(row[5]) : 0;
    history->luck_point = row[6] ? (unsigned int)atol(row[6]) : 0;
    history->reward_order = row[7] ? (unsigned int)atol(row[7]) : 0;
    history->cera = row[8] ? (unsigned int)atol(row[8]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int ChurnRewardHistory201507_Delete(DBConnectionManager* manager, unsigned int m_id, const char* occ_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_time) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM churn_reward_history_201507 WHERE m_id = %u AND occ_time = '%s'",
        m_id, occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int ChurnRewardHistory201507_Exists(DBConnectionManager* manager, unsigned int m_id, const char* occ_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager || !occ_time) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM churn_reward_history_201507 WHERE m_id = %u AND occ_time = '%s'",
        m_id, occ_time);

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

int ChurnRewardHistory201507_GetByMember(DBConnectionManager* manager, unsigned int m_id,
                                          ChurnRewardHistory201507* histories, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !histories || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, server_id, charac_no, item_id, add_info, luck_point, reward_order, cera "
        "FROM churn_reward_history_201507 WHERE m_id = %u ORDER BY occ_time DESC", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&histories[count], 0, sizeof(ChurnRewardHistory201507));
        histories[count].m_id = row[0] ? (unsigned int)atol(row[0]) : 0;
        if (row[1]) strncpy(histories[count].occ_time, row[1], sizeof(histories[count].occ_time) - 1);
        histories[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        histories[count].charac_no = row[3] ? (unsigned int)atol(row[3]) : 0;
        histories[count].item_id = row[4] ? (unsigned int)atol(row[4]) : 0;
        histories[count].add_info = row[5] ? (unsigned int)atol(row[5]) : 0;
        histories[count].luck_point = row[6] ? (unsigned int)atol(row[6]) : 0;
        histories[count].reward_order = row[7] ? (unsigned int)atol(row[7]) : 0;
        histories[count].cera = row[8] ? (unsigned int)atol(row[8]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int ChurnRewardHistory201507_GetByTimeRange(DBConnectionManager* manager, const char* start_time,
                                              const char* end_time, ChurnRewardHistory201507* histories,
                                              int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !start_time || !end_time || !histories || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, server_id, charac_no, item_id, add_info, luck_point, reward_order, cera "
        "FROM churn_reward_history_201507 WHERE occ_time >= '%s' AND occ_time <= '%s' "
        "ORDER BY occ_time DESC",
        start_time, end_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&histories[count], 0, sizeof(ChurnRewardHistory201507));
        histories[count].m_id = row[0] ? (unsigned int)atol(row[0]) : 0;
        if (row[1]) strncpy(histories[count].occ_time, row[1], sizeof(histories[count].occ_time) - 1);
        histories[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        histories[count].charac_no = row[3] ? (unsigned int)atol(row[3]) : 0;
        histories[count].item_id = row[4] ? (unsigned int)atol(row[4]) : 0;
        histories[count].add_info = row[5] ? (unsigned int)atol(row[5]) : 0;
        histories[count].luck_point = row[6] ? (unsigned int)atol(row[6]) : 0;
        histories[count].reward_order = row[7] ? (unsigned int)atol(row[7]) : 0;
        histories[count].cera = row[8] ? (unsigned int)atol(row[8]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void ChurnRewardHistory201507_PrintInfo(const ChurnRewardHistory201507* history) {
    if (!history) return;

    printf("=== Churn Reward History 201507 ===\n");
    printf("Member ID: %u\n", history->m_id);
    printf("Occurrence Time: %s\n", history->occ_time);
    printf("Server ID: %u\n", history->server_id);
    printf("Character No: %u\n", history->charac_no);
    printf("Item ID: %u\n", history->item_id);
    printf("Add Info: %u\n", history->add_info);
    printf("Luck Point: %u\n", history->luck_point);
    printf("Reward Order: %u\n", history->reward_order);
    printf("CERA: %u\n", history->cera);
    printf("====================================\n");
}
