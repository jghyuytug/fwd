#include "knowcafe_know_lev_stats_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int KnowcafeKnowLevStats_Add(DBConnectionManager* manager, const KnowcafeKnowLevStats* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO knowcafe_know_lev_stats (date_enroll, lev, m_cnt) "
        "VALUES ('%s', %d, %d)",
        record->date_enroll, record->lev, record->m_cnt);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int KnowcafeKnowLevStats_Get(DBConnectionManager* manager, const char* date_enroll, signed char lev, KnowcafeKnowLevStats* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT date_enroll, lev, m_cnt FROM knowcafe_know_lev_stats WHERE date_enroll = '%s' AND lev = %d",
        date_enroll, lev);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(KnowcafeKnowLevStats));
    if (row[0]) strncpy(record->date_enroll, row[0], sizeof(record->date_enroll) - 1);
    record->lev = row[1] ? atoi(row[1]) : 0;
    record->m_cnt = row[2] ? atoi(row[2]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int KnowcafeKnowLevStats_GetAll(DBConnectionManager* manager, KnowcafeKnowLevStats* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT date_enroll, lev, m_cnt FROM knowcafe_know_lev_stats");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(KnowcafeKnowLevStats));
        if (row[0]) strncpy(records[count].date_enroll, row[0], sizeof(records[count].date_enroll) - 1);
        records[count].lev = row[1] ? atoi(row[1]) : 0;
        records[count].m_cnt = row[2] ? atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
