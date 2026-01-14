#include "knowcafe_member_point_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int KnowcafeMemberPoint_Add(DBConnectionManager* manager, const KnowcafeMemberPoint* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO knowcafe_member_point (m_id, occ_time, total_point, now_point) "
        "VALUES (%d, %d, %d, %d)",
        record->m_id, record->occ_time, record->total_point, record->now_point);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int KnowcafeMemberPoint_Get(DBConnectionManager* manager, int m_id, KnowcafeMemberPoint* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, total_point, now_point FROM knowcafe_member_point WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(KnowcafeMemberPoint));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->occ_time = row[1] ? atoi(row[1]) : 0;
    record->total_point = row[2] ? atoi(row[2]) : 0;
    record->now_point = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int KnowcafeMemberPoint_GetAll(DBConnectionManager* manager, KnowcafeMemberPoint* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, total_point, now_point FROM knowcafe_member_point");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(KnowcafeMemberPoint));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].occ_time = row[1] ? atoi(row[1]) : 0;
        records[count].total_point = row[2] ? atoi(row[2]) : 0;
        records[count].now_point = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
