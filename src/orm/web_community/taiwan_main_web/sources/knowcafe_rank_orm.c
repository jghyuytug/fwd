#include "knowcafe_rank_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int KnowcafeRank_Add(DBConnectionManager* manager, const KnowcafeRank* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO knowcafe_rank (knowcafe_type, rank, m_id, total_point, now_point, old_rank, server_id, charac_no, charac_name, job, black) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d, '%s', %d, %d)",
        record->knowcafe_type, record->rank, record->m_id, record->total_point, record->now_point, record->old_rank, record->server_id, record->charac_no, record->charac_name, record->job, record->black);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int KnowcafeRank_GetAll(DBConnectionManager* manager, KnowcafeRank* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[11];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT knowcafe_type, rank, m_id, total_point, now_point, old_rank, server_id, charac_no, charac_name, job, black FROM knowcafe_rank");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(KnowcafeRank));
        records[count].knowcafe_type = row[0] ? atoi(row[0]) : 0;
        records[count].rank = row[1] ? atoi(row[1]) : 0;
        records[count].m_id = row[2] ? atoi(row[2]) : 0;
        records[count].total_point = row[3] ? atoi(row[3]) : 0;
        records[count].now_point = row[4] ? atoi(row[4]) : 0;
        records[count].old_rank = row[5] ? atoi(row[5]) : 0;
        records[count].server_id = row[6] ? atoi(row[6]) : 0;
        records[count].charac_no = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].charac_name, row[8], sizeof(records[count].charac_name) - 1);
        records[count].job = row[9] ? atoi(row[9]) : 0;
        records[count].black = row[10] ? atoi(row[10]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
