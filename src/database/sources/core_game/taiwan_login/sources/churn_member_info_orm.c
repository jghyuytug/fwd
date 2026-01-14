#include "churn_member_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* Add a new churn member info record */
int ChurnMemberInfo_Add(DBConnectionManager* manager, const ChurnMemberInfo* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !info) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO churn_member_info "
        "(m_id, accrue_cera, play_info, first_reward_time, last_reward_time, "
        "server_id, charac_no, item_id, add_info, luck_point, last_update_time, "
        "second_reward_time, quest_time) "
        "VALUES (%u, %u, '%s', %u, %u, %u, %u, %u, %u, %u, %u, %u, %u)",
        info->m_id, info->accrue_cera, info->play_info, info->first_reward_time,
        info->last_reward_time, info->server_id, info->charac_no, info->item_id,
        info->add_info, info->luck_point, info->last_update_time,
        info->second_reward_time, info->quest_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Get churn member info by member ID */
int ChurnMemberInfo_Get(DBConnectionManager* manager, unsigned int m_id, ChurnMemberInfo* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];

    if (!manager || !info) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, accrue_cera, play_info, first_reward_time, last_reward_time, "
        "server_id, charac_no, item_id, add_info, luck_point, last_update_time, "
        "second_reward_time, quest_time "
        "FROM churn_member_info WHERE m_id = %u", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(info, 0, sizeof(ChurnMemberInfo));
    info->m_id = row[0] ? (unsigned int)atol(row[0]) : 0;
    info->accrue_cera = row[1] ? (unsigned int)atol(row[1]) : 0;
    if (row[2]) strncpy(info->play_info, row[2], sizeof(info->play_info) - 1);
    info->first_reward_time = row[3] ? (unsigned int)atol(row[3]) : 0;
    info->last_reward_time = row[4] ? (unsigned int)atol(row[4]) : 0;
    info->server_id = row[5] ? (unsigned char)atoi(row[5]) : 0;
    info->charac_no = row[6] ? (unsigned int)atol(row[6]) : 0;
    info->item_id = row[7] ? (unsigned int)atol(row[7]) : 0;
    info->add_info = row[8] ? (unsigned char)atoi(row[8]) : 0;
    info->luck_point = row[9] ? (unsigned int)atol(row[9]) : 0;
    info->last_update_time = row[10] ? (unsigned int)atol(row[10]) : 0;
    info->second_reward_time = row[11] ? (unsigned int)atol(row[11]) : 0;
    info->quest_time = row[12] ? (unsigned int)atol(row[12]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

/* Update churn member info */
int ChurnMemberInfo_Update(DBConnectionManager* manager, const ChurnMemberInfo* info) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !info) return -1;

    snprintf(query, sizeof(query),
        "UPDATE churn_member_info SET "
        "accrue_cera = %u, play_info = '%s', first_reward_time = %u, "
        "last_reward_time = %u, server_id = %u, charac_no = %u, item_id = %u, "
        "add_info = %u, luck_point = %u, last_update_time = %u, "
        "second_reward_time = %u, quest_time = %u "
        "WHERE m_id = %u",
        info->accrue_cera, info->play_info, info->first_reward_time,
        info->last_reward_time, info->server_id, info->charac_no, info->item_id,
        info->add_info, info->luck_point, info->last_update_time,
        info->second_reward_time, info->quest_time, info->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Delete churn member info */
int ChurnMemberInfo_Delete(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM churn_member_info WHERE m_id = %u", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Check if churn member info exists */
int ChurnMemberInfo_Exists(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM churn_member_info WHERE m_id = %u", m_id);

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

/* Get churn members by server ID */
int ChurnMemberInfo_GetByServer(DBConnectionManager* manager, unsigned char server_id,
                                 ChurnMemberInfo* infos, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, accrue_cera, play_info, first_reward_time, last_reward_time, "
        "server_id, charac_no, item_id, add_info, luck_point, last_update_time, "
        "second_reward_time, quest_time "
        "FROM churn_member_info WHERE server_id = %u ORDER BY m_id",
        server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(ChurnMemberInfo));
        infos[count].m_id = row[0] ? (unsigned int)atol(row[0]) : 0;
        infos[count].accrue_cera = row[1] ? (unsigned int)atol(row[1]) : 0;
        if (row[2]) strncpy(infos[count].play_info, row[2], sizeof(infos[count].play_info) - 1);
        infos[count].first_reward_time = row[3] ? (unsigned int)atol(row[3]) : 0;
        infos[count].last_reward_time = row[4] ? (unsigned int)atol(row[4]) : 0;
        infos[count].server_id = row[5] ? (unsigned char)atoi(row[5]) : 0;
        infos[count].charac_no = row[6] ? (unsigned int)atol(row[6]) : 0;
        infos[count].item_id = row[7] ? (unsigned int)atol(row[7]) : 0;
        infos[count].add_info = row[8] ? (unsigned char)atoi(row[8]) : 0;
        infos[count].luck_point = row[9] ? (unsigned int)atol(row[9]) : 0;
        infos[count].last_update_time = row[10] ? (unsigned int)atol(row[10]) : 0;
        infos[count].second_reward_time = row[11] ? (unsigned int)atol(row[11]) : 0;
        infos[count].quest_time = row[12] ? (unsigned int)atol(row[12]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Get active members since given time */
int ChurnMemberInfo_GetActiveMembers(DBConnectionManager* manager, unsigned int since_time,
                                      ChurnMemberInfo* infos, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[13];
    int count = 0;

    if (!manager || !infos || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, accrue_cera, play_info, first_reward_time, last_reward_time, "
        "server_id, charac_no, item_id, add_info, luck_point, last_update_time, "
        "second_reward_time, quest_time "
        "FROM churn_member_info WHERE last_update_time >= %u "
        "ORDER BY last_update_time DESC",
        since_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&infos[count], 0, sizeof(ChurnMemberInfo));
        infos[count].m_id = row[0] ? (unsigned int)atol(row[0]) : 0;
        infos[count].accrue_cera = row[1] ? (unsigned int)atol(row[1]) : 0;
        if (row[2]) strncpy(infos[count].play_info, row[2], sizeof(infos[count].play_info) - 1);
        infos[count].first_reward_time = row[3] ? (unsigned int)atol(row[3]) : 0;
        infos[count].last_reward_time = row[4] ? (unsigned int)atol(row[4]) : 0;
        infos[count].server_id = row[5] ? (unsigned char)atoi(row[5]) : 0;
        infos[count].charac_no = row[6] ? (unsigned int)atol(row[6]) : 0;
        infos[count].item_id = row[7] ? (unsigned int)atol(row[7]) : 0;
        infos[count].add_info = row[8] ? (unsigned char)atoi(row[8]) : 0;
        infos[count].luck_point = row[9] ? (unsigned int)atol(row[9]) : 0;
        infos[count].last_update_time = row[10] ? (unsigned int)atol(row[10]) : 0;
        infos[count].second_reward_time = row[11] ? (unsigned int)atol(row[11]) : 0;
        infos[count].quest_time = row[12] ? (unsigned int)atol(row[12]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

/* Update reward time for a member */
int ChurnMemberInfo_UpdateRewardTime(DBConnectionManager* manager, unsigned int m_id,
                                      unsigned int reward_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE churn_member_info SET last_reward_time = %u, last_update_time = %u "
        "WHERE m_id = %u",
        reward_time, reward_time, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Print churn member info details */
void ChurnMemberInfo_PrintInfo(const ChurnMemberInfo* info) {
    if (!info) return;

    printf("=== Churn Member Info ===\n");
    printf("Member ID: %u\n", info->m_id);
    printf("Accrue CERA: %u\n", info->accrue_cera);
    printf("Play Info: %s\n", info->play_info);
    printf("First Reward: %u\n", info->first_reward_time);
    printf("Last Reward: %u\n", info->last_reward_time);
    printf("Server ID: %u\n", info->server_id);
    printf("Character No: %u\n", info->charac_no);
    printf("Item ID: %u\n", info->item_id);
    printf("Add Info: %u\n", info->add_info);
    printf("Luck Point: %u\n", info->luck_point);
    printf("Last Update: %u\n", info->last_update_time);
    printf("Second Reward: %u\n", info->second_reward_time);
    printf("Quest Time: %u\n", info->quest_time);
    printf("=========================\n");
}
