#include "cleanup_user_data_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CleanupUserData_Add(DBConnectionManager* manager, const CleanupUserData* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO cleanup_user_data (m_id, penalty, restriction_cnt, cur_state, user_trade_cnt, npc_trade_cnt, dungeon_clear_cnt, last_clear_map_idx, ghost_clear_cnt, last_penalty_date, other_penalty, other_penalty_date) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', %d, '%s')",
        record->m_id, record->penalty, record->restriction_cnt, record->cur_state, record->user_trade_cnt, record->npc_trade_cnt, record->dungeon_clear_cnt, record->last_clear_map_idx, record->ghost_clear_cnt, record->last_penalty_date, record->other_penalty, record->other_penalty_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CleanupUserData_Get(DBConnectionManager* manager, int m_id, CleanupUserData* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, penalty, restriction_cnt, cur_state, user_trade_cnt, npc_trade_cnt, dungeon_clear_cnt, last_clear_map_idx, ghost_clear_cnt, last_penalty_date, other_penalty, other_penalty_date FROM cleanup_user_data WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(CleanupUserData));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->penalty = row[1] ? atoi(row[1]) : 0;
    record->restriction_cnt = row[2] ? atoi(row[2]) : 0;
    record->cur_state = row[3] ? atoi(row[3]) : 0;
    record->user_trade_cnt = row[4] ? atoi(row[4]) : 0;
    record->npc_trade_cnt = row[5] ? atoi(row[5]) : 0;
    record->dungeon_clear_cnt = row[6] ? atoi(row[6]) : 0;
    record->last_clear_map_idx = row[7] ? atoi(row[7]) : 0;
    record->ghost_clear_cnt = row[8] ? atoi(row[8]) : 0;
    if (row[9]) strncpy(record->last_penalty_date, row[9], sizeof(record->last_penalty_date) - 1);
    record->other_penalty = row[10] ? atoi(row[10]) : 0;
    if (row[11]) strncpy(record->other_penalty_date, row[11], sizeof(record->other_penalty_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int CleanupUserData_GetAll(DBConnectionManager* manager, CleanupUserData* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, penalty, restriction_cnt, cur_state, user_trade_cnt, npc_trade_cnt, dungeon_clear_cnt, last_clear_map_idx, ghost_clear_cnt, last_penalty_date, other_penalty, other_penalty_date FROM cleanup_user_data");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CleanupUserData));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].penalty = row[1] ? atoi(row[1]) : 0;
        records[count].restriction_cnt = row[2] ? atoi(row[2]) : 0;
        records[count].cur_state = row[3] ? atoi(row[3]) : 0;
        records[count].user_trade_cnt = row[4] ? atoi(row[4]) : 0;
        records[count].npc_trade_cnt = row[5] ? atoi(row[5]) : 0;
        records[count].dungeon_clear_cnt = row[6] ? atoi(row[6]) : 0;
        records[count].last_clear_map_idx = row[7] ? atoi(row[7]) : 0;
        records[count].ghost_clear_cnt = row[8] ? atoi(row[8]) : 0;
        if (row[9]) strncpy(records[count].last_penalty_date, row[9], sizeof(records[count].last_penalty_date) - 1);
        records[count].other_penalty = row[10] ? atoi(row[10]) : 0;
        if (row[11]) strncpy(records[count].other_penalty_date, row[11], sizeof(records[count].other_penalty_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
