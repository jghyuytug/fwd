#include "knowl_class_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int KnowlClass_Add(DBConnectionManager* manager, const KnowlClass* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO knowl_class (kn_id, sup_id, kn_name, cur_depth) "
        "VALUES (%d, %d, '%s', %d)",
        record->kn_id, record->sup_id, record->kn_name, record->cur_depth);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int KnowlClass_Get(DBConnectionManager* manager, int kn_id, KnowlClass* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT kn_id, sup_id, kn_name, cur_depth FROM knowl_class WHERE kn_id = %d",
        kn_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(KnowlClass));
    record->kn_id = row[0] ? atoi(row[0]) : 0;
    record->sup_id = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(record->kn_name, row[2], sizeof(record->kn_name) - 1);
    record->cur_depth = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int KnowlClass_GetAll(DBConnectionManager* manager, KnowlClass* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT kn_id, sup_id, kn_name, cur_depth FROM knowl_class");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_MAIN_WEB, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(KnowlClass));
        records[count].kn_id = row[0] ? atoi(row[0]) : 0;
        records[count].sup_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(records[count].kn_name, row[2], sizeof(records[count].kn_name) - 1);
        records[count].cur_depth = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
