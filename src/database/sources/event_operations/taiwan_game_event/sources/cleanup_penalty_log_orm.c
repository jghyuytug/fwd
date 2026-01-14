#include "cleanup_penalty_log_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int CleanupPenaltyLog_Add(DBConnectionManager* manager, const CleanupPenaltyLog* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO cleanup_penalty_log (m_id, charac_no, server_info, occ_date, penalty_point, current_point, reset_cnt, cause, pattern) "
        "VALUES (%d, %d, %d, '%s', %d, %d, %d, %d, %d)",
        record->m_id, record->charac_no, record->server_info, record->occ_date, record->penalty_point, record->current_point, record->reset_cnt, record->cause, record->pattern);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CleanupPenaltyLog_GetAll(DBConnectionManager* manager, CleanupPenaltyLog* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[9];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, charac_no, server_info, occ_date, penalty_point, current_point, reset_cnt, cause, pattern FROM cleanup_penalty_log");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GAME_EVENT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(CleanupPenaltyLog));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].charac_no = row[1] ? atoi(row[1]) : 0;
        records[count].server_info = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(records[count].occ_date, row[3], sizeof(records[count].occ_date) - 1);
        records[count].penalty_point = row[4] ? atoi(row[4]) : 0;
        records[count].current_point = row[5] ? atoi(row[5]) : 0;
        records[count].reset_cnt = row[6] ? atoi(row[6]) : 0;
        records[count].cause = row[7] ? atoi(row[7]) : 0;
        records[count].pattern = row[8] ? atoi(row[8]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
