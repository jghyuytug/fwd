#include "knowl_class_tree_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int KnowlClassTree_Add(DBConnectionManager* manager, const KnowlClassTree* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO knowl_class_tree (kn_id, up_id, depth) "
        "VALUES (%d, %d, %d)",
        record->kn_id, record->up_id, record->depth);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int KnowlClassTree_GetAll(DBConnectionManager* manager, KnowlClassTree* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT kn_id, up_id, depth FROM knowl_class_tree");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(KnowlClassTree));
        records[count].kn_id = row[0] ? atoi(row[0]) : 0;
        records[count].up_id = row[1] ? atoi(row[1]) : 0;
        records[count].depth = row[2] ? atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
