#include "powerwar_lag_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int PowerwarLag_Add(DBConnectionManager* manager, const PowerwarLag* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO powerwar_lag (m_id, occ_time, round, player, lag_avg, lag_cnt) "
        "VALUES (%d, '%s', %d, %d, %f, %f)",
        record->m_id, record->occ_time, record->round, record->player, record->lag_avg, record->lag_cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PowerwarLag_Get(DBConnectionManager* manager, int m_id, const char* occ_time, short round, PowerwarLag* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, round, player, lag_avg, lag_cnt FROM powerwar_lag WHERE m_id = %d AND occ_time = '%s' AND round = %d",
        m_id, occ_time, round);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(PowerwarLag));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->occ_time, row[1], sizeof(record->occ_time) - 1);
    record->round = row[2] ? atoi(row[2]) : 0;
    record->player = row[3] ? atoi(row[3]) : 0;
    record->lag_avg = row[4] ? atof(row[4]) : 0;
    record->lag_cnt = row[5] ? atof(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int PowerwarLag_GetAll(DBConnectionManager* manager, PowerwarLag* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, round, player, lag_avg, lag_cnt FROM powerwar_lag");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(PowerwarLag));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].occ_time, row[1], sizeof(records[count].occ_time) - 1);
        records[count].round = row[2] ? atoi(row[2]) : 0;
        records[count].player = row[3] ? atoi(row[3]) : 0;
        records[count].lag_avg = row[4] ? atof(row[4]) : 0;
        records[count].lag_cnt = row[5] ? atof(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
