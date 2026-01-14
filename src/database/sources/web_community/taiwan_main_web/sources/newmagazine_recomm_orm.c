#include "newmagazine_recomm_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int NewmagazineRecomm_Add(DBConnectionManager* manager, const NewmagazineRecomm* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO newmagazine_recomm (m_id, mz_id) "
        "VALUES (%d, %d)",
        record->m_id, record->mz_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int NewmagazineRecomm_Get(DBConnectionManager* manager, int m_id, int mz_id, NewmagazineRecomm* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, mz_id FROM newmagazine_recomm WHERE m_id = %d AND mz_id = %d",
        m_id, mz_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(NewmagazineRecomm));
    record->m_id = row[0] ? atoi(row[0]) : 0;
    record->mz_id = row[1] ? atoi(row[1]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int NewmagazineRecomm_GetAll(DBConnectionManager* manager, NewmagazineRecomm* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, mz_id FROM newmagazine_recomm");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(NewmagazineRecomm));
        records[count].m_id = row[0] ? atoi(row[0]) : 0;
        records[count].mz_id = row[1] ? atoi(row[1]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
