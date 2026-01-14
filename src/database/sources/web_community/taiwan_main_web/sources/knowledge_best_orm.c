#include "knowledge_best_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int KnowledgeBest_Add(DBConnectionManager* manager, const KnowledgeBest* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO knowledge_best (kn_id, rank, content_no) "
        "VALUES (%d, %d, %d)",
        record->kn_id, record->rank, record->content_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int KnowledgeBest_Get(DBConnectionManager* manager, int kn_id, signed char rank, KnowledgeBest* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT kn_id, rank, content_no FROM knowledge_best WHERE kn_id = %d AND rank = %d",
        kn_id, rank);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(KnowledgeBest));
    record->kn_id = row[0] ? atoi(row[0]) : 0;
    record->rank = row[1] ? atoi(row[1]) : 0;
    record->content_no = row[2] ? atoi(row[2]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int KnowledgeBest_GetAll(DBConnectionManager* manager, KnowledgeBest* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT kn_id, rank, content_no FROM knowledge_best");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(KnowledgeBest));
        records[count].kn_id = row[0] ? atoi(row[0]) : 0;
        records[count].rank = row[1] ? atoi(row[1]) : 0;
        records[count].content_no = row[2] ? atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
