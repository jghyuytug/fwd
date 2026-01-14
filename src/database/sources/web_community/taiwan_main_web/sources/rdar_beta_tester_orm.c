#include "rdar_beta_tester_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int RdarBetaTester_Add(DBConnectionManager* manager, const RdarBetaTester* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO rdar_beta_tester (m_id, m_user_id, email, req_date, pc_spec, line_code, job_code, genre_code, fav_game, req_reason, winning_yn, times) "
        "VALUES (%d, '%s', '%s', %d, '%s', %d, %d, %d, '%s', '%s', %d, %d)",
        record->m_id, record->m_user_id, record->email, record->req_date, record->pc_spec, record->line_code, record->job_code, record->genre_code, record->fav_game, record->req_reason, record->winning_yn, record->times);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int RdarBetaTester_Get(DBConnectionManager* manager, int m_id, RdarBetaTester* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, m_user_id, email, req_date, pc_spec, line_code, job_code, genre_code, fav_game, req_reason, winning_yn, times FROM rdar_beta_tester WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(RdarBetaTester));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->m_user_id, row[1], sizeof(record->m_user_id) - 1);
    if (row[2]) strncpy(record->email, row[2], sizeof(record->email) - 1);
    record->req_date = row[3] ? atoi(row[3]) : 0;
    if (row[4]) strncpy(record->pc_spec, row[4], sizeof(record->pc_spec) - 1);
    record->line_code = row[5] ? atoi(row[5]) : 0;
    record->job_code = row[6] ? atoi(row[6]) : 0;
    record->genre_code = row[7] ? atoi(row[7]) : 0;
    if (row[8]) strncpy(record->fav_game, row[8], sizeof(record->fav_game) - 1);
    if (row[9]) strncpy(record->req_reason, row[9], sizeof(record->req_reason) - 1);
    record->winning_yn = row[10] ? atoi(row[10]) : 0;
    record->times = row[11] ? atoi(row[11]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int RdarBetaTester_GetAll(DBConnectionManager* manager, RdarBetaTester* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[12];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, m_user_id, email, req_date, pc_spec, line_code, job_code, genre_code, fav_game, req_reason, winning_yn, times FROM rdar_beta_tester");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(RdarBetaTester));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].m_user_id, row[1], sizeof(records[count].m_user_id) - 1);
        if (row[2]) strncpy(records[count].email, row[2], sizeof(records[count].email) - 1);
        records[count].req_date = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].pc_spec, row[4], sizeof(records[count].pc_spec) - 1);
        records[count].line_code = row[5] ? atoi(row[5]) : 0;
        records[count].job_code = row[6] ? atoi(row[6]) : 0;
        records[count].genre_code = row[7] ? atoi(row[7]) : 0;
        if (row[8]) strncpy(records[count].fav_game, row[8], sizeof(records[count].fav_game) - 1);
        if (row[9]) strncpy(records[count].req_reason, row[9], sizeof(records[count].req_reason) - 1);
        records[count].winning_yn = row[10] ? atoi(row[10]) : 0;
        records[count].times = row[11] ? atoi(row[11]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
