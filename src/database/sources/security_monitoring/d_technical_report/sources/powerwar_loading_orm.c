#include "powerwar_loading_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int PowerwarLoading_Add(DBConnectionManager* manager, const PowerwarLoading* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO powerwar_loading (m_id, occ_time, round, player, my_loading, other_loading, vs_loading) "
        "VALUES (%d, '%s', %d, %d, %d, %d, %d)",
        record->m_id, record->occ_time, record->round, record->player, record->my_loading, record->other_loading, record->vs_loading);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int PowerwarLoading_Get(DBConnectionManager* manager, int m_id, const char* occ_time, short round, PowerwarLoading* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, round, player, my_loading, other_loading, vs_loading FROM powerwar_loading WHERE m_id = %d AND occ_time = '%s' AND round = %d",
        m_id, occ_time, round);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(PowerwarLoading));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(record->occ_time, row[1], sizeof(record->occ_time) - 1);
    record->round = row[2] ? atoi(row[2]) : 0;
    record->player = row[3] ? atoi(row[3]) : 0;
    record->my_loading = row[4] ? atoi(row[4]) : 0;
    record->other_loading = row[5] ? atoi(row[5]) : 0;
    record->vs_loading = row[6] ? atoi(row[6]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int PowerwarLoading_GetAll(DBConnectionManager* manager, PowerwarLoading* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[7];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, round, player, my_loading, other_loading, vs_loading FROM powerwar_loading");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TECH_REPORT, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(PowerwarLoading));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].occ_time, row[1], sizeof(records[count].occ_time) - 1);
        records[count].round = row[2] ? atoi(row[2]) : 0;
        records[count].player = row[3] ? atoi(row[3]) : 0;
        records[count].my_loading = row[4] ? atoi(row[4]) : 0;
        records[count].other_loading = row[5] ? atoi(row[5]) : 0;
        records[count].vs_loading = row[6] ? atoi(row[6]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
